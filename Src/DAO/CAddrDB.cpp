/*
 * =====================================================================================
 *
 *       Filename:  CAddrDB.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:50:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "DAO/CAddrDB.h"
#include "NetWorkService/CAddress.h"
#include "NetWorkService/irc.h"
#include "NetWorkService/NetWorkServ.h"
#include "ProtocSrc/Address.pb.h"
#include "CommonBase/ProtocSerialize.h"
//
// CAddrDB
//

using namespace Enze;

bool CAddrDB::WriteAddress(DbTxn*txn, const CAddress& addr)
{
    Address cPbAddr;
    Enze::SeriaAddress(addr, cPbAddr);
    string key = string("addr") + addr.GetKey();
    printf("CAddrDB::WriteAddress key [%s], size[%d]\n", key.c_str(), cPbAddr.ByteSize());
    return Write(txn, key, cPbAddr);
}

bool CAddrDB::LoadAddresses(map<string, CAddress>& mapAddresses)
{
        // Get cursor
        Dbc* pcursor = GetCursor();
        if (!pcursor)
            return false;
        
        loop
        {
            Dbt datKey;
            Dbt datValue;
            // Read next record
            int ret = ReadAtCursor(pcursor, datKey, datValue);
            if (ret == DB_NOTFOUND)
                break;
            else if (ret != 0)
                return false;

            // Unserialize
            string strType = (const char*)datKey.get_data();
            printf("CAddrDB::LoadAddresses--key[%s]\n", strType.c_str());
            if (string::npos != strType.find("addr"))
            {
                Address cPbAddr;
                cPbAddr.ParsePartialFromString((const char*)datValue.get_data());
                CAddress addr(cPbAddr.ip(), cPbAddr.port(), cPbAddr.nservices());
                mapAddresses.insert(make_pair(addr.GetKey(), addr));
            }
            memset(datKey.get_data(), 0, datKey.get_size());
            memset(datValue.get_data(), 0, datValue.get_size());
            free(datKey.get_data());
            free(datValue.get_data());
        }
        pcursor->close();
        //// debug print
        printf("mapAddresses:\n");
        foreach(const PAIRTYPE(string, CAddress)& item, mapAddresses)
            item.second.print();
        printf("-----\n");

        // Fix for possible bug that manifests in mapAddresses.count in irc.cpp,
        // just need to call count here and it doesn't happen there.  The bug was the
        // pack pragma in irc.cpp and has been fixed, but I'm not in a hurry to delete this.
        mapAddresses.count("");

    return true;
}

/* EOF */

