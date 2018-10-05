/*
 * =====================================================================================
 *
 *       Filename:  CTxDB.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:42:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "Db/CTxDB.h"
#include "Db/db.h"
#include "TX/CTransaction.h"
#include "TX/CTxIndex.h"
#include "TX/CTxOutPoint.h"
#include "TX/CWalletTx.h"
#include "Block/CDiskBlockIndex.h"
#include "Block/CDiskTxPos.h"
#include "CommonBase/uint256.h"
//
// CTxDB
//

bool CTxDB::ReadTxIndex(const uint256& hash, CTxIndex& txindex)
{
    assert(!fClient);
    txindex.SetNull();
    return Read(make_pair(string("tx"), hash), txindex);
}

bool CTxDB::UpdateTxIndex(const uint256& hash, const CTxIndex& txindex)
{
    assert(!fClient);
    return Write(make_pair(string("tx"), hash), txindex);
}

bool CTxDB::AddTxIndex(const CTransaction& tx, const CDiskTxPos& pos, int m_nCurHeight)
{
    assert(!fClient);

    // Add to tx index
    uint256 hash = tx.GetHash();
    CTxIndex txindex(pos, tx.m_vTxOut.size());
    return Write(make_pair(string("tx"), hash), txindex);
}

bool CTxDB::EraseTxIndex(const CTransaction& tx)
{
    assert(!fClient);
    uint256 hash = tx.GetHash();

    return Erase(make_pair(string("tx"), hash));
}

bool CTxDB::ContainsTx(const uint256& hash)
{
    assert(!fClient);
    return Exists(make_pair(string("tx"), hash));
}

bool CTxDB::ReadOwnerTxes(const uint160& hash160, int nMim_nCurHeight, vector<CTransaction>& vtx)
{
    assert(!fClient);
    vtx.clear();

    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor)
        return false;

    unsigned int fFlags = DB_SET_RANGE;
    loop
    {
        // Read next record
        CDataStream ssKey;
        if (fFlags == DB_SET_RANGE)
            ssKey << string("owner") << hash160 << CDiskTxPos(0, 0, 0);
        CDataStream ssValue;
        int ret = ReadAtCursor(pcursor, ssKey, ssValue, fFlags);
        fFlags = DB_NEXT;
        if (ret == DB_NOTFOUND)
            break;
        else if (ret != 0)
            return false;

        // Unserialize
        string strType;
        uint160 hashItem;
        CDiskTxPos pos;
        ssKey >> strType >> hashItem >> pos;
        int nItemHeight;
        ssValue >> nItemHeight;

        // Read transaction
        if (strType != "owner" || hashItem != hash160)
            break;
        if (nItemHeight >= nMim_nCurHeight)
        {
            vtx.resize(vtx.size()+1);
            if (!vtx.back().ReadFromDisk(pos))
                return false;
        }
    }
    return true;
}

bool CTxDB::ReadDiskTx(const uint256& hash, CTransaction& tx, CTxIndex& txindex)
{
    assert(!fClient);
    tx.SetNull();
    if (!ReadTxIndex(hash, txindex))
        return false;
    return (tx.ReadFromDisk(txindex.m_cDiskPos));
}

bool CTxDB::ReadDiskTx(const uint256& hash, CTransaction& tx)
{
    CTxIndex txindex;
    return ReadDiskTx(hash, tx, txindex);
}

bool CTxDB::ReadDiskTx(const COutPoint& outpoint, CTransaction& tx, CTxIndex& txindex)
{
    return ReadDiskTx(outpoint.m_u256Hash, tx, txindex);
}

bool CTxDB::ReadDiskTx(const COutPoint& outpoint, CTransaction& tx)
{
    CTxIndex txindex;
    return ReadDiskTx(outpoint.m_u256Hash, tx, txindex);
}

bool CTxDB::WriteBlockIndex(const CDiskBlockIndex& blockindex)
{
    return Write(make_pair(string("blockindex"), blockindex.GetBlockHash()), blockindex);
}

bool CTxDB::EraseBlockIndex(const uint256& hash)
{
    return Erase(make_pair(string("blockindex"), hash));
}

bool CTxDB::ReadHashBestChain(uint256& hashBestChain)
{
    return Read(string("hashBestChain"), hashBestChain);
}

bool CTxDB::WriteHashBestChain(uint256 hashBestChain)
{
    return Write(string("hashBestChain"), hashBestChain);
}

bool CTxDB::LoadBlockIndex()
{
    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor)
        return false;

    unsigned int fFlags = DB_SET_RANGE;
    loop
    {
        // Read next record
        CDataStream ssKey;
        if (fFlags == DB_SET_RANGE)
            ssKey << make_pair(string("blockindex"), uint256(0));
        CDataStream ssValue;
        int ret = ReadAtCursor(pcursor, ssKey, ssValue, fFlags);
        fFlags = DB_NEXT;
        if (ret == DB_NOTFOUND)
            break;
        else if (ret != 0)
            return false;

        // Unserialize
        string strType;
        ssKey >> strType;
        if (strType == "blockindex")
        {
            CDiskBlockIndex diskindex;
            ssValue >> diskindex;

            // Construct block index object
            CBlockIndex* pindexNew = InsertBlockIndex(diskindex.GetBlockHash());
            pindexNew->m_pPrevBlkIndex          = InsertBlockIndex(diskindex.m_PrevHash);
            pindexNew->m_pNextBlkIndex          = InsertBlockIndex(diskindex.m_NextHash);
            pindexNew->m_nFile          = diskindex.m_nFile;
            pindexNew->m_nBlockPos      = diskindex.m_nBlockPos;
            pindexNew->m_nCurHeight        = diskindex.m_nCurHeight;
            pindexNew->m_nCurVersion       = diskindex.m_nCurVersion;
            pindexNew->m_hashMerkleRoot = diskindex.m_hashMerkleRoot;
            pindexNew->m_uTime          = diskindex.m_uTime;
            pindexNew->m_uBits          = diskindex.m_uBits;
            pindexNew->m_uNonce         = diskindex.m_uNonce;

            // Watch for genesis block and best block
            if (pindexGenesisBlock == NULL && diskindex.GetBlockHash() == hashGenesisBlock)
                pindexGenesisBlock = pindexNew;
        }
        else
        {
            break;
        }
    }

    if (!ReadHashBestChain(hashBestChain))
    {
        if (pindexGenesisBlock == NULL)
            return true;
        return error("CTxDB::LoadBlockIndex() : hashBestChain not found\n");
    }

    if (!mapBlockIndex.count(hashBestChain))
        return error("CTxDB::LoadBlockIndex() : blockindex for hashBestChain not found\n");
    pindexBest = mapBlockIndex[hashBestChain];
    nBestHeight = pindexBest->m_nCurHeight;
    printf("LoadBlockIndex(): hashBestChain=%s  height=%d\n", hashBestChain.ToString().substr(0,14).c_str(), nBestHeight);

    return true;
}


/* EOF */

