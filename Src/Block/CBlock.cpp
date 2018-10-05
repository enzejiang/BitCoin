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
#include "Block/CBlock.h"
#include "Block/CBlockIndex.h"
#include "Block/CDiskBlockIndex.h"
#include "Block/CDiskTxPos.h"
#include "Db/CTxDB.h"
#include "Network/net.h"
#include "TX/CTxIndex.h"

// 区块链接：每一个交易链接，增加到区块索引链中
bool CBlock::ConnectBlock(CTxDB& txdb, CBlockIndex* pindex)
{
    //// issue here: it doesn't know the version
    unsigned int nTxPos = pindex->m_nBlockPos + ::GetSerializeSize(CBlock(), SER_DISK) - 1 + GetSizeOfCompactSize(m_vTrans.size());

    map<uint256, CTxIndex> mapUnused;
    int64 nFees = 0;
    foreach(CTransaction& tx, m_vTrans)
    {
        CDiskTxPos posThisTx(pindex->m_nFile, pindex->m_nBlockPos, nTxPos);
        nTxPos += ::GetSerializeSize(tx, SER_DISK);
		// 对每一个交易进行输入链接判断
        if (!tx.ConnectInputs(txdb, mapUnused, posThisTx, pindex->m_nCurHeight, nFees, true, false))
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
        txdb.WriteBlockIndex(blockindexPrev);
    }

	// 监视在block中哪些
    // Watch for transactions paying to me
    foreach(CTransaction& tx, m_vTrans)
        AddToWalletIfMine(tx, this);
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
bool CBlock::DisconnectBlock(CTxDB& txdb, CBlockIndex* pindex)
{
	// 逆序释放交易的链接
    // Disconnect in reverse order
    for (int i = m_vTrans.size()-1; i >= 0; i--)
        if (!m_vTrans[i].DisconnectInputs(txdb))
            return false;

	// 更新区块索引
    // Update block index on disk without changing it in memory.
    // The memory index structure will be changed after the db commits.
    if (pindex->m_pPrevBlkIndex)
    {
		// 将当前区块索引对应的前一个区块索引的hashNext值为0，表示将当前区块索引从前一个区块索引链接上去除
        CDiskBlockIndex blockindexPrev(pindex->m_pPrevBlkIndex);
        blockindexPrev.m_NextHash = 0;
        txdb.WriteBlockIndex(blockindexPrev);
    }

    return true;
}

// 将当前区块增加到对应的区块索引链中mapBlockIndex
bool CBlock::AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos)
{
    // Check for duplicate
    uint256 hash = GetHash();
    if (mapBlockIndex.count(hash))
        return error("AddToBlockIndex() : %s already exists", hash.ToString().substr(0,14).c_str());

    // Construct new block index object
    CBlockIndex* pindexNew = new CBlockIndex(nFile, nBlockPos, *this);
    if (!pindexNew)
        return error("AddToBlockIndex() : new CBlockIndex failed");
    map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.insert(make_pair(hash, pindexNew)).first;
    pindexNew->m_pHashBlock = &((*mi).first);
    map<uint256, CBlockIndex*>::iterator miPrev = mapBlockIndex.find(m_hashPrevBlock);
    if (miPrev != mapBlockIndex.end())
    {
        pindexNew->m_pPrevBlkIndex = (*miPrev).second;
		// 增加前一个区块索引对应的高度
        pindexNew->m_nCurHeight = pindexNew->m_pPrevBlkIndex->m_nCurHeight + 1;
    }

    CTxDB txdb;
    txdb.TxnBegin();
    txdb.WriteBlockIndex(CDiskBlockIndex(pindexNew));

	// 更新最长链对应的指针
    // New best
	// 新链的高度已经超过主链了（即是新链到创世区块的长度 大于 本节点认为的最长链到创世区块的长度
    if (pindexNew->m_nCurHeight > nBestHeight)
    {
		// 判断是否是创世区块
        if (pindexGenesisBlock == NULL && hash == hashGenesisBlock)
        {
            pindexGenesisBlock = pindexNew;
            txdb.WriteHashBestChain(hash);
        }
        else if (m_hashPrevBlock == hashBestChain)
        {
			// 如果当前块对应的前一个块是最长的链
            // Adding to current best branch
            if (!ConnectBlock(txdb, pindexNew) || !txdb.WriteHashBestChain(hash))
            {
                txdb.TxnAbort();
                pindexNew->EraseBlockFromDisk();
                mapBlockIndex.erase(pindexNew->GetBlockHash());
                delete pindexNew;
                return error("AddToBlockIndex() : ConnectBlock failed");
            }
            txdb.TxnCommit();
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
            if (!Reorganize(txdb, pindexNew))
            {
                txdb.TxnAbort();
                return error("AddToBlockIndex() : Reorganize failed");
            }
        }

        // New best link
        hashBestChain = hash;
        pindexBest = pindexNew;
        nBestHeight = pindexBest->m_nCurHeight;
        nTransactionsUpdated++;
        printf("AddToBlockIndex: new best=%s  height=%d\n", hashBestChain.ToString().substr(0,14).c_str(), nBestHeight);
    }

    txdb.TxnCommit();
    txdb.Close();

	// 转播那些到目前为止还没有进入block中的钱包交易
    // Relay wallet transactions that haven't gotten in yet
    if (pindexNew == pindexBest)
        RelayWalletTransactions();// 在节点之间进行转播

  //  MainFrameRepaint();
    return true;
}



// 区块校验
bool CBlock::CheckBlock() const
{
    // These are checks that are independent of context
    // that can be verified before saving an orphan 孤儿 block.

    // Size limits
    if (m_vTrans.empty() || m_vTrans.size() > MAX_SIZE || ::GetSerializeSize(*this, SER_DISK) > MAX_SIZE)
        return error("CheckBlock() : size limits failed");

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
    // Check for duplicate
    uint256 hash = GetHash();
    if (mapBlockIndex.count(hash)) 
        return error("AcceptBlock() : block already in mapBlockIndex");

    // Get prev block index
    map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(m_hashPrevBlock);
    if (mi == mapBlockIndex.end())
        return error("AcceptBlock() : prev block not found");
    CBlockIndex* pindexPrev = (*mi).second;

	// 当前块创建的时间要大于前一个块对应的中位数时间
    // Check timestamp against prev
    if (m_uTime <= pindexPrev->GetMedianTimePast())
        return error("AcceptBlock() : block's timestamp is too early");

	//工作量证明校验：每一个节点自己计算对应的工作量难度
    // Check proof of work
    if (m_uBits != GetNextWorkRequired(pindexPrev))
        return error("AcceptBlock() : incorrect proof of work");

    // Write block to history file
    if (!CheckDiskSpace(::GetSerializeSize(*this, SER_DISK)))
        return error("AcceptBlock() : out of disk space");
    unsigned int nFile;
    unsigned int nBlockPos;
	// 将块信息写入文件中
    if (!WriteToDisk(!fClient, nFile, nBlockPos))
        return error("AcceptBlock() : WriteToDisk failed");
	// 增加块对应的快索引信息
    if (!AddToBlockIndex(nFile, nBlockPos))
        return error("AcceptBlock() : AddToBlockIndex failed");

    if (hashBestChain == hash)
        RelayInventory(CInv(MSG_BLOCK, hash));

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


/* EOF */

