/*
 * =====================================================================================
 *
 *       Filename:  CWalletDB.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:52:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EZ_BT_CWALLETDB_H
#define EZ_BT_CWALLETDB_H

#include "key.h"
#include "DAO/CDB.h"
#include "CommonBase/uint256.h"
#include "WalletService/CWalletTx.h"

namespace Enze {

    class CWalletTx;
    
    class CWalletDB : public CDB
    {
    public:
        CWalletDB(DbEnv*pDbenv = NULL,const char* pszMode="crw+", bool fTxn=false);
        virtual ~CWalletDB(); 
        bool LoadWallet(vector<unsigned char>& vchDefaultKeyRet);
        
        bool WriteName(DbTxn *txn, const string& strAddress, const string& strName);
        bool ReadName(const string& strAddress, string& strName);
        bool EraseName(DbTxn* txn, const string& strAddress);

        bool ReadTx(const uint256& hash, CWalletTx& wtx);

        bool WriteTx(DbTxn* txn, const uint256& hash, const CWalletTx& wtx);

        bool EraseTx(DbTxn*txn, const uint256& hash);

        bool ReadKey(const vector<unsigned char>& vchPubKey, CPrivKey& vchPrivKey);

        bool WriteKey(DbTxn* txn, const vector<unsigned char>& vchPubKey, const CPrivKey& vchPrivKey);

        bool ReadDefaultKey(vector<unsigned char>& vchPubKey);

        bool WriteDefaultKey(DbTxn* txn, const vector<unsigned char>& vchPubKey);

        bool ReadSetting(const string& strKey, string& value);

        bool WriteSetting(DbTxn* txn, const string& strKey, const string& value);

    private:
        CWalletDB(const CWalletDB&);
        CWalletDB operator=(const CWalletDB&);
    };



}

#endif /* EZ_BT_CWALLETDB_H */
/* EOF */

