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
#include "BlockEngine/BlockEngine.h"
#include "CommonBase/ProtocSerialize.h"
#include "DAO/CWalletDB.h"
#include "NetWorkService/CAddress.h"
#include "ProtocSrc/WalletTx.pb.h"
#include "WalletService/WalletServ.h"
//
// CWalletDB
//
using namespace Enze;
CWalletDB::CWalletDB(DbEnv*pDbenv,const char* pszMode, bool fTxn)
    : CDB(pDbenv,"wallet.dat", pszMode, fTxn)
{

}

CWalletDB::~CWalletDB()
{

}
        
bool CWalletDB::WriteName(DbTxn* txn, const string& strAddress, const string& strName)
{
    map<string, string>& mapAddressBook = WalletServ::getInstance()->mapAddressBook;
    mapAddressBook[strAddress] = strName;
    string key = string("name")+strAddress;
    return Write(txn, key, strName);
}

bool CWalletDB::ReadName(const string& strAddress, string& strName)
{
    strName = "";
    string key = string("name")+strAddress;
    return Read(key, strName);
}

bool CWalletDB::EraseName(DbTxn* txn, const string& strAddress)
{
    map<string, string>& mapAddressBook = WalletServ::getInstance()->mapAddressBook;
    mapAddressBook.erase(strAddress);
    string key = string("name")+strAddress;
    return Erase(txn, key);
}

bool CWalletDB::ReadTx(const uint256& hash, CWalletTx& wtx)
{
    printf("%s---%d\n", __FILE__, __LINE__);
    string key = string("tx")+hash.ToString();
    WalletTx cProtoc;
    bool bRet = Read(key, cProtoc);
    if (bRet)
    {
        bRet = UnSeriaWalletTx(cProtoc, wtx);
    }

    return bRet;
    //return Read(make_pair(string("tx"), hash), wtx);
}

bool CWalletDB::WriteTx(DbTxn*txn, const uint256& hash, const CWalletTx& wtx)
{
    printf("%s---%d\n", __FILE__, __LINE__);
    string key = string("tx")+hash.ToString();
    WalletTx cProtoData;
    if (SeriaWalletTx(wtx, cProtoData))
        return Write(txn,key, cProtoData);

    return false;
}

bool CWalletDB::EraseTx(DbTxn*txn, const uint256& hash)
{
    string key = string("tx")+hash.ToString();
    return Erase(txn, key);
}

bool CWalletDB::ReadKey(const vector<unsigned char>& vchPubKey, CPrivKey& vchPrivKey)
{
    vchPrivKey.clear();
    string strPub(vchPubKey.begin(), vchPubKey.end());
    string key = string("key") +strPub;
    string value;
    bool bRet = Read(key, value);
    if (bRet) 
    {
        foreach(auto it, value)
        {
            vchPrivKey.push_back(it);
        }
    }

    return bRet;
}

bool CWalletDB::WriteKey(DbTxn* txn, const vector<unsigned char>& vchPubKey, const CPrivKey& vchPrivKey)
{
    string strPub(vchPubKey.begin(), vchPubKey.end());
    string key = string("key") +strPub;
    printf("%s---%d\n", __FILE__, __LINE__);
    string value(vchPrivKey.begin(), vchPrivKey.end());
    
    return Write(txn, key, value, false);
}
        
bool CWalletDB::ReadDefaultKey(vector<unsigned char>& vchPubKey)
{
    vchPubKey.clear();
    string value;
    bool bRet = Read(string("defaultkey"), value);
    if (bRet) 
    {
        foreach(auto it, value)
        {
            vchPubKey.push_back(it);
        }
    }
    
    return bRet;
}

bool CWalletDB::WriteDefaultKey(DbTxn* txn, const vector<unsigned char>& vchPubKey)
{
    string value(vchPubKey.begin(), vchPubKey.end());
    return Write(txn, string("defaultkey"), value);
}

bool CWalletDB::ReadSetting(const string& strKey, string& value)
{
    string key = string("setting")+strKey;
    return Read(key, value);
}

bool CWalletDB::WriteSetting(DbTxn* txn, const string& strKey, const string& value)
{
    return Write(txn, string("setting") + strKey, value);
}

