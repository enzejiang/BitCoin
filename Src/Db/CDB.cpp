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
#include "Db/CDB.h"
#include "Db/db.h"
#include "TX/CWalletTx.h"

CDB::CDB(const char* pszFile, const char* pszMode, bool fTxn) : m_pDb(NULL)
{
    int ret;
    if (pszFile == NULL)
        return;

    bool fCreate = strchr(pszMode, 'c');
    bool fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));
    unsigned int nFlags = DB_THREAD;
    if (fCreate)
        nFlags |= DB_CREATE;
    else if (fReadOnly)
        nFlags |= DB_RDONLY;
    if (!fReadOnly || fTxn)
        nFlags |= DB_AUTO_COMMIT;

//    //CRITICAL_BLOCK(cs_db)
    {
        if (!fDbEnvInit)
        {
            string strAppDir = BlockEngine::getInstance()->GetAppDir();
            string strLogDir = strAppDir + "\/database";
            mkdir(strLogDir.c_str(), 0775);
            printf("dbenv.open strAppDir=%s\n", strAppDir.c_str());

            dbenv.set_lg_dir(strLogDir.c_str());
            dbenv.set_lg_max(10000000);
            dbenv.set_lk_max_locks(10000);
            dbenv.set_lk_max_objects(10000);
            dbenv.set_errfile(fopen("db.log", "a")); /// debug
            ///dbenv.log_set_config(DB_LOG_AUTO_REMOVE, 1); /// causes corruption
            ret = dbenv.open(strAppDir.c_str(),
                             DB_CREATE     |
                             DB_INIT_LOCK  |
                             DB_INIT_LOG   |
                             DB_INIT_MPOOL |
                             DB_INIT_TXN   |
                             DB_THREAD     |
                             DB_PRIVATE    |
                             DB_RECOVER,
                             0);
            if (ret > 0)
                throw runtime_error(strprintf("CDB() : error %d opening database environment\n", ret));
            fDbEnvInit = true;
        }

        m_cStrFile = pszFile;
        ++mapFileUseCount[m_cStrFile];
    }

    m_pDb = new Db(&dbenv, 0);

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
   //     //CRITICAL_BLOCK(cs_db)
            --mapFileUseCount[m_cStrFile];
        m_cStrFile = "";
        throw runtime_error(strprintf("CDB() : can't open database file %s, error %d\n", pszFile, ret));
    }

    if (fCreate && !Exists(string("version")))
        WriteVersion(VERSION);

    RandAddSeed();
}

void CDB::Close()
{
    if (!m_pDb)
        return;
    if (!m_vDbTxn.empty())
        m_vDbTxn.front()->abort();
    m_vDbTxn.clear();
    m_pDb->close(0);
    delete m_pDb;
    m_pDb = NULL;
    dbenv.txn_checkpoint(0, 0, 0);

    ////CRITICAL_BLOCK(cs_db)
        --mapFileUseCount[m_cStrFile];

    RandAddSeed();
}


/* EOF */

