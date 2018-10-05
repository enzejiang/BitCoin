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
#include "BlockEngine.h"
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
    
    map<uint256, CWalletTx>& mapWallet = BlockEngine::getInstance()->mapWallet; // 钱包交易对应的map，其中key对应的钱包交易的hash值，mapWallet仅仅存放和本节点相关的交易
    map<vector<unsigned char>, CPrivKey>& mapKeys = BlockEngine::getInstance()->mapKeys; // 公钥和私钥对应的映射关系，其中key为公钥，value为私钥
    map<uint160, vector<unsigned char> >& mapPubKeys = BlockEngine::getInstance()->mapPubKeys; // 公钥的hash值和公钥的关系，其中key为公钥的hash值，value为公钥
    int& fGenerateBitcoins = BlockEngine::getInstance()->fGenerateBitcoins;
    int64& nTransactionFee = BlockEngine::getInstance()->nTransactionFee;
    CAddress& addrIncoming = BlockEngine::getInstance()->addrIncoming;
    map<string, string>& mapAddressBook = BlockEngine::getInstance()->mapAddressBook;
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
   mapWallet.count(18);
    return true;
}

/* EOF */

