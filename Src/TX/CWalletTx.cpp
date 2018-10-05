/*
 * =====================================================================================
 *
 *       Filename:  CWalletTx.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:21:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "TX/CWalletTx.h"
#include "Block/CBlockIndex.h"
#include "Network/CInv.h"
#include "Network/net.h"
// 获取交易时间
int64 CWalletTx::GetTxTime() const
{
    if (!m_bTimeReceivedIsTxTime && m_hashBlock != 0)
    {
        // If we did not receive the transaction directly, we rely on the block's
        // time to figure out when it happened.  We use the median over a range
        // of blocks to try to filter out inaccurate block times.
        map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(m_hashBlock);
        if (mi != mapBlockIndex.end())
        {
            CBlockIndex* pindex = (*mi).second;
            if (pindex)
                return pindex->GetMedianTime();
        }
    }
    return m_m_uTimeReceived;
}



// 增加支持的交易
void CWalletTx::AddSupportingTransactions(CTxDB& txdb)
{
    m_vPrevTx.clear();

    const int COPY_DEPTH = 3;
	// 如果当前交易所在的block和最长链末尾之间的block数量小于3
    if (SetMerkleBranch() < COPY_DEPTH)
    {
        vector<uint256> vWorkQueue;// 对应当前交易输入对应的交易的hash值
        foreach(const CTxIn& txin, m_vTxIn)
            vWorkQueue.push_back(txin.m_cPrevOut.m_u256Hash);

        // This critsect is OK because txdb is already open
//        CRITICAL_BLOCK(cs_mapWallet)
        {
            map<uint256, const CMerkleTx*> mapWalletPrev;
            set<uint256> setAlreadyDone;
            for (int i = 0; i < vWorkQueue.size(); i++)
            {
                uint256 hash = vWorkQueue[i];
                if (setAlreadyDone.count(hash))
                    continue;
                setAlreadyDone.insert(hash);

                CMerkleTx tx;
                if (mapWallet.count(hash))
                {
                    tx = mapWallet[hash];
                    foreach(const CMerkleTx& txWalletPrev, mapWallet[hash].m_vPrevTx)
                        mapWalletPrev[txWalletPrev.GetHash()] = &txWalletPrev;
                }
                else if (mapWalletPrev.count(hash))
                {
                    tx = *mapWalletPrev[hash];
                }
                else if (!fClient && txdb.ReadDiskTx(hash, tx))
                {
                    ;
                }
                else
                {
                    printf("ERROR: AddSupportingTransactions() : unsupported transaction\n");
                    continue;
                }

                int nDepth = tx.SetMerkleBranch();
                m_vPrevTx.push_back(tx);

                if (nDepth < COPY_DEPTH)
                    foreach(const CTxIn& txin, tx.m_vTxIn)
                        vWorkQueue.push_back(txin.m_cPrevOut.m_u256Hash);
            }
        }
    }

    reverse(m_vPrevTx.begin(), m_vPrevTx.end());
}

// 钱包交易进行转播
void CWalletTx::RelayWalletTransaction(CTxDB& txdb)
{
    // 对于那些交易所在block到最长链的block之间的距离小于3的需要对这些交易进行转播
    foreach(const CMerkleTx& tx, m_vPrevTx)
    {
        if (!tx.IsCoinBase())
        {
            uint256 hash = tx.GetHash();
            if (!txdb.ContainsTx(hash))
                RelayMessage(CInv(MSG_TX, hash), (CTransaction)tx);
        }
    }
    if (!IsCoinBase())
    {
        uint256 hash = GetHash();
        if (!txdb.ContainsTx(hash))
        {
            printf("Relaying wtx %s\n", hash.ToString().substr(0,6).c_str());
            RelayMessage(CInv(MSG_TX, hash), (CTransaction)*this);
        }
    }
}

//bool CWalletTx::AcceptTransaction()
//{
//    CTxDB txdb("r");
//    return AcceptTransaction(txdb);
//}
// 判断当前交易是否能够被接收
bool CWalletTx::AcceptWalletTransaction(CTxDB& txdb, bool fCheckInputs)
{
 //   CRITICAL_BLOCK(cs_mapTransactions)
    {
        foreach(CMerkleTx& tx, m_vPrevTx)
        {
            if (!tx.IsCoinBase())
            {
                uint256 hash = tx.GetHash();
                if (!mapTransactions.count(hash) && !txdb.ContainsTx(hash))
                    tx.AcceptTransaction(txdb, fCheckInputs);
            }
        }
        if (!IsCoinBase())
            return AcceptTransaction(txdb, fCheckInputs);
    }
    return true;
}


/* EOF */

