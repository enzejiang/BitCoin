// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef EZ_BT_DB_H
#define EZ_BT_DB_H

#include <db_cxx.h>
#include "DAO/CWalletDB.h"

namespace Enze
{

class CTxIndex;
class CDiskBlockIndex;
class CDiskTxPos;
class COutPoint;
class CUser;
class CReview;
class CAddress;
class CWalletTx;
class CBlockIndex;


CBlockIndex* InsertBlockIndex(uint256 hash);
class CMarketDB : public Enze::CDB
{
public:
    CMarketDB(DbEnv*pDbenv, const char* pszMode="r+", bool fTxn=false) : Enze::CDB(pDbenv, "market.dat", pszMode, fTxn) { }
private:
    CMarketDB(const CMarketDB&);
    void operator=(const CMarketDB&);
};



} //end namespace 

#endif /* EZ_BT_DB_H */
/* EOF */
