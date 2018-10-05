// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef CXX_BT_MAIN_H
#define CXX_BT_MAIN_H

#include "key.h"
#include "CommonBase/CommDataDef.h"
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
extern map<uint256, CTransaction> mapTransactions;
extern unsigned int nTransactionsUpdated;
extern map<COutPoint, CInPoint> mapNextTx;// 如果对应的区块已经放入到主链中，则对应的区块交易应该要从本节点保存的交易内存池中删除
extern map<uint256, CBlockIndex*> mapBlockIndex;
extern const uint256 hashGenesisBlock;
extern CBlockIndex* pindexGenesisBlock;
extern int nBestHeight;
extern uint256 hashBestChain;
extern CBlockIndex* pindexBest;
extern map<uint256, CBlock*> mapOrphanBlocks; // 孤儿块map
extern multimap<uint256, CBlock*> mapOrphanBlocksByPrev;

extern map<uint256, CDataStream*> mapOrphanTransactions;// 孤儿交易，其中key对应的交易hash值
extern multimap<uint256, CDataStream*> mapOrphanTransactionsByPrev; // 其中key为value交易对应输入的交易的hash值，value为当前交易


extern map<uint256, CWalletTx> mapWallet; // 钱包交易对应的map，其中key对应的钱包交易的hash值，mapWallet仅仅存放和本节点相关的交易
extern vector<pair<uint256, bool> > vWalletUpdated;
extern map<vector<unsigned char>, CPrivKey> mapKeys; // 公钥和私钥对应的映射关系，其中key为公钥，value为私钥
extern map<uint160, vector<unsigned char> > mapPubKeys; // 公钥的hash值和公钥的关系，其中key为公钥的hash值，value为公钥
extern map<vector<unsigned char>, CPrivKey> mapKeys;
extern map<uint160, vector<unsigned char> > mapPubKeys;
extern CKey keyUser;
extern string strSetDataDir;
extern int nDropMessagesTest;


// Settings
extern int fGenerateBitcoins;
extern int64 nTransactionFee;
extern CAddress addrIncoming;




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
bool LoadBlockIndex(bool fAllowNew=true);
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

#endif 
/* EOF */
