/*
 * =====================================================================================
 *
 *       Filename:  CAddrDB.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:49:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CADDRDB_H
#define EZ_BT_CADDRDB_H
#include "DAO/CDB.h"
namespace Enze 
{
    class CAddress;
    
    class CAddrDB : public CDB
    {
    public:
        CAddrDB(DbEnv*pDbenv = NULL, const char* pszMode="crw+", bool fTxn=false) : CDB(pDbenv, "addr.dat", pszMode, fTxn) { }

        bool WriteAddress(DbTxn*txn, const CAddress& addr);
        bool LoadAddresses(map<string, CAddress>& mapAddresses);

    private:
        CAddrDB(const CAddrDB&);
        CAddrDB operator=(const CAddrDB&);
    };

}


#endif /* EZ_BT_CADDRDB_H */
/* EOF */