bool CWalletDB::LoadWallet(vector<unsigned char>& vchDefaultKeyRet)
{
    vchDefaultKeyRet.clear();

    //// todo: shouldn't we catch exceptions and try to recover and continue?
    
    map<uint256, CWalletTx>& mapWallet = WalletServ::getInstance()->mapWallet; // 钱包交易对应的map，其中key对应的钱包交易的hash值，mapWallet仅仅存放和本节点相关的交易
    map<vector<unsigned char>, CPrivKey>& mapKeys = WalletServ::getInstance()->mapKeys; // 公钥和私钥对应的映射关系，其中key为公钥，value为私钥
    map<uint160, vector<unsigned char> >& mapPubKeys = WalletServ::getInstance()->mapPubKeys; // 公钥的hash值和公钥的关系，其中key为公钥的hash值，value为公钥
    int& fGenerateBitcoins = BlockEngine::getInstance()->fGenerateBitcoins;
    int64& nTransactionFee = WalletServ::getInstance()->nTransactionFee;
    CAddress& addrIncoming = WalletServ::getInstance()->addrIncoming;
    map<string, string>& mapAddressBook = WalletServ::getInstance()->mapAddressBook;
    {
        // Get cursor
        Dbc* pcursor = GetCursor();
        if (!pcursor)
            return false;

        loop
        {
            // Read next record
            Dbt ssKey;
            Dbt ssValue;
            int ret = ReadAtCursor(pcursor, ssKey, ssValue);
            if (ret == DB_NOTFOUND)
                break;
            else if (ret != 0)
                return false;

            // Unserialize
            // Taking advantage of the fact that pair serialization
            // is just the two items serialized one after the other
            string strType((const char*)ssKey.get_data());
            printf("error:%s---%d--[%s]\n", __FILE__, __LINE__, strType.c_str());
            if (strType.length()!= ssKey.get_size())
            {
                printf("error:%s---%d--[%d]--[%d]--%s\n", __FILE__, __LINE__, strType.length(), ssKey.get_size(), strType.c_str());
               //  break;
            }
            if (0 == strType.find_first_of("name"))
            {
                int pos = strType.find_first_not_of("name");
                string strAddress = strType.substr(pos);
                string name((const char*)ssValue.get_data(),ssValue.get_size());
                mapAddressBook[strAddress] = name;
                printf("Addr[%s]---name[%s]--Size[%d]\n", strAddress.c_str(), name.c_str(), ssValue.get_size());
            }
            else if (0 == strType.find_first_of("tx"))
            {
                printf("error:%s---%d\n", __FILE__, __LINE__);
                WalletTx cProtoc;
                bool bRet = cProtoc.ParsePartialFromArray(ssValue.get_data(), ssValue.get_size());
                if (bRet)
                {
                    CWalletTx wtx;
                    if(UnSeriaWalletTx(cProtoc, wtx))
                    {
                        int pos = strType.find_first_not_of("tx");
                        if (pos != string::npos)
                        {
                            string strHash = strType.substr(pos);
                            uint256 hash;
                            hash.SetHex(strHash);
                            mapWallet[hash] = wtx;

                            if (wtx.GetHash() != hash)
                                printf("Error in wallet.dat, hash mismatch\n");

                            printf("LoadWallet  %s\n", wtx.GetHash().ToString().c_str());
                            printf(" LoadWallet--Value[%0x]  \n",
                                wtx.m_vTxOut[0].m_nValue);
                        }
                        else
                             break;
                    }
                    else
                         break;
                }
                else
                    break;
            }
            else if (0 == strType.find_first_of("key"))
            {
                vector<unsigned char> vchPubKey(strType.begin()+3, strType.end());
               // ssKey >> vchPubKey;
                string valueData((const char*)ssValue.get_data(), ssValue.get_size());
                if (valueData.length() != ssValue.get_size()) {
                    printf("error:%s---%d\n", __FILE__, __LINE__);
                    continue;
                }
                CPrivKey vchPrivKey(valueData.begin(), valueData.end());


                mapKeys[vchPubKey] = vchPrivKey;
                mapPubKeys[Hash160(vchPubKey)] = vchPubKey;
            }
            else if (0 == strType.find_first_of("defaultkey"))
            {
                printf("error:%s---%d--[%s]\n", __FILE__, __LINE__, strType.c_str());
                string valueData((const char*)ssValue.get_data(), ssValue.get_size());
                if (valueData.length() != ssValue.get_size()) {
                    printf("error:%s---%d\n", __FILE__, __LINE__);
                    continue;
                }
                foreach(auto it, valueData)
                {
                    vchDefaultKeyRet.push_back(it);
                }

            }
            else if (0 == strType.find_first_of("setting"))  /// or settings or option or options or config?
            {

                if (string::npos != strType.find_first_of("fGenerateBitcoins"))
                {
                    stringstream sout;
                    sout<<ssValue.get_data();
                    sout>>fGenerateBitcoins;

                }
                else if (string::npos != strType.find_first_of("nTransactionFee"))
                {
                    stringstream sout;
                    sout<<ssValue.get_data();
                    sout<<nTransactionFee;
#if 0
                    string valueData(ssValue.get_data());
                    if (valueData.length() != ssValue.get_size()) {
                        printf("error:%s---%d\n", __FILE__, __LINE__);
                        continue;
                    }
                     ssValue >> nTransactionFee;
#endif
                }
                else if (string::npos != strType.find_first_of("addrIncoming"))
                {
                    printf("error:%s---%d\n", __FILE__, __LINE__);
                    //continue;
                    //ssValue >> addrIncoming;
                }
            }
        }
        pcursor->close();
    }

    printf("fGenerateBitcoins = %d\n", fGenerateBitcoins);
    printf("nTransactionFee = %I64d\n", nTransactionFee);
    printf("addrIncoming = %s\n", addrIncoming.ToString().c_str());
    mapWallet.count(18);
    return true;
}

/* EOF */

