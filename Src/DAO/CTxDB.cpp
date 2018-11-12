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
#include "DAO/CTxDB.h"
#include "DAO/db.h"
#include "WalletService/CTransaction.h"
#include "WalletService/CTxIndex.h"
#include "WalletService/CTxOutPoint.h"
#include "BlockEngine/CDiskBlockIndex.h"
#include "BlockEngine/CDiskTxPos.h"
#include "BlockEngine/BlockEngine.h"
#include "CommonBase/uint256.h"
#include "CommonBase/ProtocSerialize.h"
#include "ProtocSrc/TxIndex.pb.h"
#include "ProtocSrc/DiskBlockIndex.pb.h"
//
// CTxDB
//
namespace Enze
{


bool CTxDB::ReadTxIndex(const uint256& hash, CTxIndex& txindex)
{
    txindex.SetNull();
    string key = string("tx")+hash.ToString();
    TxIndex cProtocData;
    bool bRet = Read(key,cProtocData);
    if (bRet) 
    {
        return UnSeriaTxIndex(cProtocData, txindex);
    }
    
    return bRet;
}

bool CTxDB::UpdateTxIndex(DbTxn*txn, const uint256& hash, const CTxIndex& txindex)
{
    string key = string("tx")+hash.ToString();
    TxIndex cProtocData;
    
    if (Enze::SeriaTxIndex(txindex, cProtocData))
    {
        return Write(txn, key,cProtocData); 
    }
    
    return false;
}

bool CTxDB::AddTxIndex(DbTxn*txn,const CTransaction& tx, const CDiskTxPos& pos, int m_nCurHeight)
{
    // Add to tx index
    uint256 hash = tx.GetHash();
    CTxIndex txindex(pos, tx.m_vTxOut.size());
    return UpdateTxIndex(txn, tx.GetHash(), txindex);
   // return Write(make_pair(string("tx"), hash), txindex);
}

bool CTxDB::EraseTxIndex(DbTxn*txn,const CTransaction& tx)
{
  //  uint256 hash = tx.GetHash();
    string key = string("tx")+tx.GetHash().ToString();
    return Erase(txn,key);
}

bool CTxDB::ContainsTx(const uint256& hash)
{
    string key = string("tx")+hash.ToString();
    return Exists(key);
}

bool CTxDB::ReadOwnerTxes(const uint160& hash160, int nMim_nCurHeight, vector<CTransaction>& vtx)
{
    printf("%s--%d---\n", __FILE__, __LINE__);
    return false;
    vtx.clear();

    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor)
        return false;

#if 0
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
#endif 
    return true;
}

