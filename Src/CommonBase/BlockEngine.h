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

#ifndef CXX_BT_BLOCKENGINE_H
#define CXX_BT_BLOCKENGINE_H

#include <pthread.h>
#include "key.h"
#include "CommonBase/CommDataDef.h"
#include "Network/CAddress.h"
class COutPoint;
class CInPoint;
class CDiskTxPos;
class CCoinBase;
class CBlock;
class CBlockIndex;
class CWalletTx;
class CKeyItem;
class CNode;
class CScript;
class CAddress;
class CDataStream;
class CTransaction;
class uint256;
class uint160;
class CTxDB;
class CInv;
class BlockEngine
{
public:
    static BlockEngine* getInstance();
    static void Destory();
public:
    void initiation();
    bool AddKey(const CKey& key);
    vector<unsigned char> GenerateNewKey();
    bool AddToWallet(const CWalletTx& wtxIn);
    bool AddToWalletIfMine(const CTransaction& tx, const CBlock* pblock);
    bool EraseFromWallet(uint256 hash);
    void AddOrphanTx(const CDataStream& vMsg);
    void EraseOrphanTx(uint256 hash);
    void ReacceptWalletTransactions();
    void RelayWalletTransactions();
    uint256 GetOrphanRoot(const CBlock* pblock);
    unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast);
    bool Reorganize(CTxDB& txdb, CBlockIndex* pindexNew);
    bool ProcessBlock(CNode* pfrom, CBlock* pblock);
    template<typename Stream>
    bool ScanMessageStart(Stream& s);
    string GetAppDir();
    bool CheckDiskSpace(int64 nAdditionalBytes=0);
    FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode="rb");
    FILE* AppendBlockFile(unsigned int& m_nFileRet);
    void PrintBlockTree();
    bool AlreadyHave(CTxDB& txdb, const CInv& inv);
    bool ProcessMessages(CNode* pfrom);
    bool ProcessMessage(CNode* pfrom, string strCommand, CDataStream& vRecv);
    bool SendMessages(CNode* pto);
    int FormatHashBlocks(void* pbuffer, unsigned int len);
    void BlockSHA256(const void* pin, unsigned int nBlocks, void* pout);
    bool BitcoinMiner();
    int64 GetBalance();
    bool SelectCoins(int64 nTargetValue, set<CWalletTx*>& setCoinsRet);
    bool CreateTransaction(CScript m_cScriptPubKey, int64 nValue, CWalletTx& txNew, int64& nFeeRequiredRet);
    bool CommitTransactionSpent(const CWalletTx& wtxNew);
    bool SendMoney(CScript m_cScriptPubKey, int64 nValue, CWalletTx& wtxNew);

private:
    BlockEngine();
    ~BlockEngine();
    BlockEngine(const BlockEngine&);
    BlockEngine& operator =(const BlockEngine&);
    
    bool LoadBlockIndex(bool fAllowNew=true);
    bool LoadWallet();
    bool LoadAddresses();
public:
    map<uint256, CTransaction> mapTransactions;// 如果交易对应的区块已经放入主链中，则将从内存上删除这些放入区块中的交易，也就是说这里面仅仅保存没有被打包到主链中交易
    unsigned int nTransactionsUpdated;
    map<COutPoint, CInPoint> mapNextTx;// 如果对应的区块已经放入到主链中，则对应的区块交易应该要从本节点保存的交易内存池中删除
    map<uint256, CBlockIndex*> mapBlockIndex;
    const uint256 hashGenesisBlock;
    CBlockIndex* pindexGenesisBlock;
    int nBestHeight;
    uint256 hashBestChain;
    CBlockIndex* pindexBest;
    map<uint256, CBlock*> mapOrphanBlocks; // 孤儿块map
    multimap<uint256, CBlock*> mapOrphanBlocksByPrev;

    map<uint256, CDataStream*> mapOrphanTransactions;// 孤儿交易，其中key对应的交易hash值
    multimap<uint256, CDataStream*> mapOrphanTransactionsByPrev; // 其中key为value交易对应输入的交易的hash值，value为当前交易


    map<uint256, CWalletTx> mapWallet; // 钱包交易对应的map，其中key对应的钱包交易的hash值，mapWallet仅仅存放和本节点相关的交易
    vector<pair<uint256, bool> > vWalletUpdated;
    map<vector<unsigned char>, CPrivKey> mapKeys; // 公钥和私钥对应的映射关系，其中key为公钥，value为私钥
    map<uint160, vector<unsigned char> > mapPubKeys; // 公钥的hash值和公钥的关系，其中key为公钥的hash值，value为公钥
    CKey keyUser; // 当前用户公私钥对信息
    string strSetDataDir;
    int nDropMessagesTest;
    int fGenerateBitcoins;
    int64 nTransactionFee;
    CAddress addrIncoming;
    map<string, string> mapAddressBook; // 地址和名称的映射，其中key为地址，value为名称

private:
    static BlockEngine* m_pInstance;
    static pthread_mutex_t m_mutexLock;
};

#endif/* CXX_BT_BLOCKENGINE_H */
/* EOF */

