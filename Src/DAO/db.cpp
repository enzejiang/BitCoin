// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#include <sys/stat.h>
#include <sys/types.h>
#include "headers.h"
#include "DAO/db.h"
#include "DAO/CAddrDB.h"
#include "WalletService/CWalletTx.h"
#include "CommonBase/base58.h"
#include "CommonBase/uint256.h"
#include "BlockEngine/BlockEngine.h"
#include "BlockEngine/CBlockIndex.h"

using namespace Enze;
//
// CDB
//
#if 0

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



#endif


CBlockIndex* Enze::InsertBlockIndex(uint256 hash)
{
    if (hash == 0)
        return NULL;
    const map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->getMapBlockIndex();
    // Return existing
    map<uint256, CBlockIndex*>::const_iterator mi = mapBlockIndex.find(hash);
    if (mi != mapBlockIndex.end())
        return (*mi).second;

    // Create new
    CBlockIndex* pindexNew = new CBlockIndex();
    if (!pindexNew)
        throw runtime_error("LoadBlockIndex() : new CBlockIndex failed");
    mi = BlockEngine::getInstance()->insertBlockIndex(make_pair(hash, pindexNew));//mapBlockIndex.insert(make_pair(hash, pindexNew)).first;
    pindexNew->m_pHashBlock = &((*mi).first);

    return pindexNew;
}




/* EOF */
