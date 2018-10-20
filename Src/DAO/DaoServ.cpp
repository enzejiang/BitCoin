/*
 * =====================================================================================
 *
 *       Filename:  DaoServ.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/06/2018 08:15:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "DaoServ.h"
using namespace Enze;

DaoServ* DaoServ::m_pInstance = NULL;
pthread_mutex_t DaoServ::m_DaoLock;
DbEnv DaoServ::m_cDbenv(0);
DaoServ* DaoServ::getInstance()
{
    if (NULL == m_pInstance)
    {
        if (NULL == m_pInstance) 
        {
            pthread_mutex_lock(&m_DaoLock);
            string strAppDir = ".";//BlockEngine::getInstance()->GetAppDir();
            string strLogDir = strAppDir + "\/database";
            mkdir(strLogDir.c_str(), 0775);
            printf("dbenv.open strAppDir=%s\n", strAppDir.c_str());

            m_cDbenv.set_lg_dir(strLogDir.c_str());
            m_cDbenv.set_lg_max(10000000);
            m_cDbenv.set_lk_max_locks(10000);
            m_cDbenv.set_lk_max_objects(10000);
            m_cDbenv.set_errfile(fopen("db.log", "a")); /// debug
            m_cDbenv.log_set_config(DB_LOG_AUTO_REMOVE, 1); /// causes corruption
            m_cDbenv.set_timeout(1000000, DB_SET_TXN_TIMEOUT);
            int ret = m_cDbenv.open(strAppDir.c_str(),
                             DB_CREATE     |
                             DB_INIT_LOCK  |
                             DB_INIT_LOG   |
                             DB_INIT_MPOOL |
                             DB_INIT_TXN   |
                             DB_THREAD     |
                             DB_PRIVATE    |
                             DB_RECOVER,
                             0);
            m_pInstance = new DaoServ(&m_cDbenv);
            pthread_mutex_unlock(&m_DaoLock); 
            if (ret > 0)
                throw runtime_error(strprintf("DaoServ() : error %d opening database environment\n", ret));
        
        }
    }
    
    return m_pInstance;
}

void DaoServ::Destory()
{
    pthread_mutex_lock(&m_DaoLock);
    if (NULL == m_pInstance) 
    {
        delete m_pInstance;
        m_pInstance = NULL; 
    }
    pthread_mutex_unlock(&m_DaoLock); 

}

bool DaoServ::WriteAddress(const CAddress& addr)
{
    DbTxn* pcTxn = NULL; 
    int bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cAddrDb.WriteAddress(pcTxn, addr); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;
}

bool DaoServ::LoadAddresses(map<string, CAddress>& mapAddresses)
{
    return m_cAddrDb.LoadAddresses(mapAddresses);
}

bool DaoServ::ReadReviews(const uint256& hash, vector<CReview>& vReviews)
{
    return false;
}

bool DaoServ::WriteReviews(const uint256& hash, const vector<CReview>& vReviews)
{
    return false;

}

bool DaoServ::ReadUser(const uint256& hash, CUser& user)
{
    return false;

}

bool DaoServ::WriteUser(const uint256& hash, const CUser& user)
{
    return false;

}

bool DaoServ::ReadTxIndex(const uint256& hash, CTxIndex& txindex)
{
    return m_cTxDb.ReadTxIndex(hash, txindex);
}

bool DaoServ::UpdateTxIndex(const uint256& hash, const CTxIndex& txindex)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cTxDb.UpdateTxIndex(pcTxn, hash, txindex); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::AddTxIndex(const CTransaction& tx, const CDiskTxPos& pos, int nHeight)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cTxDb.AddTxIndex(pcTxn, tx, pos, nHeight); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::EraseTxIndex(const CTransaction& tx)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cTxDb.EraseTxIndex(pcTxn, tx); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}
bool DaoServ::ContainsTx(const uint256& hash)
{
    return m_cTxDb.ContainsTx(hash);
}
bool DaoServ::ReadOwnerTxes(const uint160& hash160, int nHeight, vector<CTransaction>& vtx)
{
    return m_cTxDb.ReadOwnerTxes(hash160, nHeight, vtx);
}

bool DaoServ::ReadDiskTx(const uint256& hash, CTransaction& tx, CTxIndex& txindex)
{
    return m_cTxDb.ReadDiskTx(hash, tx, txindex);
}

bool DaoServ::ReadDiskTx(const uint256& hash, CTransaction& tx)
{
    return m_cTxDb.ReadDiskTx(hash, tx);
}

bool DaoServ::ReadDiskTx(const COutPoint& outpoint, CTransaction& tx, CTxIndex& txindex)
{
    return m_cTxDb.ReadDiskTx(outpoint, tx, txindex);
}

bool DaoServ::ReadDiskTx(const COutPoint& outpoint, CTransaction& tx)
{
    return m_cTxDb.ReadDiskTx(outpoint, tx);
}

bool DaoServ::WriteBlockIndex(const CDiskBlockIndex& blockindex)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cTxDb.WriteBlockIndex(pcTxn, blockindex); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::EraseBlockIndex(const uint256& hash)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cTxDb.EraseBlockIndex(pcTxn, hash); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::ReadHashBestChain(uint256& hashBestChain)
{
    m_cTxDb.ReadHashBestChain(hashBestChain); 
}

bool DaoServ::WriteHashBestChain(const uint256& hashBestChain)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cTxDb.WriteHashBestChain(pcTxn, hashBestChain); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }

    printf("DaoServ::WriteHashBestChain--[%s]-end\n", hashBestChain.ToString().c_str());
    return bRet;

}

bool DaoServ::LoadBlockIndex()
{
    return m_cTxDb.LoadBlockIndex();
}

bool DaoServ::LoadWallet(vector<unsigned char>& vchDefaultKeyRet)
{
    return m_cWalletDb.LoadWallet(vchDefaultKeyRet);
}

bool DaoServ::ReadName(const string& strAddress, string& strName)
{
    return m_cWalletDb.ReadName(strAddress, strName);
}

bool DaoServ::WriteName(const string& strAddress, const string& strName)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.WriteName(pcTxn, strAddress, strName); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::EraseName(const string& strAddress)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.EraseName(pcTxn, strAddress); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::ReadTx(const uint256& hash, CWalletTx& wtx)
{
    return m_cWalletDb.ReadTx(hash, wtx);
}

bool DaoServ::WriteTx(const uint256& hash, const CWalletTx& wtx)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.WriteTx(pcTxn, hash, wtx); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::EraseTx(const uint256& hash)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.EraseTx(pcTxn, hash); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::ReadKey(const vector<unsigned char>& vchPubKey, CPrivKey& vchPrivKey)
{
    return m_cWalletDb.ReadKey(vchPubKey,vchPrivKey);
}

bool DaoServ::WriteKey(const vector<unsigned char>& vchPubKey, const CPrivKey& vchPrivKey)
{
    printf("%s---%d--%s\n", __FILE__, __LINE__, __func__);

    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    printf("%s---%d--%s---\n", __FILE__, __LINE__, __func__);
    if (NULL == pcTxn || 0 != bRet)
    {
        printf("%s---%d--%s\n", __FILE__, __LINE__, __func__);
        return false;
    }
    
    try {
         bRet = m_cWalletDb.WriteKey(pcTxn, vchPubKey, vchPrivKey); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
        bRet = false;
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    printf("%s---%d--%s-Ret[%d]\n", __FILE__, __LINE__, __func__, bRet);
    return bRet;

}

bool DaoServ::ReadDefaultKey(vector<unsigned char>& vchPubKey)
{
    return m_cWalletDb.ReadDefaultKey(vchPubKey);
}

bool DaoServ::WriteDefaultKey(const vector<unsigned char>& vchPubKey)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.WriteDefaultKey(pcTxn, vchPubKey); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    return bRet;

}

bool DaoServ::ReadSetting(const string& strKey, string& value)
{
    return m_cWalletDb.ReadSetting(strKey, value);
}

bool DaoServ::WriteSetting(const string& strKey, const string& value)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.WriteSetting(pcTxn, strKey, value); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
        return false;
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }

}

bool DaoServ::SetAddressBookName(const string& strAddress, const string& strName)
{
    DbTxn* pcTxn = NULL; 
    bool bRet = m_cDbenv.txn_begin(NULL, &pcTxn, 0);
    if (NULL == pcTxn || 0 != bRet) {
        return false;
    }
    
    try {
         bRet = m_cWalletDb.WriteName(pcTxn, strAddress, strName); 
    }
    catch(DbException& e)
    {
        pcTxn->abort();
        bRet = false;
    }
    
    if (bRet) {
       pcTxn->commit(0); 
    }
    else 
    { 
        pcTxn->abort();
    }
    printf("%s---%d---Ret[%d]\n", __FILE__, __LINE__, bRet);
    return bRet;
}
            
void DaoServ::DBFlush(bool fShutdown)
{
#if 0
// Flush log data to the actual data file
//  on all files that are not in use
printf("DBFlush(%s)\n", fShutdown ? "true" : "false");
{
    m_cDbenv.txn_checkpoint(0, 0, 0);
    map<string, int>::iterator mi = mapFileUseCount.begin();
    while (mi != mapFileUseCount.end())
    {
        string m_cStrFile = (*mi).first;
        int nRefCount = (*mi).second;
        if (nRefCount == 0)
        {
            m_cDbenv.lsn_reset(m_cStrFile.c_str(), 0);
            mapFileUseCount.erase(mi++);
        }
        else
            mi++;
        }
        if (fShutdown)
        {
            char** listp;
            if (mapFileUseCount.empty())
                m_cDbenv.log_archive(&listp, DB_ARCH_REMOVE);
            m_cDbenv.close(0);
            fDbEnvInit = false;
        }
    }
#endif
}

DaoServ::DaoServ(DbEnv* pDbEnv)
    : m_cAddrDb(pDbEnv),
      m_cReviewDb(pDbEnv),
      m_cTxDb(pDbEnv),
      m_cWalletDb(pDbEnv)
{
}

DaoServ::~DaoServ()
{
    m_cAddrDb.Close();
    m_cReviewDb.Close();
    m_cTxDb.Close();
    m_cWalletDb.Close();
    m_cDbenv.txn_checkpoint(0, 0, 0);
    m_cDbenv.close(0);
}

/* EOF */

