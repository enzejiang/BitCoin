/*
 * =====================================================================================
 *
 *       Filename:  WalletServ.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/14/2018 03:45:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_WALLET_SERV_H
#define EZ_BT_WALLET_SERV_H
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
    class CNode;
    class CScript;
    class CAddress;
    class CTransaction;
    class uint256;
    class uint160;
    class CInv;
    class Transaction;
    class WalletServ
    {
        public:
            static WalletServ* getInstance();
            static void Destory();
            
            void initiation();
            bool AddKey(const CKey& key);
            vector<unsigned char> GenerateNewKey();
            bool AddToWallet(const CWalletTx& wtxIn);
            bool AddToWalletIfMine(const CTransaction& tx, const CBlock* pblock);
            bool EraseFromWallet(uint256 hash);
            void AddOrphanTx(const Transaction& pbTx);
            void EraseOrphanTx(uint256 hash);
            void ReacceptWalletTransactions();
            void RelayWalletTransactions();
            bool AlreadyHave(const CInv& inv);
            int64 GetBalance();
            bool SelectCoins(int64 nTargetValue, set<CWalletTx*>& setCoinsRet);
            bool CreateTransaction(CScript m_cScriptPubKey, int64 nValue, CWalletTx& txNew, int64& nFeeRequiredRet);
            bool CommitTransactionSpent(const CWalletTx& wtxNew);
            bool SendMoney(CScript m_cScriptPubKey, int64 nValue, CWalletTx& wtxNew);    
            
        public:
            map<uint256, CTransaction> mapTransactions;// 如果交易对应的区块已经放入主链中，则将从内存上删除这些放入区块中的交易，也就是说这里面仅仅保存没有被打包到主链中交易
            unsigned int nTransactionsUpdated;
            map<COutPoint, CInPoint> mapNextTx;// 如果对应的区块已经放入到主链中，则对应的区块交易应该要从本节点保存的交易内存池中删除
            map<uint256, CWalletTx> mapWallet; // 钱包交易对应的map，其中key对应的钱包交易的hash值，mapWallet仅仅存放和本节点相关的交易
            vector<pair<uint256, bool> > vWalletUpdated;
            map<vector<unsigned char>, CPrivKey> mapKeys; // 公钥和私钥对应的映射关系，其中key为公钥，value为私钥
            map<uint160, vector<unsigned char> > mapPubKeys; // 公钥的hash值和公钥的关系，其中key为公钥的hash值，value为公钥
            CKey keyUser; // 当前用户公私钥对信息
            int64 nTransactionFee;
            CAddress addrIncoming;
            map<string, string> mapAddressBook; // 地址和名称的映射，其中key为地址，value为名称
            map<uint256, CTransaction*> mapOrphanTransactions;// 孤儿交易，其中key对应的交易hash值
            multimap<uint256, CTransaction*> mapOrphanTransactionsByPrev; // 其中key为value交易对应输入的交易的hash值，value为当前交易
            
        private:
            WalletServ();
            ~WalletServ();
            bool LoadWallet();
        private:
            static WalletServ* m_pInstance;
            static pthread_mutex_t m_mutexLock;
    
    };

}// end namespace

#endif /* EZ_BT_WALLET_SERV_H */
/* EOF */

