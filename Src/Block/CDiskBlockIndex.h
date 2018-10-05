/*
 * =====================================================================================
 *
 *       Filename:  CDiskBlockIndex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 05:29:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_CDISK_BLOCK_INDEX_H
#define CXX_CDISK_BLOCK_INDEX_H
#include "Block/CBlockIndex.h"
//
// Used to marshal pointers into hashes for db storage.
// 用于将指针替换成hash值用于数据库存储
//
class CDiskBlockIndex : public CBlockIndex
{
public:
    uint256 m_PrevHash; // block对应的hash值，将指针变成对应的hash
    uint256 m_NextHash;

    CDiskBlockIndex()
    {
        m_PrevHash = 0;
        m_NextHash = 0;
    }

    explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex)
    {
        m_PrevHash = (m_pPrevBlkIndex ? m_pPrevBlkIndex->GetBlockHash() : 0);
        m_NextHash = (m_pNextBlkIndex ? m_pNextBlkIndex->GetBlockHash() : 0);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(m_nCurVersion);

        READWRITE(m_NextHash);
        READWRITE(m_nFile);
        READWRITE(m_nBlockPos);
        READWRITE(m_nCurHeight);

        // block header
        READWRITE(this->m_nCurVersion);
        READWRITE(m_PrevHash);
    char fFromMe;
    char fSpent; // 是否花费交易标记
        READWRITE(m_hashMerkleRoot);
        READWRITE(m_uTime);
        READWRITE(m_uBits);
        READWRITE(m_uNonce);
    )

    uint256 GetBlockHash() const
    {
        CBlock block;
        block.m_nCurVersion        = m_nCurVersion;
        block.m_hashPrevBlock   = m_PrevHash;
        block.m_hashMerkleRoot  = m_hashMerkleRoot;
        block.m_uTime           = m_uTime;
        block.m_uBits           = m_uBits;
        block.m_uNonce          = m_uNonce;
        return block.GetHash(); // 块的hash仅仅包含上面这些内容
    }


    string ToString() const
    {
        string str = "CDiskBlockIndex(";
        str += CBlockIndex::ToString();
        str += strprintf("\n                hashBlock=%s, hashPrev=%s, hashNext=%s)",
            GetBlockHash().ToString().c_str(),
            m_PrevHash.ToString().substr(0,14).c_str(),
            m_NextHash.ToString().substr(0,14).c_str());
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};


#endif /* CXX_CDISK_BLOCK_INDEX_H */
/* EOF */

