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
#ifndef CXX_BT_CTXDB_H
#define CXX_BT_CTXDB_H
#include "Db/CDB.h"

class uint256;
class uint160;
class CDiskBlockIndex;
class CTransaction;
class CTxIndex;
class CDiskTxPos;
class COutPoint;
extern bool fClient;
class CTxDB : public CDB
{
public:
    CTxDB(const char* pszMode="r+", bool fTxn=false) : CDB(!fClient ? "blkindex.dat" : NULL, pszMode, fTxn) { }
private:
    CTxDB(const CTxDB&);
    void operator=(const CTxDB&);
public:
    bool ReadTxIndex(const uint256& hash, CTxIndex& txindex);
    bool UpdateTxIndex(const uint256& hash, const CTxIndex& txindex);
    bool AddTxIndex(const CTransaction& tx, const CDiskTxPos& pos, int nHeight);
    bool EraseTxIndex(const CTransaction& tx);
    bool ContainsTx(const uint256& hash);
    bool ReadOwnerTxes(const uint160& hash160, int nHeight, vector<CTransaction>& vtx);
    bool ReadDiskTx(const uint256& hash, CTransaction& tx, CTxIndex& txindex);
    bool ReadDiskTx(const uint256& hash, CTransaction& tx);
    bool ReadDiskTx(const COutPoint& outpoint, CTransaction& tx, CTxIndex& txindex);
    bool ReadDiskTx(const COutPoint& outpoint, CTransaction& tx);
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool EraseBlockIndex(const uint256& hash);
    bool ReadHashBestChain(uint256& hashBestChain);
    bool WriteHashBestChain(uint256 hashBestChain);
    bool LoadBlockIndex();
};

#endif /* CXX_BT_CTXDB_H */
/* EOF */

