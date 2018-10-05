/*
 * =====================================================================================
 *
 *       Filename:  CMerkleTx.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 03:38:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_BT_CMERKLETX_H
#define CXX_BT_CMERKLETX_H

#include "TX/CTransaction.h"

class CBlock;
class CTxDB;
//
// A transaction with a merkle branch linking it to the block chain
//
class CMerkleTx : public CTransaction
{
public:
    uint256 m_hashBlock;// 交易所在block对应的hash值，因为block中有对应整个交易的默克尔树，这样才能根据分支来校验当前交易是否在block中
    vector<uint256> m_vMerkleBranch; // 当前交易对应的默克尔分支
    int m_nIndex;// 当前交易在对应的block对应的输入m_vTrans列表中的索引，CMerkleTx就是根据索引来计算这个交易对应的默克尔树分支的

    // memory only
    mutable bool m_bMerkleVerified;// 标记默克尔交易是否已经校验，如果没有校验则进行校验，校验之后将这个值设为true


    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        m_hashBlock = 0;
        m_nIndex = -1;
        m_bMerkleVerified = false;
    }

	// 获取默克尔树对应的贷方金额的时候，对于币基交易，一定要等对应的block足够成熟了才能使用
    int64 GetCredit() const
    {
        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;
        return CTransaction::GetCredit();
    }

    IMPLEMENT_SERIALIZE
    (
        nSerSize += SerReadWrite(s, *(CTransaction*)this, nType, m_nCurVersion, ser_action);
        nVersion = this->m_nCurVersion;
        READWRITE(m_hashBlock);
        READWRITE(m_vMerkleBranch);
        READWRITE(m_nIndex);
    )

    // 如果交易在对应的区块中，则设置交易对应的默克尔树分支
    int SetMerkleBranch(const CBlock* pblock=NULL);
	//获取默克尔交易在主链中的深度--当前块距离最长链末尾中间隔了多少个block
    int GetDepthInMainChain() const;
	// 判断当前交易是否在主链上
    bool IsInMainChain() const { return GetDepthInMainChain() > 0; }
	// 判断对应的块是否成熟，即是被其他矿工所接受认可，如果是非币基交易对应的为块成熟度为0，否则要进行计算
    // 成熟度越小越好，说明当前交易被认可的度越高
    int GetBlocksToMaturity() const;
	// 判断这边交易能不能被接受，如果能接受将对应的交易放入全局变量中mapTransactions，mapNextTx中
    bool AcceptTransaction(CTxDB& txdb, bool fCheckInputs=true);
    bool AcceptTransaction();// { CTxDB txdb("r"); return AcceptTransaction(txdb); }
};



#endif /* CXX_BT_CMERKLETX_H */
/* EOF */

