/*
 * =====================================================================================
 *
 *       Filename:  CBlock.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:24:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "BlockEngine.h"
#include "BlockEngine/CBlock.h"
#include "BlockEngine/CBlockIndex.h"
#include "BlockEngine/CDiskBlockIndex.h"
#include "BlockEngine/CDiskTxPos.h"
#include "NetWorkService/NetWorkServ.h"
#include "WalletService/CTxIndex.h"
#include "WalletService/WalletServ.h"
#include "DAO/DaoServ.h"
#include "CommonBase/ProtocSerialize.h"
#include "ProtocSrc/Block.pb.h"
using namespace Enze;

// 区块链接：每一个交易链接，增加到区块索引链中
bool CBlock::ConnectBlock(CBlockIndex* pindex)
{
    //// issue here: it doesn't know the version
    printf("error %s_%d\n", __FILE__, __LINE__);
    unsigned int nTxPos = pindex->m_nBlockPos;// + ::GetSerializeSize(CBlock(), SER_DISK) - 1 + GetSizeOfCompactSize(m_vTrans.size());

    map<uint256, CTxIndex> mapUnused;
    int64 nFees = 0;
    foreach(CTransaction& tx, m_vTrans)
    {
        CDiskTxPos posThisTx(pindex->m_nFile, pindex->m_nBlockPos, nTxPos);
        printf("%s---%d\n", __FILE__, __LINE__); 
        //nTxPos += ::GetSerializeSize(tx, SER_DISK);
		// 对每一个交易进行输入链接判断
        if (!tx.ConnectInputs(mapUnused, posThisTx, pindex->m_nCurHeight, nFees, true, false))
            return false;
    }
	// 币基交易中对应的输出不能大于整个对应的奖励+交易手续费
    if (m_vTrans[0].GetValueOut() > GetBlockValue(nFees))
        return false;

    // Update block index on disk without changing it in memory.
    // The memory index structure will be changed after the db commits.
    if (pindex->m_pPrevBlkIndex)
    {
		// 将当前区块索引 挂在 前一个区块索引之后
        CDiskBlockIndex blockindexPrev(pindex->m_pPrevBlkIndex);
        blockindexPrev.m_NextHash = pindex->GetBlockHash();
        DaoServ::getInstance()->WriteBlockIndex(blockindexPrev);
    }

	// 监视在block中哪些
    // Watch for transactions paying to me
    foreach(CTransaction& tx, m_vTrans)
        WalletServ::getInstance()->AddToWalletIfMine(tx, this);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// CBlock and CBlockIndex
//
// 根据区块索引从数据库文件中读取对应的区块信息
bool CBlock::ReadFromDisk(const CBlockIndex* pblockindex, bool fReadTransactions)
{
    return ReadFromDisk(pblockindex->m_nFile, pblockindex->m_nBlockPos, fReadTransactions);
}

// 获取这个区块对应的价值（奖励+交易手续费）
int64 CBlock::GetBlockValue(int64 nFees) const
{
    int nBestHeight = BlockEngine::getInstance()->getBestHeight();
	// 补贴;津贴，初始奖励是50个比特币
    int64 nSubsidy = 50 * COIN;

	// 奖励是每4年减一半，总量是2100万
	// nBestHeight 可以这样理解每产出210000块block则对应的奖励减半，而产生一个block需要10分钟
	// 则产生210000个block需要的时间是 210000*10/(60*24*360)=4.0509...（年） 将近于每4年减一半
    // Subsidy is cut in half every 4 years
    nSubsidy >>= (nBestHeight / 210000);

    return nSubsidy + nFees;
}

// 将一个区块block断开连接（就是释放区块对应的信息，同时释放区块对应的区块索引）
bool CBlock::DisconnectBlock(CBlockIndex* pindex)
{
	// 逆序释放交易的链接
    // Disconnect in reverse order
    for (int i = m_vTrans.size()-1; i >= 0; i--)
        if (!m_vTrans[i].DisconnectInputs())
            return false;

	// 更新区块索引
    // Update block index on disk without changing it in memory.
    // The memory index structure will be changed after the db commits.
    if (pindex->m_pPrevBlkIndex)
    {
		// 将当前区块索引对应的前一个区块索引的hashNext值为0，表示将当前区块索引从前一个区块索引链接上去除
        CDiskBlockIndex blockindexPrev(pindex->m_pPrevBlkIndex);
        blockindexPrev.m_NextHash = 0;
        DaoServ::getInstance()->WriteBlockIndex(blockindexPrev);
    }

    return true;
}

// 将当前区块增加到对应的区块索引链中mapBlockIndex
bool CBlock::AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos)
{
    printf("CBlock::AddToBlockIndex--- serail--nFile[%d]\n", nFile);
    const map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->getMapBlockIndex();
    // Check for duplicate
    uint256 hash = GetHash();
    if (mapBlockIndex.count(hash))
        return error("AddToBlockIndex() : %s already exists", hash.ToString().substr(0,14).c_str());
    printf("CBlock::AddToBlockIndex--- serail----111\n");
    // Construct new block index object
    CBlockIndex* pindexNew = new CBlockIndex(nFile, nBlockPos, *this);
    if (!pindexNew)
        return error("AddToBlockIndex() : new CBlockIndex failed");
    //map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.insert(make_pair(hash, pindexNew)).first;
    map<uint256, CBlockIndex*>::iterator mi = BlockEngine::getInstance()->insertBlockIndex(make_pair(hash, pindexNew));
    pindexNew->m_pHashBlock = &((*mi).first);
    map<uint256, CBlockIndex*>::const_iterator miPrev = mapBlockIndex.find(m_hashPrevBlock);
      printf("CBlock::AddToBlockIndex--- serail----222\n");
    if (miPrev != mapBlockIndex.end())
    {
        pindexNew->m_pPrevBlkIndex = (*miPrev).second;
		// 增加前一个区块索引对应的高度
        pindexNew->m_nCurHeight = pindexNew->m_pPrevBlkIndex->m_nCurHeight + 1;
    }
      printf("CBlock::AddToBlockIndex--- serail----333\n");
    DaoServ::getInstance()->WriteBlockIndex(CDiskBlockIndex(pindexNew));

	// 更新最长链对应的指针
    // New best
	// 新链的高度已经超过主链了（即是新链到创世区块的长度 大于 本节点认为的最长链到创世区块的长度
    const int nBestHeight = BlockEngine::getInstance()->getBestHeight();
    const uint256& hashGenesisBlock = BlockEngine::getInstance()->getGenesisHash();
    const CBlockIndex* pindexGenesisBlock = BlockEngine::getInstance()->getGenesisBlock();
    const uint256& hashBestChain = BlockEngine::getInstance()->getHashBestChain();
    const CBlockIndex* pindexBest = BlockEngine::getInstance()->getBestIndex();
      printf("CBlock::AddToBlockIndex--- serail----444--CurHeight[%d]---nBestHeight[%d]\n",pindexNew->m_nCurHeight, nBestHeight);
    if (pindexNew->m_nCurHeight > nBestHeight)
    {
		// 判断是否是创世区块
        if (pindexGenesisBlock == NULL && hash == hashGenesisBlock)
        {
            printf("CBlock::AddToBlockIndex--- serail----no pindexGenesisBlock\n");
            pindexGenesisBlock = pindexNew;
            DaoServ::getInstance()->WriteHashBestChain(hash);
        }
        else if (m_hashPrevBlock == hashBestChain)
        {
			// 如果当前块对应的前一个块是最长的链
            // Adding to current best branch
            if (!ConnectBlock(pindexNew) || !DaoServ::getInstance()->WriteHashBestChain(hash))
            {
                pindexNew->EraseBlockFromDisk();
                BlockEngine::getInstance()->eraseBlockIndex(pindexNew->GetBlockHash());
                delete pindexNew;
                return error("AddToBlockIndex() : ConnectBlock failed");
            }
			// 如果在最长链中，才设置对应区块索引的m_pNextBlkIndex字段，将当前区块索引设置在前一个区块索引的后面
            pindexNew->m_pPrevBlkIndex->m_pNextBlkIndex = pindexNew;

			// 如果对应的区块已经放入到主链中，则对应的区块交易应该要从本节点保存的交易内存池中删除
            // Delete redundant memory transactions
            foreach(CTransaction& tx, m_vTrans)
                tx.RemoveFromMemoryPool();
        }
        else
        {
			// 当前区块既不是创世区块，且当前区块对应的前一个区块也不在最长主链上的情况
			// 再加上新区块所在链的长度大于本节点认为主链的长度，所有将进行分叉处理
            // New best branch
            if (!BlockEngine::getInstance()->Reorganize(pindexNew))
            {
                return error("AddToBlockIndex() : Reorganize failed");
            }
        }

        printf("CBlock::AddToBlockIndex--- serail----888888\n");
        // New best link
        BlockEngine::getInstance()->setBestChain(hash);
        printf("CBlock::AddToBlockIndex--- serail----888--1111\n");
        BlockEngine::getInstance()->setBestIndex(pindexNew);
        printf("CBlock::AddToBlockIndex--- serail----8888--222\n");
        BlockEngine::getInstance()->setBestHeight(pindexNew->m_nCurHeight);
        printf("CBlock::AddToBlockIndex--- serail----888-333\n");
        WalletServ::getInstance()->nTransactionsUpdated++;
        printf("AddToBlockIndex: new best=%s  height=%d\n", hashBestChain.ToString().substr(0,14).c_str(), nBestHeight);
    }

      printf("CBlock::AddToBlockIndex--- serail----99999999\n");
	// 转播那些到目前为止还没有进入block中的钱包交易
    // Relay wallet transactions that haven't gotten in yet
    if (pindexNew == pindexBest)
        WalletServ::getInstance()->RelayWalletTransactions();// 在节点之间进行转播
  printf("CBlock::AddToBlockIndex--- serail----9999999910\n");
  //  MainFrameRepaint();
    return true;
}



// 区块校验
bool CBlock::CheckBlock() const
{
    // These are checks that are independent of context
    // that can be verified before saving an orphan 孤儿 block.

    printf("error %s_%d\n", __FILE__, __LINE__);
    // Size limits
    //if (m_vTrans.empty() || m_vTrans.size() > MAX_SIZE || ::GetSerializeSize(*this, SER_DISK) > MAX_SIZE)
    //    return error("CheckBlock() : size limits failed");

	// block的创建时间 相对于当前时间 早了2个小时
    // Check timestamp
    if (m_uTime > GetAdjustedTime() + 2 * 60 * 60)
        return error("CheckBlock() : block timestamp too far in the future");

	// 在块中币基交易一定要存在，而且仅仅只能存在一条
    // First transaction must be coinbase, the rest must not be
    if (m_vTrans.empty() || !m_vTrans[0].IsCoinBase())
        return error("CheckBlock() : first tx is not coinbase");
    for (int i = 1; i < m_vTrans.size(); i++)
        if (m_vTrans[i].IsCoinBase())
            return error("CheckBlock() : more than one coinbase");

	// 对块中的交易进行校验
    // Check transactions
    foreach(const CTransaction& tx, m_vTrans)
        if (!tx.CheckTransaction())
            return error("CheckBlock() : CheckTransaction failed");

	// 对工作量难度指标进行校验
    // Check proof of work matches claimed amount
    if (CBigNum().SetCompact(m_uBits) > bnProofOfWorkLimit)
        return error("CheckBlock() : m_uBits below minimum work");
	// 计算当前块的hash是否满足对应工作量难度指标
    if (GetHash() > CBigNum().SetCompact(m_uBits).getuint256())
        return error("CheckBlock() : hash doesn't match m_uBits");

	// 对默克尔树对应的根进行校验
    // Check merkleroot
    if (m_hashMerkleRoot != BuildMerkleTree())
        return error("CheckBlock() : hashMerkleRoot mismatch");

    return true;
}
// 判断当前区块能够被接收
bool CBlock::AcceptBlock()
{
    const map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->getMapBlockIndex();
    // Check for duplicate
    uint256 hash = GetHash();
    if (mapBlockIndex.count(hash)) 
        return error("AcceptBlock() : block already in mapBlockIndex");

    // Get prev block index
    map<uint256, CBlockIndex*>::const_iterator mi = mapBlockIndex.find(m_hashPrevBlock);
    if (mi == mapBlockIndex.end())
        return error("AcceptBlock() : prev block not found");
    CBlockIndex* pindexPrev = (*mi).second;

	// 当前块创建的时间要大于前一个块对应的中位数时间
    // Check timestamp against prev
    if (m_uTime <= pindexPrev->GetMedianTimePast())
        return error("AcceptBlock() : block's timestamp is too early");

	//工作量证明校验：每一个节点自己计算对应的工作量难度
    // Check proof of work
    if (m_uBits != BlockEngine::getInstance()->GetNextWorkRequired(pindexPrev))
        return error("AcceptBlock() : incorrect proof of work");

    printf("error %s_%d\n", __FILE__, __LINE__);
    // Write block to history file
    //if (!BlockEngine::getInstance()->CheckDiskSpace(::GetSerializeSize(*this, SER_DISK)))
    //    return error("AcceptBlock() : out of disk space");
    unsigned int nFile;
    unsigned int nBlockPos;
	// 将块信息写入文件中
    bool fClient = false; 
    if (!WriteToDisk(!fClient, nFile, nBlockPos))
        return error("AcceptBlock() : WriteToDisk failed");
	// 增加块对应的快索引信息
    if (!AddToBlockIndex(nFile, nBlockPos))
        return error("AcceptBlock() : AddToBlockIndex failed");

    if (BlockEngine::getInstance()->getHashBestChain() == hash)
        Enze::NetWorkServ::getInstance()->RelayInventory(CInv(MSG_BLOCK, hash));

    // // Add atoms to user reviews for coins created
    // vector<unsigned char> vchPubKey;
    // if (ExtractPubKey(vtx[0].vout[0].m_cScriptPubKey, false, vchPubKey))
    // {
    //     unsigned short nAtom = GetRand(USHRT_MAX - 100) + 100;
    //     vector<unsigned short> vAtoms(1, nAtom);
    //     AddAtomsAndPropagate(Hash(vchPubKey.begin(), vchPubKey.end()), vAtoms, true);
    // }

    return true;
}

// 根据交易构建对应的默克尔树
uint256 CBlock::BuildMerkleTree() const
{
    m_vMerkleTree.clear();
    foreach(const CTransaction& tx, m_vTrans)
        m_vMerkleTree.push_back(tx.GetHash());
    int j = 0;
    for (int nSize = m_vTrans.size(); nSize > 1; nSize = (nSize + 1) / 2)
    {
        for (int i = 0; i < nSize; i += 2)
        {
            int i2 = min(i+1, nSize-1);
            m_vMerkleTree.push_back(Hash(BEGIN(m_vMerkleTree[j+i]),  END(m_vMerkleTree[j+i]),
                                       BEGIN(m_vMerkleTree[j+i2]), END(m_vMerkleTree[j+i2])));
        }
        j += nSize;
    }
    return (m_vMerkleTree.empty() ? 0 : m_vMerkleTree.back());
}
// 根据交易对应的索引获得交易对应的默克尔分支
vector<uint256> CBlock::GetMerkleBranch(int nIndex) const
{
    if (m_vMerkleTree.empty())
        BuildMerkleTree();
    vector<uint256> vMerkleBranch;
    int j = 0;
    for (int nSize = m_vTrans.size(); nSize > 1; nSize = (nSize + 1) / 2)
    {
        int i = min(nIndex^1, nSize-1);
        vMerkleBranch.push_back(m_vMerkleTree[j+i]);
        nIndex >>= 1;
        j += nSize;
    }
    return vMerkleBranch;
}
// 判断当前对应的交易hash和默克尔分支来验证对应的交易是否在对应的blcok中
uint256 CBlock::CheckMerkleBranch(uint256 hash, const vector<uint256>& vMerkleBranch, int nIndex)
{
    if (nIndex == -1)
        return 0;
    foreach(const uint256& otherside, vMerkleBranch)
    {
        if (nIndex & 1)
            hash = Hash(BEGIN(otherside), END(otherside), BEGIN(hash), END(hash));
        else
            hash = Hash(BEGIN(hash), END(hash), BEGIN(otherside), END(otherside));
        nIndex >>= 1;
    }
    return hash;
}

// 将block写入到文件中
bool CBlock::WriteToDisk(bool fWriteTransactions, unsigned int& nFileRet, unsigned int& nBlockPosRet)
{
    // Open history file to append
    CAutoFile fileout = BlockEngine::getInstance()->AppendBlockFile(nFileRet);
    if (!fileout)
        return error("CBlock::WriteToDisk() : AppendBlockFile failed");
    if (!fWriteTransactions)
        fileout.nType |= SER_BLOCKHEADERONLY;

    Block cProtocData;
    printf("CBlock::WriteToDisk---start serail\n");
    if (!SeriaBlock(*this, cProtocData))
    {
         printf("CBlock::WriteToDisk---fault serail\n");
        return false;
    }
    printf("CBlock::WriteToDisk---end serail\n");
    string strSerial;
    cProtocData.SerializePartialToString(&strSerial);

    // Write index header
    unsigned int nSize = strSerial.length();//fileout.GetSerializeSize(*this);
        printf("CBlock::WriteToDisk--- serail--Len[%d]\n", nSize);
    nBlockPosRet = ftell(fileout);

    if (nBlockPosRet == -1)
        return error("CBlock::WriteToDisk() : ftell failed");
    // 放入消息头和对应块的大小值
    fileout.write((const char*)&nSize, sizeof(nSize));
     printf("CBlock::WriteToDisk--- serail--Len[%d]---2\n", nSize);
    // Write block
    // 将block的内容写入到文件中
    fileout.write(strSerial.c_str(), nSize);
     printf("CBlock::WriteToDisk--- serail--Len[%d]--3\n", nSize);
    return true;
}

// 从文件中读取块信息
bool CBlock::ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions)
{
    SetNull();

    // Open history file to read
    CAutoFile filein = BlockEngine::getInstance()->OpenBlockFile(nFile, nBlockPos, "rb");
    if (!filein)
        return error("CBlock::ReadFromDisk() : OpenBlockFile failed");
    if (!fReadTransactions)
        filein.nType |= SER_BLOCKHEADERONLY;

    // Read block 将文件中的内容读取到块中
    unsigned int nSize = 0;
    filein.read((char*)&nSize, sizeof(nSize));
    if (-1 == nSize)
        return error("CBlock::ReadFromDisk, Get Block Size");
    char *buf = (char*)malloc(nSize);
    filein.read(buf, nSize);
    Block cProtoc;
    cProtoc.ParsePartialFromArray(buf, nSize);
    UnSeriaBlock(cProtoc, *this);
  //  filein >> *this;

    // Check the header 1. 工作量证明难度比较 2. 计算的hash值要小于对应的工作量难度
    if (CBigNum().SetCompact(m_uBits) > bnProofOfWorkLimit)
        return error("CBlock::ReadFromDisk() : m_uBits errors in block header");
    if (GetHash() > CBigNum().SetCompact(m_uBits).getuint256())
        return error("CBlock::ReadFromDisk() : GetHash() errors in block header");

    return true;
}


/* EOF */

