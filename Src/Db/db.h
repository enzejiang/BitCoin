// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef CXX_BT_DB_H
#define CXX_BT_DB_H

#include <db_cxx.h>
#include "Db/CWalletDB.h"


class CTxIndex;
class CDiskBlockIndex;
class CDiskTxPos;
class COutPoint;
class CUser;
class CReview;
class CAddress;
class CWalletTx;
class CBlockIndex;
extern map<string, string> mapAddressBook;
extern bool fClient;
extern bool fDbEnvInit;
extern map<string, int> mapFileUseCount;

extern DbEnv dbenv;
extern void DBFlush(bool fShutdown);
extern string GetAppDir();

bool LoadAddresses();
CBlockIndex* InsertBlockIndex(uint256 hash);
class CMarketDB : public CDB
{
public:
    CMarketDB(const char* pszMode="r+", bool fTxn=false) : CDB("market.dat", pszMode, fTxn) { }
private:
    CMarketDB(const CMarketDB&);
    void operator=(const CMarketDB&);
};


bool LoadWallet();

inline bool SetAddressBookName(const string& strAddress, const string& strName)
{
    return CWalletDB().WriteName(strAddress, strName);
}

#endif
/* EOF */
