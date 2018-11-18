/*
 * =====================================================================================
 *
 *       Filename:  BlockEngine.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/05/2018 11:28:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EZ_BT_BLOCKENGINE_H
#define EZ_BT_BLOCKENGINE_H

#include <pthread.h>
#include "key.h"
#include "CommonBase/CommDataDef.h"
#include "NetWorkService/CAddress.h"
#include "WalletService/CWalletTx.h"

namespace Enze
{
class CDataStream;
class COutPoint;
class CInPoint;
class CDiskTxPos;
class CCoinBase;
class CBlock;
class CBlockIndex;
class CKeyItem;
class PeerNode;
class CScript;
class CAddress;
class CTransaction;
class uint256;
class uint160;
class CInv;
class BlockEngine
{
public:
    static BlockEngine* getInstance();
    static void Destory();
public:
    void initiation();
    uint256 GetOrphanRoot(const CBlock* pblock);
    unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast);
    bool Reorganize(CBlockIndex* pindexNew);
    bool ProcessBlock(PeerNode* pfrom, CBlock* pblock);
    string GetAppDir();
    bool CheckDiskSpace(int64 nAdditionalBytes=0);
    FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode="rb");
    FILE* AppendBlockFile(unsigned int& m_nFileRet);
    void PrintBlockTree();
    int FormatHashBlocks(void* pbuffer, unsigned int len);
    void BlockSHA256(const void* pin, unsigned int nBlocks, void* pout);
    bool BitcoinMiner();
    
    map<uint256, CBlockIndex*>&getMapBlockIndex()
    {
        return mapBlockIndex;
    }
    
    map<uint256, CBlockIndex*>::iterator insertBlockIndex(const pair<uint256, CBlockIndex*> it)
    {
        return mapBlockIndex.insert(it).first;
    }
    
    void eraseBlockIndex(const uint256& hash)
    {
         mapBlockIndex.erase(hash);
    }
    
    const CBlockIndex* getGenesisBlock()const
    {
        return pindexGenesisBlock;
    }

    const uint256& getGenesisHash()
    {
        return hashGenesisBlock;
    }
    
    int getBestHeight()const
    {
        return nBestHeight;
    }
    
    void setBestHeight(int bestHigh)
    {
        nBestHeight = bestHigh;
    }
    
    const uint256& getHashBestChain()const
    {
        return hashBestChain;
    }
    
    void setBestChain(const uint256& bestChain)
    {
        hashBestChain = bestChain;
    }
    
    CBlockIndex* getBestIndex()const
    {
        return pindexBest;
    }

    void setBestIndex(CBlockIndex* pIndexNew)
    {
        pindexBest = pIndexNew;
    }
private:
    BlockEngine();
    ~BlockEngine();
    BlockEngine(const BlockEngine&);
    BlockEngine& operator =(const BlockEngine&);
    
    bool LoadBlockIndex(bool fAllowNew=true);
public:
    map<uint256, CBlock*> mapOrphanBlocks; // ¹Â¶ù¿émap
    multimap<uint256, CBlock*> mapOrphanBlocksByPrev;
     int fGenerateBitcoins;
private:
    map<uint256, CBlockIndex*> mapBlockIndex;
    const uint256 hashGenesisBlock;
    CBlockIndex* pindexGenesisBlock;
    int nBestHeight;
    uint256 hashBestChain;
    CBlockIndex* pindexBest;
    string strSetDataDir;


private:
    static BlockEngine* m_pInstance;
    static pthread_mutex_t m_mutexLock;
};

}
#endif/* EZ_BT_BLOCKENGINE_H */
/* EOF */

