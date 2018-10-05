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
#include "Db/CAddrDB.h"
#include "Network/CAddress.h"
#include "Network/irc.h"
#include "Network/net.h"
//
// CAddrDB
//

bool CAddrDB::WriteAddress(const CAddress& addr)
{
    return Write(make_pair(string("addr"), addr.GetKey()), addr);
}

bool CAddrDB::LoadAddresses()
{
    //CRITICAL_BLOCK(cs_mapIRCAddresses)
    //CRITICAL_BLOCK(cs_mapAddresses)
    {
        // Load user provided addresses
        CAutoFile filein = fopen("addr.txt", "a+");
        if (filein)
        {
            try
            {
                char psz[1000];
                while (fgets(psz, sizeof(psz), filein))
                {
                    CAddress addr(psz, NODE_NETWORK);
                    if (addr.ip != 0)
                    {
                        AddAddress(*this, addr);
                        mapIRCAddresses.insert(make_pair(addr.GetKey(), addr));
                    }
                }
            }
            catch (...) { }
        }

        // Get cursor
        Dbc* pcursor = GetCursor();
        if (!pcursor)
            return false;

        loop
        {
            // Read next record
            CDataStream ssKey;
            CDataStream ssValue;
            int ret = ReadAtCursor(pcursor, ssKey, ssValue);
            if (ret == DB_NOTFOUND)
                break;
            else if (ret != 0)
                return false;

            // Unserialize
            string strType;
            ssKey >> strType;
            if (strType == "addr")
            {
                CAddress addr;
                ssValue >> addr;
                mapAddresses.insert(make_pair(addr.GetKey(), addr));
            }
        }

        CAddress addr1("192.168.2.201");
        CAddress addr2("192.168.2.198");
        mapAddresses.insert(make_pair(addr1.GetKey(), addr1));
        mapAddresses.insert(make_pair(addr2.GetKey(), addr2));
        //// debug print
        printf("mapAddresses:\n");
        foreach(const PAIRTYPE(vector<unsigned char>, CAddress)& item, mapAddresses)
            item.second.print();
        printf("-----\n");

        // Fix for possible bug that manifests in mapAddresses.count in irc.cpp,
        // just need to call count here and it doesn't happen there.  The bug was the
        // pack pragma in irc.cpp and has been fixed, but I'm not in a hurry to delete this.
        mapAddresses.count(vector<unsigned char>(18));
    }

    return true;
}

/* EOF */

