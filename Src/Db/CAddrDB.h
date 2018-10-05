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
#ifndef CXX_BT_CADDRDB_H
#define CXX_BT_CADDRDB_H
#include "Db/CDB.h"
class CAddress;
class CAddrDB : public CDB
{
public:
    CAddrDB(const char* pszMode="r+", bool fTxn=false) : CDB("addr.dat", pszMode, fTxn) { }
    
    bool WriteAddress(const CAddress& addr);
    bool LoadAddresses();

private:
    CAddrDB(const CAddrDB&);
    void operator=(const CAddrDB&);
};


#endif /* CXX_BT_CADDRDB_H */
/* EOF */

