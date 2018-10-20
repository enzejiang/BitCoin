/*
 * =====================================================================================
 *
 *       Filename:  DaoServ.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/06/2018 06:54:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EZ_BT_DAOSERV_H
#define EZ_BT_DAOSERV_H

#include <pthread.h>
#include <db_cxx.h>
#include "CAddrDB.h"
#include "CReviewDB.h"
#include "CTxDB.h"
#include "CWalletDB.h"

namespace Enze
{
    class DaoServ
    {
        public:
            static DaoServ* getInstance();
            static void Destory();
            void DBFlush(bool fShutdown);
            /* CAddrDB public function */
            bool WriteAddress(const CAddress& addr);
            bool LoadAddresses(map<string, CAddress>& mapAddresses);
           
            /* CReviewDB public function */
            bool ReadReviews(const uint256& hash, vector<CReview>& vReviews);
            bool WriteReviews(const uint256& hash, const vector<CReview>& vReviews);
            bool ReadUser(const uint256& hash, CUser& user);
            bool WriteUser(const uint256& hash, const CUser& user);
            /* CTxDB */ 
            bool ReadTxIndex(const uint256& hash, CTxIndex& txindex);
            bool UpdateTxIndex(const uint256& hash, const CTxIndex& txindex);
            bool AddTxIndex(const CTransaction& tx, const CDiskTxPos& pos, int nHeight);
            bool EraseTxIndex(const CTransaction& tx);
            bool ContainsTx(const uint256& hash);
            bool ReadOwnerTxes(const uint160& hash160, int nHeight, vector<CTransaction>& vtx);
            bool ReadDiskTx(const uint256& hash, CTransaction& tx, CTxIndex& txindex);
            bool ReadDiskTx(const uint256& hash, CTransaction& tx);
            bool ReadDiskTx(const COutPoint& outpoint, CTransaction& tx, CTxIndex& txindex);
            bool ReadDiskTx(const COutPoint& outpoint, CTransaction& tx);
            bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
            bool EraseBlockIndex(const uint256& hash);
            bool ReadHashBestChain(uint256& hashBestChain);
            bool WriteHashBestChain(const uint256& hashBestChain);
            bool LoadBlockIndex();
            
            /* CWalletDB */
            bool LoadWallet(vector<unsigned char>& vchDefaultKeyRet);
        
            bool ReadName(const string& strAddress, string& strName);
            
            bool WriteName(const string& strAddress, const string& strName);

            bool EraseName(const string& strAddress);

            bool ReadTx(const uint256& hash, CWalletTx& wtx);
            bool WriteTx(const uint256& hash, const CWalletTx& wtx);
            bool EraseTx(const uint256& hash);
            bool ReadKey(const vector<unsigned char>& vchPubKey, CPrivKey& vchPrivKey);
            bool WriteKey(const vector<unsigned char>& vchPubKey, const CPrivKey& vchPrivKey);
            bool ReadDefaultKey(vector<unsigned char>& vchPubKey);
            bool WriteDefaultKey(const vector<unsigned char>& vchPubKey);
            bool ReadSetting(const string& strKey, string& value);
            bool WriteSetting(const string& strKey, const string& value);
            
            bool SetAddressBookName(const string& strAddress, const string& strName);
        private:
            DaoServ(DbEnv* pDbEnv);
            ~DaoServ();
        private:
            static DaoServ* m_pInstance;
            static pthread_mutex_t m_DaoLock;
            static DbEnv m_cDbenv;//((u_int32_t)0);
            CAddrDB m_cAddrDb;
            CReviewDB m_cReviewDb;
            CTxDB m_cTxDb;
            CWalletDB m_cWalletDb;
             
    };

}

#endif
/* EOF */

