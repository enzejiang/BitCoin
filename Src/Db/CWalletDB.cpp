/*
 * =====================================================================================
 *
 *       Filename:  CWalletDB.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:56:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "main.h"
#include "Db/CWalletDB.h"
#include "TX/CWalletTx.h"
#include "Network/CAddress.h"
//
// CWalletDB
//

bool CWalletDB::LoadWallet(vector<unsigned char>& vchDefaultKeyRet)
{
    vchDefaultKeyRet.clear();

    //// todo: shouldn't we catch exceptions and try to recover and continue?
    //CRITICAL_BLOCK(cs_mapKeys)
    //CRITICAL_BLOCK(cs_mapWallet)
    {
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
            // Taking advantage of the fact that pair serialization
            // is just the two items serialized one after the other
            string strType;
            ssKey >> strType;
            if (strType == "name")
            {
                string strAddress;
                ssKey >> strAddress;
                ssValue >> mapAddressBook[strAddress];
            }
            else if (strType == "tx")
            {
                uint256 hash;
                ssKey >> hash;
                CWalletTx& wtx = mapWallet[hash];
                ssValue >> wtx;

                if (wtx.GetHash() != hash)
                    printf("Error in wallet.dat, hash mismatch\n");

                //// debug print
                printf("LoadWallet  %s\n", wtx.GetHash().ToString().c_str());
                printf(" LoadWallet--Value[%0x]  \n",
                    wtx.m_vTxOut[0].m_nValue);
                   // wtx.m_mapValue["message"].c_str());
            }
            else if (strType == "key")
            {
                vector<unsigned char> vchPubKey;
                ssKey >> vchPubKey;
                CPrivKey vchPrivKey;
                ssValue >> vchPrivKey;

                mapKeys[vchPubKey] = vchPrivKey;
                mapPubKeys[Hash160(vchPubKey)] = vchPubKey;
            }
            else if (strType == "defaultkey")
            {
                ssValue >> vchDefaultKeyRet;
            }
            else if (strType == "setting")  /// or settings or option or options or config?
            {
                string strKey;
                ssKey >> strKey;
                if (strKey == "fGenerateBitcoins")  ssValue >> fGenerateBitcoins;
                if (strKey == "nTransactionFee")    ssValue >> nTransactionFee;
                if (strKey == "addrIncoming")       ssValue >> addrIncoming;
            }
        }
    }

    printf("fGenerateBitcoins = %d\n", fGenerateBitcoins);
    printf("nTransactionFee = %I64d\n", nTransactionFee);
    printf("addrIncoming = %s\n", addrIncoming.ToString().c_str());

    return true;
}

/* EOF */

