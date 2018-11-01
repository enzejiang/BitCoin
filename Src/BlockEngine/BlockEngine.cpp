/*
 * =====================================================================================
 *
 *       Filename:  BlockEngine.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/05/2018 11:44:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "BlockEngine.h"
#include "headers.h"
#include "sha.h"
#include "market.h"
#include "util.h"
#include "CommonBase/CommDataDef.h"
#include "CommonBase/bignum.h"
#include "CommonBase/uint256.h"
#include "CommonBase/base58.h"
#include "NetWorkService/NetWorkServ.h"
#include"NetWorkService/ZMQNode.h"
#include "BlockEngine/CBlock.h"
#include "BlockEngine/CBlockIndex.h"
#include "BlockEngine/CBlockLocator.h"
#include "DAO/DaoServ.h"
#include "WalletService/WalletServ.h"
#include "WalletService/CTxIndex.h"
using namespace Enze;

BlockEngine* BlockEngine::m_pInstance = NULL;
pthread_mutex_t BlockEngine::m_mutexLock;

BlockEngine* BlockEngine::getInstance()
{
    if (NULL == m_pInstance)
    {
        pthread_mutex_lock(&m_mutexLock); 
        
        if (NULL == m_pInstance) 
        {
            m_pInstance = new BlockEngine();
        }
        
        pthread_mutex_unlock(&m_mutexLock); 
    
    }
    
    return m_pInstance;
}

void BlockEngine::Destory()
{
    pthread_mutex_lock(&m_mutexLock); 
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
    pthread_mutex_unlock(&m_mutexLock); 

}

BlockEngine::BlockEngine()
: hashGenesisBlock("0xf510d86af37ae70980806706913a812d0b30350572dda9c748de079f3805dd76")
{

    pindexGenesisBlock = NULL; // 基础块对应的索引，也即是创世区块对应的索引
    nBestHeight = -1; // 最长链对应的区块个数，从创世区块到当前主链最后一个区块，中间隔了多少个区块
    hashBestChain = 0; // 最长链最后一个区块对应的hash
    pindexBest = NULL; // 记录当前最长链主链对应的区块索引指针


    // Settings
    fGenerateBitcoins = 1; // 是否挖矿，产生比特币
}

BlockEngine::~BlockEngine()
{

}

void BlockEngine::initiation()
{
    //
    // Load data files
    //
    string strErrors;
    int64 nStart, nEnd;
    printf("Loading block index...\n");
    if (!LoadBlockIndex())
        strErrors += "Error loading blkindex.dat      \n";
    printf(" block index %20I64d\n", nEnd - nStart);

    if (!strErrors.empty())
    {
        printf("Initiation Error\n");
        exit(-1);
    }

}












// 获取孤儿块对应的根
uint256 BlockEngine::GetOrphanRoot(const CBlock* pblock)
{
    // Work back to the first block in the orphan chain
    while (mapOrphanBlocks.count(pblock->m_hashPrevBlock))
        pblock = mapOrphanBlocks[pblock->m_hashPrevBlock];
    return pblock->GetHash();
}


// 根据前一个block对应的工作量获取下一个block获取需要的工作量
unsigned int BlockEngine::GetNextWorkRequired(const CBlockIndex* pindexLast)
{
    const unsigned int nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
    const unsigned int nTargetSpacing = 10 * 60; // 10分钟产生一个block
	// 每隔2016个块对应的工作量难度就需要重新计算一次
    const unsigned int nInterval = nTargetTimespan / nTargetSpacing; // 中间隔了多少个block 2016个块
 //   printf("GetNextWorkRequired---bnProofOfWorkLimit[%0x]\n", bnProofOfWorkLimit.GetCompact());
	// 说明当前块是一个创世区块，因为当前块对应的前一个区块为空
    // Genesis block
    if (pindexLast == NULL)
        return bnProofOfWorkLimit.GetCompact();

	// 如果不等于0不进行工作量难度改变
    // Only change once per interval
    if ((pindexLast->m_nCurHeight+1) % nInterval != 0)
        return pindexLast->m_uBits;

	// 往前推2016个区块
    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < nInterval-1; i++)
        pindexFirst = pindexFirst->m_pPrevBlkIndex;
    assert(pindexFirst);

	// 当前区块的前一个区块创建时间 减去 从当前区块向前推2016个区块得到区块创建时间
    // Limit adjustment step
    unsigned int nActualTimespan = pindexLast->m_uTime - pindexFirst->m_uTime;
    printf("  nActualTimespan = %d  before bounds\n", nActualTimespan);
    // 控制目标难度调整的跨度不能太大
	if (nActualTimespan < nTargetTimespan/4)
        nActualTimespan = nTargetTimespan/4;
    if (nActualTimespan > nTargetTimespan*4)
        nActualTimespan = nTargetTimespan*4;

	// 重新目标计算难度：当前区块对应的前一个区块对应的目标难度 * 实际2016区块对应的创建时间间隔 / 目标时间跨度14天
    // Retarget
    CBigNum bnNew;
    bnNew.SetCompact(pindexLast->m_uBits);
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

	// 如果计算的工作量难度（值越大对应的工作难度越小）小于当前对应的工作量难度
    if (bnNew > bnProofOfWorkLimit)
        bnNew = bnProofOfWorkLimit;

    /// debug print
    printf("\n\n\nGetNextWorkRequired RETARGET *****\n");
    printf("nTargetTimespan = %d    nActualTimespan = %d\n", nTargetTimespan, nActualTimespan);
    printf("Before: %08x  %s\n", pindexLast->m_uBits, CBigNum().SetCompact(pindexLast->m_uBits).getuint256().ToString().c_str());
    printf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());

    return bnNew.GetCompact();
}





// 重新组织区块的索引：因为此时已经出现区块链分叉
bool BlockEngine::Reorganize(CBlockIndex* pindexNew)
{
    printf("*** REORGANIZE ***\n");

	// 找到区块分叉点
    // Find the fork
    CBlockIndex* pfork = pindexBest;
    CBlockIndex* plonger = pindexNew;
	// 找到主链和分叉链对应的交叉点
    while (pfork != plonger)
    {
        if (!(pfork = pfork->m_pPrevBlkIndex))
            return error("Reorganize() : pfork->m_pPrevBlkIndex is null");
        while (plonger->m_nCurHeight > pfork->m_nCurHeight)
            if (!(plonger = plonger->m_pPrevBlkIndex))
                return error("Reorganize() : plonger->m_pPrevBlkIndex is null");
    }

	// 列举出当前节点认为的最长链中（从当前最长链到交叉点）失去连接的块
    // List of what to disconnect
    vector<CBlockIndex*> vDisconnect;
    for (CBlockIndex* pindex = pindexBest; pindex != pfork; pindex = pindex->m_pPrevBlkIndex)
        vDisconnect.push_back(pindex);

	// 获取需要连接的块，因为自己认为的最长链实际上不是最长链
    // List of what to connect
    vector<CBlockIndex*> vConnect;
    for (CBlockIndex* pindex = pindexNew; pindex != pfork; pindex = pindex->m_pPrevBlkIndex)
        vConnect.push_back(pindex);
	// 因为上面放入的时候是倒着放的，所以这里在将这个逆序，得到正向的
    reverse(vConnect.begin(), vConnect.end());

	// 释放断链（仅仅释放对应的block链，对应的block索引链还没有释放）
    // Disconnect shorter branch
    vector<CTransaction> vResurrect;
    foreach(CBlockIndex* pindex, vDisconnect)
    {
        CBlock block;
        if (!block.ReadFromDisk(pindex->m_nFile, pindex->m_nBlockPos, true))
            return error("Reorganize() : ReadFromDisk for disconnect failed");
        if (!block.DisconnectBlock(pindex))
            return error("Reorganize() : DisconnectBlock failed");

		// 将释放块中的交易放入vResurrect，等待复活
        // Queue memory transactions to resurrect
        foreach(const CTransaction& tx, block.m_vTrans)
            if (!tx.IsCoinBase())
                vResurrect.push_back(tx);
    }

	// 连接最长的分支
    // Connect longer branch
    vector<CTransaction> vDelete;
    for (int i = 0; i < vConnect.size(); i++)
    {
        CBlockIndex* pindex = vConnect[i];
        CBlock block;
        if (!block.ReadFromDisk(pindex->m_nFile, pindex->m_nBlockPos, true))
            return error("Reorganize() : ReadFromDisk for connect failed");
        if (!block.ConnectBlock(pindex))
        {
			// 如果block连接失败之后，说明这个block无效，则删除这块之后的分支
            // Invalid block, delete the rest of this branch
            for (int j = i; j < vConnect.size(); j++)
            {
                CBlockIndex* pindex = vConnect[j];
                pindex->EraseBlockFromDisk();
                DaoServ::getInstance()->EraseBlockIndex(pindex->GetBlockHash());
                mapBlockIndex.erase(pindex->GetBlockHash());
                delete pindex;
            }
            return error("Reorganize() : ConnectBlock failed");
        }
		// 将加入区块链的块中的交易从对应的内存中删除
        // Queue memory transactions to delete
        foreach(const CTransaction& tx, block.m_vTrans)
            vDelete.push_back(tx);
    }
	// 写入最长链
    if (!DaoServ::getInstance()->WriteHashBestChain(pindexNew->GetBlockHash()))
        return error("Reorganize() : WriteHashBestChain failed");


	// 释放对应的块索引链
    // Disconnect shorter branch
    foreach(CBlockIndex* pindex, vDisconnect)
        if (pindex->m_pPrevBlkIndex)
            pindex->m_pPrevBlkIndex->m_pNextBlkIndex = NULL; // 表示这些块没有在主链上

	// 形成一条主链的块索引链
    // Connect longer branch
    foreach(CBlockIndex* pindex, vConnect)
        if (pindex->m_pPrevBlkIndex)
            pindex->m_pPrevBlkIndex->m_pNextBlkIndex = pindex;

	// 从释放链接的分支中获取对应的交易，将这些交易放入对应的全局变量中得到复活
    // Resurrect memory transactions that were in the disconnected branch
    foreach(CTransaction& tx, vResurrect)
        tx.AcceptTransaction(false);

	// 从全局变量中删除那些已经在主链中的交易
    // Delete redundant memory transactions that are in the connected branch
    foreach(CTransaction& tx, vDelete)
        tx.RemoveFromMemoryPool();

    return true;
}

// 处理区块，不管是接收到的还是自己挖矿得到的
bool BlockEngine::ProcessBlock(ZNode* pfrom, CBlock* pblock)
{
    // Check for duplicate
    uint256 hash = pblock->GetHash();
    if (mapBlockIndex.count(hash))
        return error("ProcessBlock() : already have block %d %s", mapBlockIndex[hash]->m_nCurHeight, hash.ToString().substr(0,14).c_str());
    if (mapOrphanBlocks.count(hash))
        return error("ProcessBlock() : already have block (orphan) %s", hash.ToString().substr(0,14).c_str());

    // Preliminary checks 初步娇艳
    if (!pblock->CheckBlock())
    {
        delete pblock;
        return error("ProcessBlock() : CheckBlock FAILED");
    }

    // If don't already have its previous block, shunt it off to holding area until we get it
    if (!mapBlockIndex.count(pblock->m_hashPrevBlock))
    {
        printf("ProcessBlock: ORPHAN BLOCK, prev=%s\n", pblock->m_hashPrevBlock.ToString().substr(0,14).c_str());
        mapOrphanBlocks.insert(make_pair(hash, pblock));
        mapOrphanBlocksByPrev.insert(make_pair(pblock->m_hashPrevBlock, pblock));

        // Ask this guy to fill in what we're missing
        if (pfrom)
            pfrom->SendGetBlocks(CBlockLocator(pindexBest), GetOrphanRoot(pblock));
        return true;
    }

    // Store to disk
    if (!pblock->AcceptBlock())
    {
        delete pblock;
        return error("ProcessBlock() : AcceptBlock FAILED");
    }
    delete pblock;

    // Recursively process any orphan blocks that depended on this one
    vector<uint256> vWorkQueue;
    vWorkQueue.push_back(hash);
    for (int i = 0; i < vWorkQueue.size(); i++)
    {
        uint256 hashPrev = vWorkQueue[i];
        for (multimap<uint256, CBlock*>::iterator mi = mapOrphanBlocksByPrev.lower_bound(hashPrev);
             mi != mapOrphanBlocksByPrev.upper_bound(hashPrev);
             ++mi)
        {
            CBlock* pblockOrphan = (*mi).second;
            if (pblockOrphan->AcceptBlock())
                vWorkQueue.push_back(pblockOrphan->GetHash());
            mapOrphanBlocks.erase(pblockOrphan->GetHash());
            delete pblockOrphan;
        }
        mapOrphanBlocksByPrev.erase(hashPrev);
    }

    printf("ProcessBlock: ACCEPTED\n");
    return true;
}








template<typename Stream>
bool ScanMessageStart(Stream& s)
{
    // Scan ahead to the next pchMessageStart, which should normally be immediately
    // at the file pointer.  Leaves file pointer at end of pchMessageStart.
    s.clear(0);
    short prevmask = s.exceptions(0);
    const char* p = BEGIN(pchMessageStart);
    try
    {
        loop
        {
            char c;
            s.read(&c, 1);
            if (s.fail())
            {
                s.clear(0);
                s.exceptions(prevmask);
                return false;
            }
            if (*p != c)
                p = BEGIN(pchMessageStart);
            if (*p == c)
            {
                if (++p == END(pchMessageStart))
                {
                    s.clear(0);
                    s.exceptions(prevmask);
                    return true;
                }
            }
        }
    }
    catch (...)
    {
        s.clear(0);
        s.exceptions(prevmask);
        return false;
    }
}

string BlockEngine::GetAppDir()
{
    string strDir;
    if (!strSetDataDir.empty())
    {
        strDir = strSetDataDir;
    }
    else if (getenv("APPDATA"))
    {
        //strDir = strprintf("%s\\Bitcoin", getenv("APPDATA"));
		strDir = strprintf("%s\\Bitcoin-debug", getenv("APPDATA"));
    }
    else if (getenv("USERPROFILE"))
    {
        string strAppData = strprintf("%s\\Application Data", getenv("USERPROFILE"));
        static bool fMkdirDone;
        if (!fMkdirDone)
        {
            fMkdirDone = true;
            mkdir(strAppData.c_str(), 775);
        }
       // strDir = strprintf("%s\\Bitcoin", strAppData.c_str());
		strDir = strprintf("%s\\Bitcoin-debug", strAppData.c_str());
    }
    else
    {
        return ".";
    }
    static bool fMkdirDone;
    if (!fMkdirDone)
    {
        fMkdirDone = true;
        mkdir(strDir.c_str(), 775);
    }
    return strDir;
}

bool BlockEngine::CheckDiskSpace(int64 nAdditionalBytes)
{
#if 0
    uint64 nFreeBytesAvailable = 0;     // bytes available to caller
    uint64 nTotalNumberOfBytes = 0;     // bytes on disk
    uint64 nTotalNumberOfFreeBytes = 0; // free bytes on disk

    if (!GetDiskFreeSpaceEx(GetAppDir().c_str(),
            (PULARGE_INTEGER)&nFreeBytesAvailable,
            (PULARGE_INTEGER)&nTotalNumberOfBytes,
            (PULARGE_INTEGER)&nTotalNumberOfFreeBytes))
    {
        printf("ERROR: GetDiskFreeSpaceEx() failed\n");
        return true;
    }

    // Check for 15MB because database could create another 10MB log file at any time
    if ((int64)nFreeBytesAvailable < 15000000 + nAdditionalBytes)
    {
        fShutdown = true;
        //wxMessageBox("Warning: Your disk space is low  ", "Bitcoin", wxICON_EXCLAMATION);
        _beginthread(Shutdown, 0, NULL);
        return false;
    }
#endif
    return true;
}

// 打开块文件
// 知道块文件对应nFile值就可以知道其对应的文件名：blk${nFile}.dat
FILE* BlockEngine::OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode)
{
    if (nFile == -1)
        return NULL;
    FILE* file = fopen(strprintf("%s\/blk%04d.dat", GetAppDir().c_str(), nFile).c_str(), pszMode);
    if (!file)
        return NULL;
    if (nBlockPos != 0 && !strchr(pszMode, 'a') && !strchr(pszMode, 'w'))
    {
		// 在文件中根据块的偏移进行定位文件指针
        if (fseek(file, nBlockPos, SEEK_SET) != 0)
        {
            fclose(file);
            return NULL;
        }
    }
    return file;
}

// 全局静态变量来控制对应的当前block对应的文件编号，也即是文件名称
// 每一个block对应一个单独的文件
static unsigned int nCurrentBlockFile = 1;

// 返回当前block应该在的文件指针
FILE* BlockEngine::AppendBlockFile(unsigned int& nFileRet)
{
    nFileRet = 0;
    loop
    {
        FILE* file = OpenBlockFile(nCurrentBlockFile, 0, "ab");
        if (!file)
            return NULL;
        if (fseek(file, 0, SEEK_END) != 0)
            return NULL;
        // FAT32 filesize max 4GB, fseek and ftell max 2GB, so we must stay under 2GB
        if (ftell(file) < 0x7F000000 - MAX_SIZE)
        {
            nFileRet = nCurrentBlockFile;
            return file;
        }
        fclose(file);
        nCurrentBlockFile++;
    }
}

bool BlockEngine::LoadBlockIndex(bool fAllowNew)
{
    //
    // Load block index
    //
    if (!DaoServ::getInstance()->LoadBlockIndex())
        return false;

    //
    // Init with genesis block
    //
    if (mapBlockIndex.empty())
    {
        if (!fAllowNew)
            return false;


        // Genesis Block:
        // GetHash()      = 0xdc8be64865ce298ca87ebee785b15fa29240e60db149c2ba85666e702f3eb734
        // hashMerkleRoot = 0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b
        // txNew.vin[0].scriptSig     = 486604799 4 0x736B6E616220726F662074756F6C69616220646E6F63657320666F206B6E697262206E6F20726F6C6C65636E61684320393030322F6E614A2F33302073656D695420656854
        // txNew.vout[0].m_nValue       = 5000000000
        // txNew.vout[0].m_cScriptPubKey = 0x5F1DF16B2B704C8A578D0BBAF74D385CDE12C11EE50455F3C438EF4C3FBCF649B6DE611FEAE06279A60939E028A8D65C10B73071A6F16719274855FEB0FD8A6704 OP_CHECKSIG
        // block.nVersion = 1
        // block.m_uTime    = 1231006505
        // block.m_uBits    = 0x1d00ffff
        // block.m_uNonce   = 2083236893
        // CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, m_uTime=1231006505, m_uBits=1d00ffff, m_uNonce=2083236893, vtx=1)
        //   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        //     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
        //     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
        //   vMerkleTree: 4a5e1e

        // Genesis block
        char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
        CTransaction txNew;
        txNew.m_vTxIn.resize(1);
        txNew.m_vTxOut.resize(1);
        txNew.m_vTxIn[0].m_cScriptSig     = CScript() << 486604799 << CBigNum(4) << vector<unsigned char>((unsigned char*)pszTimestamp, (unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.m_vTxOut[0].m_nValue       = 50 * COIN;
        txNew.m_vTxOut[0].m_cScriptPubKey = CScript() << CBigNum("0x5F1DF16B2B704C8A578D0BBAF74D385CDE12C11EE50455F3C438EF4C3FBCF649B6DE611FEAE06279A60939E028A8D65C10B73071A6F16719274855FEB0FD8A6704") << OP_CHECKSIG;
        CBlock block;
        block.m_vTrans.push_back(txNew);
        block.m_hashPrevBlock = 0;
        block.m_hashMerkleRoot = block.BuildMerkleTree();
        block.m_nCurVersion = 1;
        block.m_uTime    = 1231006505;
        block.m_uBits    = bnProofOfWorkLimit.GetCompact();//0x1d00ffff;
        block.m_uNonce   = 2083236893;

        //// debug print, delete this later
        printf("%s\n", block.GetHash().ToString().c_str());
        printf("%s\n", block.m_hashMerkleRoot.ToString().c_str());
       // printf("%s\n", hashGenesisBlock.ToString().c_str());
       // txNew.m_vTxOut[0].m_cScriptPubKey.print();
        //block.print();
        //assert(block.m_hashMerkleRoot == uint256("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));
        assert(block.m_hashMerkleRoot == uint256("0x0e5ce7366413c7c0dc91923b90a7a9cf1575ece3b492baf74e02cc6b67c52845"));

        assert(block.GetHash() == hashGenesisBlock);

        // Start new block file
        unsigned int nFile;
        unsigned int nBlockPos;
        bool fClient = false;
        printf("BlockEngine::LoadBlockInde--start write to disk\n");
        if (!block.WriteToDisk(!fClient, nFile, nBlockPos))
            return error("LoadBlockIndex() : writing genesis block to disk failed");
        if (!block.AddToBlockIndex(nFile, nBlockPos))
            return error("LoadBlockIndex() : genesis block not accepted");
    }

    return true;
}



void BlockEngine::PrintBlockTree()
{
    // precompute tree structure
    map<CBlockIndex*, vector<CBlockIndex*> > mapNext;
    for (map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.begin(); mi != mapBlockIndex.end(); ++mi)
    {
        CBlockIndex* pindex = (*mi).second;
        mapNext[pindex->m_pPrevBlkIndex].push_back(pindex);
        // test
        //while (rand() % 3 == 0)
        //    mapNext[pindex->m_pPrevBlkIndex].push_back(pindex);
    }

    vector<pair<int, CBlockIndex*> > vStack;
    vStack.push_back(make_pair(0, pindexGenesisBlock));

    int nPrevCol = 0;
    while (!vStack.empty())
    {
        int nCol = vStack.back().first;
        CBlockIndex* pindex = vStack.back().second;
        vStack.pop_back();

        // print split or gap
        if (nCol > nPrevCol)
        {
            for (int i = 0; i < nCol-1; i++)
                printf("| ");
            printf("|\\\n");
        }
        else if (nCol < nPrevCol)
        {
            for (int i = 0; i < nCol; i++)
                printf("| ");
            printf("|\n");
        }
        nPrevCol = nCol;

        // print columns
        for (int i = 0; i < nCol; i++)
            printf("| ");

        // print item
        CBlock block;
        block.ReadFromDisk(pindex, true);
#if 0
        printf("%d (%u,%u) %s  %s  tx %d",
            pindex->m_nCurHeight,
            pindex->nFile,
            pindex->nBlockPos,
            block.GetHash().ToString().substr(0,14).c_str(),
            DateTimeStr(block.m_uTime).c_str(),
            block.m_vTrans.size());
        //CRITICAL_BLOCK(cs_mapWallet)
        {
            if (mapWallet.count(block.m_vTrans[0].GetHash()))
            {
                CWalletTx& wtx = mapWallet[block.m_vTrans[0].GetHash()];
                printf("    mine:  %d  %d  %d", wtx.GetDepthInMainChain(), wtx.GetBlocksToMaturity(), wtx.GetCredit());
            }
        }
        printf("\n");


        // put the main timechain first
        vector<CBlockIndex*>& vNext = mapNext[pindex];
        for (int i = 0; i < vNext.size(); i++)
        {
            if (vNext[i]->m_pNextBlkIndex)
            {
                printf("error %s_%d\n", __FILE__, __LINE__);
                //swap(vNext[0], vNext[i]);
                break;
            }
        }

        // iterate children
        for (int i = 0; i < vNext.size(); i++)
            vStack.push_back(make_pair(nCol+i, vNext[i]));
#endif

    }
}






int BlockEngine::FormatHashBlocks(void* pbuffer, unsigned int len)
{
    unsigned char* pdata = (unsigned char*)pbuffer;
    unsigned int blocks = 1 + ((len + 8) / 64);
    unsigned char* pend = pdata + 64 * blocks;
    memset(pdata + len, 0, 64 * blocks - len);
    pdata[len] = 0x80;
    unsigned int bits = len * 8;
    pend[-1] = (bits >> 0) & 0xff;
    pend[-2] = (bits >> 8) & 0xff;
    pend[-3] = (bits >> 16) & 0xff;
    pend[-4] = (bits >> 24) & 0xff;
    return blocks;
}

using CryptoPP::ByteReverse;
static int detectlittleendian = 1;

// 计算hash
void BlockEngine::BlockSHA256(const void* pin, unsigned int nBlocks, void* pout)
{
    unsigned int* pinput = (unsigned int*)pin;
    unsigned int* pstate = (unsigned int*)pout;

    CryptoPP::SHA256::InitState(pstate);

    // 检查是大端还是小端
    if (*(char*)&detectlittleendian != 0)
    {
        for (int n = 0; n < nBlocks; n++)
        {
            unsigned int pbuf[16];
            // 大小端的问题将字节翻转
            for (int i = 0; i < 16; i++)
                pbuf[i] = ByteReverse(pinput[n * 16 + i]);
            CryptoPP::SHA256::Transform(pstate, pbuf);
        }
        for (int i = 0; i < 8; i++)
            pstate[i] = ByteReverse(pstate[i]);
    }
    else
    {
        for (int n = 0; n < nBlocks; n++)
            CryptoPP::SHA256::Transform(pstate, pinput + n * 16);
    }
}

// 节点挖矿
bool BlockEngine::BitcoinMiner()
{
    printf("BitcoinMiner started\n");
    ////SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

    CKey key;
    key.MakeNewKey(); // 使用椭圆曲线算法获得一对公钥和私钥
	// 随机数从0开始
    CBigNum bnExtraNonce = 0;
    while (fGenerateBitcoins)
    {
        sleep(50);
#if 0
        CheckForShutdown(3);
        while (vNodes.empty())
        {
            sleep(30);
            CheckForShutdown(3);
        }
#endif
        unsigned int nTransactionsUpdatedLast = WalletServ::getInstance()->nTransactionsUpdated;
        CBlockIndex* pindexPrev = pindexBest;
        if (NULL == pindexPrev) {
            printf("**************************\nBitcoinMiner---pindexPrev == NULL\n*************************\n");


        }
		// 获取挖矿难度
        unsigned int m_uBits = GetNextWorkRequired(pindexPrev);
       // printf("BitcoinMiner---[%0x]--prev Hash [%s]\n", m_uBits, pindexPrev->GetBlockHash().ToString().c_str());

        // 创建币基交易
        // Create coinbase tx
        //
        CTransaction txNew;
        txNew.m_vTxIn.resize(1);
        txNew.m_vTxIn[0].m_cPrevOut.SetNull();
        txNew.m_vTxIn[0].m_cScriptSig << m_uBits << ++bnExtraNonce;
        txNew.m_vTxOut.resize(1);
        txNew.m_vTxOut[0].m_cScriptPubKey << key.GetPubKey() << OP_CHECKSIG;


        // 创建新的区块
        // Create new block
        //
        auto_ptr<CBlock> pblock(new CBlock());
        if (!pblock.get())
            return false;

		// 增加币基交易左右区块的第一个交易
        // Add our coinbase tx as first transaction
        pblock->m_vTrans.push_back(txNew);

		// 收集最新的交易放入区块中
        // Collect the latest transactions into the block
        int64 nFees = 0;
        //CRITICAL_BLOCK(cs_main)
        //CRITICAL_BLOCK(cs_mapTransactions)
        map<uint256, CTransaction>& mapTransactions = WalletServ::getInstance()->mapTransactions;
        {
            map<uint256, CTxIndex> mapTestPool;
            vector<char> vfAlreadyAdded(mapTransactions.size());
            bool fFoundSomething = true;
            unsigned int nBlockSize = 0;
            // 外层循环是因为是多线程，可能刚开始对应的交易没有怎么多，则在等待交易，进行打包，只等待一轮，如果mapTransactions有很多交易则一起打包
            while (fFoundSomething && nBlockSize < MAX_SIZE/2)
            {
                fFoundSomething = false;
                unsigned int n = 0;
                for (map<uint256, CTransaction>::iterator mi = mapTransactions.begin(); mi != mapTransactions.end(); ++mi, ++n)
                {
                    if (vfAlreadyAdded[n])
                        continue;
                    CTransaction& tx = (*mi).second;
                    if (tx.IsCoinBase() || !tx.IsFinal())
                        continue;

                    // Transaction fee requirements, mainly only needed for flood control
                    // Under 10K (about 80 inputs) is free for first 100 transactions
                    // Base rate is 0.01 per KB
                    // 根据费用来判断每一个交易需要的最少费用
                    int64 nMinFee = tx.GetMinFee(pblock->m_vTrans.size() < 100);

                    map<uint256, CTxIndex> mapTestPoolTmp(mapTestPool);
                    // 判断当前交易是否满足对应的最低费用要求，对应的nFees在ConnectInputs是进行累加的
                    if (!tx.ConnectInputs(mapTestPoolTmp, CDiskTxPos(1,1,1), 0, nFees, false, true, nMinFee))
                        continue;
                    swap(mapTestPool, mapTestPoolTmp);

                    pblock->m_vTrans.push_back(tx);
                    printf("error %s--%d\n", __FILE__, __LINE__);
                    //nBlockSize += ::GetSerializeSize(tx, SER_NETWORK); // 将当前加入块的交易大小加入对应的块大小中
                    vfAlreadyAdded[n] = true;
                    fFoundSomething = true;
                }
            }
        }
        pblock->m_uBits = m_uBits; // 设置对应的挖坑难度值
        pblock->m_vTrans[0].m_vTxOut[0].m_nValue = pblock->GetBlockValue(nFees); // 设置对应的块第一个交易对应的输出对应的值=奖励 + 交易费用
        printf("\n\nRunning BitcoinMiner with %d transactions in block, uBit[%0x]\n", pblock->m_vTrans.size(), m_uBits);


        //
        // Prebuild hash buffer
        //
        struct unnamed1
        {
            struct unnamed2
            {
                int nVersion;
                uint256 hashPrevBlock;
                uint256 hashMerkleRoot;
                unsigned int m_uTime;
                unsigned int m_uBits;
                unsigned int m_uNonce;
            }
            block;
            unsigned char pchPadding0[64];
            uint256 hash1;
            unsigned char pchPadding1[64];
        }
        tmp;

        tmp.block.nVersion       = pblock->m_nCurVersion;
        tmp.block.hashPrevBlock  = pblock->m_hashPrevBlock  = (pindexPrev ? pindexPrev->GetBlockHash() : 0);
        tmp.block.hashMerkleRoot = pblock->m_hashMerkleRoot = pblock->BuildMerkleTree();
        // 取前11个区块对应的创建时间对应的中位数
        tmp.block.m_uTime          = pblock->m_uTime          = max((pindexPrev ? pindexPrev->GetMedianTimePast()+1 : 0), GetAdjustedTime());
        tmp.block.m_uBits          = pblock->m_uBits          = m_uBits;
        tmp.block.m_uNonce         = pblock->m_uNonce         = 1; // 随机数从1开始

        unsigned int nBlocks0 = FormatHashBlocks(&tmp.block, sizeof(tmp.block));
        unsigned int nBlocks1 = FormatHashBlocks(&tmp.hash1, sizeof(tmp.hash1));


        //
        // Search
        //
        unsigned int nStart = GetTime();
        uint256 hashTarget = CBigNum().SetCompact(pblock->m_uBits).getuint256(); // 根据难度系数值获取对应的hash目标值
        printf("BitcoinMiner: hashTarget[%s]--nBlocks0[%u]--nBlocks1[%u]\n", hashTarget.ToString().c_str(), nBlocks0, nBlocks1);
        uint256 hash;
        loop
        {
            BlockSHA256(&tmp.block, nBlocks0, &tmp.hash1);
            BlockSHA256(&tmp.hash1, nBlocks1, &hash);


            // 挖矿成功
            if (hash <= hashTarget)
            {
                pblock->m_uNonce = tmp.block.m_uNonce;
                assert(hash == pblock->GetHash());

                //// debug print
                printf("BitcoinMiner:\n");
                printf("proof-of-work found  \n  hash: %s  \ntarget: %s\n", hash.GetHex().c_str(), hashTarget.GetHex().c_str());
                pblock->print();

                // Save key
                if (!WalletServ::getInstance()->AddKey(key))
                    return false;
                key.MakeNewKey();

                // Process this block the same as if we had received it from another node
                if (!ProcessBlock(NULL, pblock.release()))
                    printf("ERROR in BitcoinMiner, ProcessBlock, block not accepted\n");

                sleep(500);
                break;
            }

            // 更新区块创建时间，重新用于挖矿
            // Update m_uTime every few seconds
            if ((++tmp.block.m_uNonce & 0x3ffff) == 0)
            {
                if (tmp.block.m_uNonce == 0)
                    break;
                if (pindexPrev != pindexBest)
                    break;
                if (WalletServ::getInstance()->nTransactionsUpdated != nTransactionsUpdatedLast && GetTime() - nStart > 60)
                    break;
                if (!fGenerateBitcoins)
                    break;
                tmp.block.m_uTime = pblock->m_uTime = max(pindexPrev->GetMedianTimePast()+1, GetAdjustedTime());
            }
        }
    }

    return true;
}





/* EOF */

