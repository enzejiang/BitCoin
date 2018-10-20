/*
 * =====================================================================================
 *
 *       Filename:  CBlock.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 05:25:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CXX_BT_CBLOCK_H
#define CXX_BT_CBLOCK_H
#include "headers.h"
#include "CommonBase/uint256.h"
#include "WalletService/CTransaction.h"
#include "NetWorkService/CMessageHeader.h"

namespace Enze
{

    class CBlockIndex;
//
// Nodes collect new transactions into a block, hash them into a hash tree,
// and scan through nonce values to make the block's hash satisfy proof-of-work
// requirements.  When they solve the proof-of-work, they broadcast the block
// to everyone and the block is added to the block chain.  The first transaction
// in the block is a special one that creates a new coin owned by the creator
// of the block.
//
// Blocks are appended to blk0001.dat files on disk.  Their location on disk
// is indexed by CBlockIndex objects in memory.
//
// 块定义
class CBlock
{
public:
    // header
    int m_nCurVersion; // 块的版本，主要为了后续的升级使用
    uint256 m_hashPrevBlock; // 前一个块对应的hash
    uint256 m_hashMerkleRoot; // 默克尔对应的根
	// 取前11个区块对应的创建时间平均值
    unsigned int m_uTime; // 单位为秒，取区块链中对应的前多少个区块对应时间的中位数，如果不存在前一个则去当前时间
    unsigned int m_uBits; // 记录本区块难度
    unsigned int m_uNonce; // 工作量证明获得随机数，这个随机数正好满足当前挖矿对应的难度

    // network and disk
    vector<CTransaction> m_vTrans; // 块中交易列表

    // memory only
    mutable vector<uint256> m_vMerkleTree; // 整个交易对应的默克尔树列表


    CBlock()
    {
        SetNull();
    }


    void SetNull()
    {
        m_nCurVersion = 1;
        m_hashPrevBlock = 0;
        m_hashMerkleRoot = 0;
        m_uTime = 0;
        m_uBits = 0;
        m_uNonce = 0;
        m_vTrans.clear();
        m_vMerkleTree.clear();
    }

    bool IsNull() const
    {
        return (m_uBits == 0);
    }

	// 块hash值仅仅包含从m_nCurVersion 到 m_uNonce 的值
    uint256 GetHash() const
    {
        return Hash(BEGIN(m_nCurVersion), END(m_uNonce));
    }

	// 根据交易构建对应的默克尔树
    uint256 BuildMerkleTree() const;
	// 根据交易对应的索引获得交易对应的默克尔分支
    vector<uint256> GetMerkleBranch(int nIndex) const;
	// 判断当前对应的交易hash和默克尔分支来验证对应的交易是否在对应的blcok中
    static uint256 CheckMerkleBranch(uint256 hash, const vector<uint256>& vMerkleBranch, int nIndex);

	// 将block写入到文件中
    bool WriteToDisk(bool fWriteTransactions, unsigned int& nFileRet, unsigned int& nBlockPosRet);

	// 从文件中读取块信息
    bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions);

    void print() const
    {
        printf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, m_uTime=%u, m_uBits=%08x, m_uNonce=%u, m_vTrans=%d)\n",
            GetHash().ToString().substr(0,14).c_str(),
            m_nCurVersion,
            m_hashPrevBlock.ToString().substr(0,14).c_str(),
            m_hashMerkleRoot.ToString().substr(0,6).c_str(),
            m_uTime, m_uBits, m_uNonce,
            m_vTrans.size());
        for (int i = 0; i < m_vTrans.size(); i++)
        {
            printf("  ");
            m_vTrans[i].print();
        }
        printf("  m_vMerkleTree: ");
        for (int i = 0; i < m_vMerkleTree.size(); i++)
            printf("%s ", m_vMerkleTree[i].ToString().substr(0,6).c_str());
        printf("\n");
    }

	// 获取这个区块对应的价值（奖励+交易手续费）
    int64 GetBlockValue(int64 nFees) const;
	// 将一个区块block断开连接（就是释放区块对应的信息，同时释放区块对应的区块索引）
    bool DisconnectBlock(CBlockIndex* pindex);
	// 区块链接：每一个交易链接，增加到区块索引链中
    bool ConnectBlock(CBlockIndex* pindex);
	// 根据区块索引从数据库文件中读取对应的区块信息
    bool ReadFromDisk(const CBlockIndex* blockindex, bool fReadTransactions);
	// 将当前区块增加到对应的区块索引中
    bool AddToBlockIndex(unsigned int m_nFile, unsigned int m_nBlockPos);
	// 区块校验
    bool CheckBlock() const;
	// 判断当前区块能够被接收
    bool AcceptBlock();
};




}

#endif /* CXX_BT_CBLOCK_H */
/* EOF */

