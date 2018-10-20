/*
 * =====================================================================================
 *
 *       Filename:  CDB.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:39:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "util.h"
#include "BlockEngine.h"
#include "DAO/CDB.h"
#include "DAO/db.h"
#include "WalletService/CWalletTx.h"

using namespace Enze;

bool CDB::m_bDbEnvInit = false;
map<string, int> CDB::mapFileUseCount;
CDB::CDB(DbEnv*pDbenv, const char* pszFile, const char* pszMode, bool fTxn) 
: m_pDb(NULL)
{
    int ret;
    if (pszFile == NULL)
        return;

    bool fCreate = strchr(pszMode, 'c');
    bool fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));
    unsigned int nFlags = DB_THREAD|DB_AUTO_COMMIT;
    if (fCreate)
        nFlags |= DB_CREATE;
    else if (fReadOnly)
        nFlags |= DB_RDONLY;
  //  if (!fReadOnly || fTxn)
  //      nFlags |= DB_AUTO_COMMIT;


//    m_cStrFile = pszFile;
//    ++mapFileUseCount[m_cStrFile];

    m_pDb = new Db(pDbenv, 0);

    m_pDb->set_error_stream(&std::cerr);

    ret = m_pDb->open(NULL,      // Txn pointer
                    pszFile,   // Filename
                    "main",    // Logical db name
                    DB_BTREE,  // Database type
                    nFlags,    // Flags
                    0);

    if (ret > 0)
    {
        delete m_pDb;
        m_pDb = NULL;
//        --mapFileUseCount[m_cStrFile];
//        m_cStrFile = "";
        throw runtime_error(strprintf("CDB() : can't open database file %s, error %d\n", pszFile, ret));
    }
 //   printf("%s\n", pszFile);
   // if (fCreate && !Exists(string("version")))
   //     WriteVersion(NULL, VERSION);

    RandAddSeed();
}

CDB::~CDB() 
{ 
    Close();
}

void CDB::Close()
{
    if (!m_pDb)
        return;

    m_pDb->close(0);
    delete m_pDb;
    m_pDb = NULL;

    RandAddSeed();
}

bool CDB::Erase(DbTxn*txn, const string& key)
{
    if (NULL == m_pDb)
        return false;
    
    Dbt datKey((void*) key.c_str(), key.length());

    // Erase
    int ret = m_pDb->del(txn, &datKey, 0);

    // Clear memory
    memset(datKey.get_data(), 0, datKey.get_size());
    return (ret == 0 || ret == DB_NOTFOUND);
     
}

bool CDB::Exists(const string& key)
{
    if (!m_pDb)
        return false;

    // Key
    Dbt datKey((void*)key.c_str(), key.length());

    // Exists
    int ret = m_pDb->exists(NULL, &datKey, 0);

    // Clear memory
    memset(datKey.get_data(), 0, datKey.get_size());
    return (ret == 0);

}


bool CDB::ReadVersion(int& nVersion)
{
    nVersion = 0;
    string value;
    bool bRet = Read(string("version"), value);
    if (bRet) {
        nVersion = atoi(value.c_str());
    }
    
    return bRet;
}

bool CDB::WriteVersion(DbTxn* txn, int nVersion)
{
    string key("version");
    ostringstream strValue;
    strValue <<nVersion;
    return Write(txn, key, strValue.str());
}

bool CDB::Read(const string& key, ::google::protobuf::Message& cProtoData)
{
    if (NULL == m_pDb)
        return false;
    Dbt datKey((void*) key.c_str(), key.length());
    
    // Read
    Dbt datValue;
    datValue.set_flags(DB_DBT_MALLOC);
    int ret = m_pDb->get(NULL, &datKey, &datValue, 0);
    memset(datKey.get_data(), 0, datKey.get_size());
    if (0!= ret || datValue.get_data() == NULL)
        return false;
    datValue.set_flags(DB_DBT_USERMEM);
    // Unserialize value
    bool bRet = cProtoData.ParsePartialFromArray((void*)datValue.get_data(), datValue.get_size());
    // Clear and free memory
    memset(datValue.get_data(), 0, datValue.get_size());
    free(datValue.get_data());
    return bRet;
     
}

bool CDB::Read(const string& key, string& value)
{
    if (NULL == m_pDb)
        return false;
    Dbt datKey((void*) key.c_str(), key.length());
    
    // Read
    Dbt datValue;
    datValue.set_flags(DB_DBT_MALLOC);
    int ret = m_pDb->get(NULL, &datKey, &datValue, 0);
    memset(datKey.get_data(), 0, datKey.get_size());
    if (0!= ret || datValue.get_data() == NULL)
        return false;
   
    value =  (char*)datValue.get_data();
    if (value.length() != datValue.get_size()) 
    {
        return false;
    }
    // Clear and free memory
    memset(datValue.get_data(), 0, datValue.get_size());
    free(datValue.get_data());
    return true;
}

bool CDB::Write(DbTxn*txn, const string& key, const ::google::protobuf::Message& value, bool fOverwrite)
{

    if (NULL == m_pDb)
        return false;
    string strSerial;

    if(!value.SerializePartialToString(&strSerial))
    {
        return false;
    }

    Dbt datKey((void*) key.c_str(), key.length());
    Dbt datValue((void*) strSerial.c_str(), strSerial.length());

    // Write
    int ret = m_pDb->put(txn, &datKey, &datValue, (fOverwrite ? 0 : DB_NOOVERWRITE));

    // Clear memory in case it was a private key
    memset(datKey.get_data(), 0, datKey.get_size());
    memset(datValue.get_data(), 0, datValue.get_size());
    return (ret == 0);

}

bool CDB::Write(DbTxn* txn, const string&key, const string& value, bool fOverwrite)
{
    printf("CDB::write--[%s]--value[%s]\n", key.c_str(), value.c_str());
    if (NULL == m_pDb)
        return false;

    Dbt datKey((void*) key.c_str(), key.length());
    Dbt datValue((void*) value.c_str(), value.length());
    // Write
    int ret = m_pDb->put(txn, &datKey, &datValue, (fOverwrite ? 0 : DB_NOOVERWRITE));
    printf("CDB::write--[%s]--value[%s]--end\n", key.c_str(), value.c_str());
    // Clear memory in case it was a private key
    memset(datKey.get_data(), 0, datKey.get_size());
    memset(datValue.get_data(), 0, datValue.get_size());
    return (ret == 0);

}

Dbc* CDB::GetCursor()
{
    if (!m_pDb)
        return NULL;
    Dbc* pcursor = NULL;
    int ret = m_pDb->cursor(NULL, &pcursor, 0);
    if (ret != 0)
        return NULL;
    return pcursor;
}
            
int CDB::ReadAtCursor(Dbc* pcursor, Dbt& datKey, Dbt& datValue, unsigned int fFlags)
{
    datKey.set_flags(DB_DBT_MALLOC);
    datValue.set_flags(DB_DBT_MALLOC);
    int ret = pcursor->get(&datKey, &datValue, fFlags);
    if (ret != 0)
    {
        printf("%s---%d\n", __FILE__, __LINE__);
        return ret;
    }
    else if (datKey.get_data() == NULL || datValue.get_data() == NULL)
    {
        printf("%s---%d\n", __FILE__, __LINE__);
         return 99999;
    }


    return 0;
}

/* EOF */