bool CTxDB::ReadDiskTx(const uint256& hash, CTransaction& tx, CTxIndex& txindex)
{
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

bool CTxDB::WriteBlockIndex(DbTxn*txn,const CDiskBlockIndex& blockindex)
{
    string key = string("blockindex") + blockindex.GetBlockHash().ToString();
    printf("CTxDB::WriteBlockIndex---key[%s]--[%s]\n", key.c_str(), blockindex.m_hashMerkleRoot.ToString().c_str());
    DiskBlockIndex cProtocData;

    if (Enze::SeriaDiskBlockIndex(blockindex, cProtocData))
        return Write(txn, key, cProtocData);

    return false;
}

bool CTxDB::EraseBlockIndex(DbTxn*txn,const uint256& hash)
{
    string key = string("blockindex") + hash.ToString();
    return Erase(txn, key);
}

bool CTxDB::ReadHashBestChain(uint256& hashBestChain)
{
    string value;
    bool bRet = Read(string("hashBestChain"), value);
    if (bRet) {
        hashBestChain.SetHex(value);
    }
    return bRet;
}

bool CTxDB::WriteHashBestChain(DbTxn*txn, const uint256& hashBestChain)
{
    return Write(txn, string("hashBestChain"), hashBestChain.ToString());
}

bool CTxDB::LoadBlockIndex()
{
    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor)
        return false;
    const CBlockIndex* pindexGenesisBlock = BlockEngine::getInstance()->getGenesisBlock();//pindexGenesisBlock;
    const uint256& hashGenesisBlock  = BlockEngine::getInstance()->getGenesisHash();//hashGenesisBlock;
    const uint256& hashBestChain = BlockEngine::getInstance()->getHashBestChain();//hashBestChain;
    CBlockIndex* pindexBest = BlockEngine::getInstance()->getBestIndex();//pindexBest;
    map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->getMapBlockIndex();//mapBlockIndex;
    const int nBestHeight = BlockEngine::getInstance()->getBestHeight();//nBestHeight;
    unsigned int fFlags = DB_SET_RANGE;
    loop
    {
        // Read next record
        Dbt datKey;
        if (fFlags == DB_SET_RANGE)
        {
            string strKey = string("blockindex");//+uint256(0).ToString();
            datKey.set_size(strKey.length());
            datKey.set_data((void*)strKey.c_str());
        }
        
        Dbt datValue;
        int ret = ReadAtCursor(pcursor, datKey, datValue, fFlags);

        if (ret == DB_NOTFOUND)
        {
            printf("Not Found cursor %s---%d\n", __FILE__, __LINE__);
            break;
        }
        else if (ret != 0)
        {
            printf("Not Found cursor %s---%d\n", __FILE__, __LINE__);
            return false;
        }

        printf("Find cursor Size[%d]---keySize[%d]--[%d]\n", datValue.get_size(), datKey.get_size(), ret);
        // Unserialize
        DiskBlockIndex diskindex;
        bool bState = diskindex.ParsePartialFromArray(datValue.get_data(), datValue.get_size());
        if (!bState) {
            printf("Error CTxDB::LoadBlockIndex--[%s]--[%s]\n", diskindex.hashprev().c_str(), diskindex.hashmerkleroot().c_str());
            break;
        }
        printf("CTxDB::LoadBlockIndex--[%s]--[%s]\n", diskindex.hashprev().c_str(), diskindex.hashmerkleroot().c_str());
        CBlock cBlock;
        cBlock.m_uBits = diskindex.nbit();
        cBlock.m_uNonce = diskindex.nnonce();
        cBlock.m_uTime  = diskindex.ntime();
        cBlock.m_hashPrevBlock.SetHex(diskindex.hashprev());
        cBlock.m_hashMerkleRoot.SetHex(diskindex.hashmerkleroot());
        cBlock.m_nCurVersion = diskindex.nversion();
        uint256 prevHash;
        prevHash.SetHex(diskindex.hashprev());
        uint256 nextHash;
        nextHash.SetHex(diskindex.hashnext());
        // Construct block index object
        CBlockIndex* pindexNew = InsertBlockIndex(cBlock.GetHash());
        pindexNew->m_pPrevBlkIndex          = InsertBlockIndex(prevHash);
        pindexNew->m_pNextBlkIndex          = InsertBlockIndex(nextHash);
        pindexNew->m_nFile          = diskindex.nfile();
        pindexNew->m_nBlockPos      = diskindex.nblockpos();
        pindexNew->m_nCurHeight        = diskindex.nheight();
        pindexNew->m_nCurVersion       = cBlock.m_nCurVersion;
        pindexNew->m_hashMerkleRoot = cBlock.m_hashMerkleRoot;
        pindexNew->m_uTime          = cBlock.m_uTime;
        pindexNew->m_uBits          = cBlock.m_uBits;
        pindexNew->m_uNonce         = cBlock.m_uNonce;

        // Watch for genesis block and best block
        if (pindexGenesisBlock == NULL && cBlock.GetHash() == hashGenesisBlock)
            pindexGenesisBlock = pindexNew;


        if (fFlags != DB_SET_RANGE)
        {
            memset(datKey.get_data(), 0, datKey.get_size());
            free(datKey.get_data());
        }
        memset(datValue.get_data(), 0, datValue.get_size());
        free(datValue.get_data());
        fFlags = DB_NEXT;
    }

    pcursor->close();
   // pcursor->del(0);
    uint256 BestHash;
    printf("CTxDB::LoadBlockIndex---read best hash\n");
    if (!ReadHashBestChain(BestHash))
    {
        if (BlockEngine::getInstance()->getGenesisBlock() == NULL)
        {
            printf("CTxDB::LoadBlockIndex---no  getGenesisBlock\n");
            return true;
        }

        return error("CTxDB::LoadBlockIndex() : hashBestChain not found\n");
    }
    else
    {
        BlockEngine::getInstance()->setBestChain(BestHash);
    }
     printf("CTxDB::LoadBlockIndex---read best hash--end\n");
    if (!mapBlockIndex.count(BestHash))
        return error("CTxDB::LoadBlockIndex() : blockindex for hashBestChain not found\n");
    printf("error %s--%d\n", __FILE__, __LINE__);
    BlockEngine::getInstance()->setBestIndex(mapBlockIndex[BestHash]);
    pindexBest = mapBlockIndex[BestHash];
    BlockEngine::getInstance()->setBestHeight(pindexBest->m_nCurHeight);
    printf("LoadBlockIndex(): hashBestChain=%s  height=%d\n", hashBestChain.ToString().substr(0,14).c_str(), nBestHeight);

    return true;
}


}
/* EOF */

