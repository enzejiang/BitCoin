/*
 * =====================================================================================
 *
 *       Filename:  CBlockIndex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 05:27:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CXX_BT_CBLOCK_INDEX_H
#define CXX_BT_CBLOCK_INDEX_H

#include "Block/CBlock.h"
//
// The block chain is a tree shaped structure starting with the
// genesis block at the root, with each block potentially having multiple
// candidates to be the next block.  m_pPrevBlkIndex and m_pNextBlkIndex link a path through the
// main/longest chain.  A blockindex may have multiple m_pPrevBlkIndex pointing back
// to it, but m_pNextBlkIndex will only point forward to the longest branch, or will
// be null if the block is not part of the longest chain.
//
// 如果块索引对应的pNext不为空，则这个块索引一定对应的是主链
// 块索引
class CBlockIndex
{
public:
    const uint256* m_pHashBlock; // 对应块hash值指针
    CBlockIndex* m_pPrevBlkIndex; // 指向前一个blockIndex
    CBlockIndex* m_pNextBlkIndex; // 指向当前区块索引的下一个，只有当前区块索引在主链上的时候，这个值才是非空
	// 块所在文件中的信息
    unsigned int m_nFile; 
    unsigned int m_nBlockPos;
    int m_nCurHeight; // 块索引在最长链的深度，即是中间隔了多少个block，即是从创世区块到当前区块中间隔了多少个区块

    // block header 块的头部信息
    int m_nCurVersion;
    uint256 m_hashMerkleRoot;
	// 取前11个区块对应的创建时间平均值
    unsigned int m_uTime;// 块创建时间，取块链上时间中位数
    unsigned int m_uBits;
    unsigned int m_uNonce;


    CBlockIndex()
    {
        m_pHashBlock = NULL;
        m_pPrevBlkIndex = NULL;
        m_pNextBlkIndex = NULL;
        m_nFile = 0;
        m_nBlockPos = 0;
        m_nCurHeight = 0;

        m_nCurVersion       = 0;
        m_hashMerkleRoot = 0;
        m_uTime          = 0;
        m_uBits          = 0;
        m_uNonce         = 0;
    }

    CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block)
    {
        m_pHashBlock = NULL;
        m_pPrevBlkIndex = NULL;
        m_pNextBlkIndex = NULL;
        m_nFile = nFileIn;
        m_nBlockPos = nBlockPosIn;
        m_nCurHeight = 0;

        m_nCurVersion       = block.m_nCurVersion;
        m_hashMerkleRoot = block.m_hashMerkleRoot;
        m_uTime          = block.m_uTime;
        m_uBits          = block.m_uBits;
        m_uNonce         = block.m_uNonce;
    }

    uint256 GetBlockHash() const
    {
        return *m_pHashBlock;
    }

	// 判断是否在主链，通过m_pNextBlkIndex是否为空和当前块索引指针是否就是最长链指针
    bool IsInMainChain() const
    {
        return (m_pNextBlkIndex || this == pindexBest);
    }

	// 从文件中移除对应的块
    bool EraseBlockFromDisk()
    {
        // Open history file
        CAutoFile fileout = OpenBlockFile(m_nFile, m_nBlockPos, "rb+");
        if (!fileout)
            return false;

		// 在文件对应的位置重新写一个空块，这样就相当于从文件中删除对应的当期块
        // Overwrite with empty null block
        CBlock block;
        block.SetNull();
        fileout << block;

        return true;
    }

	// 取前11个区块对应的创建时间平均值
    enum { nMedianTimeSpan=11 };

	// 从当前块往前推，获取过去对应的中位数时间，在对应的区块链中获取每一个区块对应的时间，然后取中位数
    int64 GetMedianTimePast() const
    {
        unsigned int pmedian[nMedianTimeSpan];
        unsigned int* pbegin = &pmedian[nMedianTimeSpan];
        unsigned int* pend = &pmedian[nMedianTimeSpan];

        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan && pindex; i++, pindex = pindex->m_pPrevBlkIndex)
            *(--pbegin) = pindex->m_uTime;

        sort(pbegin, pend);
        return pbegin[(pend - pbegin)/2];
    }
	// 从当前块往后推，取中位数时间
    int64 GetMedianTime() const
    {
        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan/2; i++)
        {
            if (!pindex->m_pNextBlkIndex)
                return m_uTime;
            pindex = pindex->m_pNextBlkIndex;
        }
        return pindex->GetMedianTimePast();
    }



    string ToString() const
    {
        return strprintf("CBlockIndex(nprev=%08x, m_pNextBlkIndex=%08x, m_nFile=%d, m_nBlockPos=%-6d m_nCurHeight=%d, merkle=%s, hashBlock=%s)",
            m_pPrevBlkIndex, m_pNextBlkIndex, m_nFile, m_nBlockPos, m_nCurHeight,
            m_hashMerkleRoot.ToString().substr(0,6).c_str(),
            GetBlockHash().ToString().substr(0,14).c_str());
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};


#endif /* CXX_BT_CBLOCK_INDEX_H */
/* EOF */

