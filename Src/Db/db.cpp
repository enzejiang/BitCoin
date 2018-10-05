// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#include <sys/stat.h>
#include <sys/types.h>
#include "headers.h"
#include "Db/db.h"
#include "Db/CAddrDB.h"
#include "TX/CWalletTx.h"
#include "CommonBase/base58.h"
#include "CommonBase/uint256.h"
#include "CommonBase/BlockEngine.h"
#include "Block/CBlockIndex.h"
//
// CDB
//

//static CCriticalSection cs_db;
bool fDbEnvInit = false;
//DbEnv dbenv(0);
DbEnv dbenv((u_int32_t)0);
map<string, int> mapFileUseCount;

class CDBInit
{
public:
    CDBInit()
    {
    }
    ~CDBInit()
    {
        if (fDbEnvInit)
        {
            dbenv.close(0);
            fDbEnvInit = false;
        }
    }
}
instance_of_cdbinit;


void DBFlush(bool fShutdown)
{
    // Flush log data to the actual data file
    //  on all files that are not in use
    printf("DBFlush(%s)\n", fShutdown ? "true" : "false");
    //CRITICAL_BLOCK(cs_db)
    {
        dbenv.txn_checkpoint(0, 0, 0);
        map<string, int>::iterator mi = mapFileUseCount.begin();
        while (mi != mapFileUseCount.end())
        {
            string m_cStrFile = (*mi).first;
            int nRefCount = (*mi).second;
            if (nRefCount == 0)
            {
                dbenv.lsn_reset(m_cStrFile.c_str(), 0);
                mapFileUseCount.erase(mi++);
            }
            else
                mi++;
        }
        if (fShutdown)
        {
            char** listp;
            if (mapFileUseCount.empty())
                dbenv.log_archive(&listp, DB_ARCH_REMOVE);
            dbenv.close(0);
            fDbEnvInit = false;
        }
    }
}




CBlockIndex* InsertBlockIndex(uint256 hash)
{
    if (hash == 0)
        return NULL;
    map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->mapBlockIndex;
    // Return existing
    map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
    if (mi != mapBlockIndex.end())
        return (*mi).second;

    // Create new
    CBlockIndex* pindexNew = new CBlockIndex();
    if (!pindexNew)
        throw runtime_error("LoadBlockIndex() : new CBlockIndex failed");
    mi = mapBlockIndex.insert(make_pair(hash, pindexNew)).first;
    pindexNew->m_pHashBlock = &((*mi).first);

    return pindexNew;
}




bool LoadAddresses()
{
    return CAddrDB("cr+").LoadAddresses();
}


/* EOF */
