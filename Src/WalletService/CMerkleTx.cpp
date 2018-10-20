/*
 * =====================================================================================
 *
 *       Filename:  CMerkleTx.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 03:37:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "WalletService/CMerkleTx.h"
#include "WalletService/CTxIndex.h"
#include "WalletService/CWalletTx.h"
#include "BlockEngine/BlockEngine.h"
#include "BlockEngine/CBlock.h"
#include "BlockEngine/CBlockIndex.h"
#include "DAO/DaoServ.h"
#include "NetWorkService/CInv.h"
#include "NetWorkService/NetWorkServ.h"
using namespace Enze;
extern bool fClient ;
// 如果交易在对应的区块中，则设置交易对应的默克尔树分支
int CMerkleTx::SetMerkleBranch(const CBlock* pblock)
{
    if (fClient)
    {
        if (m_hashBlock == 0)
            return 0;
    }
    else
    {
        CBlock blockTmp;
        if (pblock == NULL)
        {
            // Load the block this tx is in
            CTxIndex txindex;
			// 根据当前交易的hash从数据库中查找对应的交易索引
            if (!DaoServ::getInstance()->ReadTxIndex(GetHash(), txindex))
                return 0;
			// 根据交易索引的信息从数据库中查询对应的block信息
            if (!blockTmp.ReadFromDisk(txindex.m_cDiskPos.m_nFile, txindex.m_cDiskPos.m_nBlockPos, true))
                return 0;
            pblock = &blockTmp;
        }

		// 根据交易对应的block的hash值
        // Update the tx's hashBlock
        m_hashBlock = pblock->GetHash();

		// 定位当前交易在block对应的交易列表中的索引
        // Locate the transaction
        for (m_nIndex = 0; m_nIndex < pblock->m_vTrans.size(); m_nIndex++)
            if (pblock->m_vTrans[m_nIndex] == *(CTransaction*)this)
                break;
        if (m_nIndex == pblock->m_vTrans.size())
        {
            m_vMerkleBranch.clear();
            m_nIndex = -1;
            printf("ERROR: SetMerkleBranch() : couldn't find tx in block\n");
            return 0;
        }

        // Fill in merkle branch
        m_vMerkleBranch = pblock->GetMerkleBranch(m_nIndex);
    }

    const map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->getMapBlockIndex();
    // Is the tx in a block that's in the main chain
    map<uint256, CBlockIndex*>::const_iterator mi = mapBlockIndex.find(m_hashBlock);
    if (mi == mapBlockIndex.end())
        return 0;
    CBlockIndex* pindex = (*mi).second;
    if (!pindex || !pindex->IsInMainChain())
        return 0;

	// 返回当前交易在主链中的高度（即是当前block相对于主链末尾之间中间隔了多少个block）
    return BlockEngine::getInstance()->getBestIndex()->m_nCurHeight - pindex->m_nCurHeight + 1;
}


// 获取默克尔交易在主链中的深度--当前块距离最长链末尾中间隔了多少个block
int CMerkleTx::GetDepthInMainChain() const
{
	// 交易的初始化，还没有被放入block中
    if (m_hashBlock == 0 || m_nIndex == -1)
        return 0;

	// 获取当前交易所在的block，从内存对象mapBlockIndex中获取
    // Find the block it claims to be in
    const map<uint256, CBlockIndex*>& mapBlockIndex = BlockEngine::getInstance()->getMapBlockIndex();
    map<uint256, CBlockIndex*>::const_iterator mi = mapBlockIndex.find(m_hashBlock);
    if (mi == mapBlockIndex.end())
        return 0;
    CBlockIndex* pindex = (*mi).second;
    if (!pindex || !pindex->IsInMainChain())
        return 0;

	// 标记默克尔交易是否已经校验，如果没有校验则进行校验，校验之后将这个值设为true
    // Make sure the merkle branch connects to this block
    if (!m_bMerkleVerified)
    {
		// 判断交易是否在block对应的默克尔树中
        if (CBlock::CheckMerkleBranch(GetHash(), m_vMerkleBranch, m_nIndex) != pindex->m_hashMerkleRoot)
            return 0;
        m_bMerkleVerified = true;
    }

    return BlockEngine::getInstance()->getBestIndex()->m_nCurHeight - pindex->m_nCurHeight + 1;
}


// 判断对应的块是否成熟，即是被其他矿工所接受认可，如果是非币基交易对应的为块成熟度为0，否则要进行计算
// 成熟度越小越好，说明当前交易被认可的度越高
int CMerkleTx::GetBlocksToMaturity() const
{
    if (!IsCoinBase())
        return 0;
    int iMax = max(0, (COINBASE_MATURITY+20) - GetDepthInMainChain());
    printf("CMerkleTx::GetBlocksToMaturity--[%d]\n", iMax);
    return iMax;
}


bool CMerkleTx::AcceptTransaction(bool fCheckInputs)
{
    if (fClient)
    {
        if (!IsInMainChain() && !ClientConnectInputs())
            return false;
        return CTransaction::AcceptTransaction(false);
    }
    else
    {
        return CTransaction::AcceptTransaction(fCheckInputs);
    }
}


/* EOF */

