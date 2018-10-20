/*
 * =====================================================================================
 *
 *       Filename:  CTxDB.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:40:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CTXDB_H
#define EZ_BT_CTXDB_H
#include "DAO/CDB.h"

namespace Enze 
{
    class uint256;
    class uint160;
    class CDiskBlockIndex;
    class CTransaction;
    class CTxIndex;
    class CDiskTxPos;
    class COutPoint;
    
    class CTxDB : public CDB
    {
    public:
        CTxDB(DbEnv*pDbenv = NULL, const char* pszMode="crw+", bool fTxn=false, bool bClient = false) : CDB(pDbenv, !bClient ? "blkindex.dat" : NULL, pszMode, fTxn) { }
        virtual ~CTxDB() {}
    private:
        CTxDB(const CTxDB&);
        CTxDB operator=(const CTxDB&);
    public:
        bool ReadTxIndex(const uint256& hash, CTxIndex& txindex);
        bool UpdateTxIndex(DbTxn*txn, const uint256& hash, const CTxIndex& txindex);
        bool AddTxIndex(DbTxn*txn,const CTransaction& tx, const CDiskTxPos& pos, int nHeight);
        bool EraseTxIndex(DbTxn*txn,const CTransaction& tx);
        bool ContainsTx(const uint256& hash);
        bool ReadOwnerTxes(const uint160& hash160, int nHeight, vector<CTransaction>& vtx);
        bool ReadDiskTx(const uint256& hash, CTransaction& tx, CTxIndex& txindex);
        bool ReadDiskTx(const uint256& hash, CTransaction& tx);
        bool ReadDiskTx(const COutPoint& outpoint, CTransaction& tx, CTxIndex& txindex);
        bool ReadDiskTx(const COutPoint& outpoint, CTransaction& tx);
        bool WriteBlockIndex(DbTxn*txn,const CDiskBlockIndex& blockindex);
        bool EraseBlockIndex(DbTxn*txn,const uint256& hash);
        bool ReadHashBestChain(uint256& hashBestChain);
        bool WriteHashBestChain(DbTxn*txn,const uint256& hashBestChain);
        bool LoadBlockIndex();
    };

}
#endif /* EZ_BT_CTXDB_H */
/* EOF */

