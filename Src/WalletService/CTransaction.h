/*
 * =====================================================================================
 *
 *       Filename:  CTransaction.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 10:57:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CTRANSACTION_H
#define EZ_BT_CTRANSACTION_H
#include "script.h"
#include "BlockEngine/CDiskTxPos.h"
#include "WalletService/CTxIn.h"
#include "WalletService/CTxOut.h"

namespace Enze
{

class CTxIndex;
//
// The basic transaction that is broadcasted on the network and contained in
// blocks.  A transaction can contain multiple inputs and outputs.
//
class CTransaction
{
public:
    int m_nCurVersion; // 交易的版本号，用于升级
    vector<CTxIn> m_vTxIn; // 交易对应的输入
    vector<CTxOut> m_vTxOut; // 交易对应的输出
    int m_nLockTime; // 交易对应的锁定时间


    CTransaction()
    {
        SetNull();
    }


    void SetNull();

    bool IsNull() const;

    uint256 GetHash() const;

    // 判断是否是最终的交易
    bool IsFinal() const;
	// 对比相同的交易哪一个更新点：对于相同输入的交易判断哪一个更新
    bool IsNewerThan(const CTransaction& old) const;

	// 判断当前交易是否是币基交易：币基交易的特点是交易输入大小为1，但是对应的输入的输出为空
    bool IsCoinBase() const;
	/* 对这边交易进行检查：
	(1)交易对应的输入或者输出列表都不能为空
	(2)交易对应的输出金额不能小于0
	(3)如果是币基交易，则对应的输入只能为1，且对应的输入签名大小不能大于100
	(4)如果是非币基交易，则对应的交易输入的输出不能为空
	*/
    bool CheckTransaction() const;
	// 判断当前的交易是否包含节点本身的交易（在输出列表中）
    bool IsMine() const;

	// 获得当前交易总的输入：借方
    int64 GetDebit() const;

	// 获得当前交易总的贷方金额：属于节点自身的
    int64 GetCredit() const;
	// 获取交易对应所有输出金额之和
    int64 GetValueOut() const;
	// 获取交易对应的最小交易费：大小小于10000字节则对应的最小交易费为0，否则为按照大小进行计算交易费
	// Transaction fee requirements, mainly only needed for flood control
	// Under 10K (about 80 inputs) is free for first 100 transactions
	// Base rate is 0.01 per KB
    int64 GetMinFee(bool fDiscount=false) const;

	// 从硬盘中进行读取
    bool ReadFromDisk(const CDiskTxPos& pos, FILE** pfileRet=NULL);


    friend bool operator==(const CTransaction& a, const CTransaction& b);

    friend bool operator!=(const CTransaction& a, const CTransaction& b);

    string ToString() const;

    void print() const;

	// 断开连接：释放交易对应输入的占用和将交易从对应的交易索引表中释放掉
    bool DisconnectInputs();
	// 交易输入链接，将对应的交易输入占用对应的交易输入的花费标记
    bool ConnectInputs(map<uint256, CTxIndex>& mapTestPool, const CDiskTxPos& posThisTx, int m_nCurHeight, int64& nFees, bool fBlock, bool fMiner, int64 nMinFee=0);
	// 客户端连接输入，对交易本身进行验证
	bool ClientConnectInputs();
	// 判断这笔交易是否应该被接受

    bool AcceptTransaction(bool fCheckInputs=true, bool* pfMissingInputs=NULL);

protected:
	// 将当前交易增加到内存池mapTransactions,mapNextTx中，并且更新交易更新的次数
    bool AddToMemoryPool();
public:
	// 将当前交易从内存对象mapTransactions，mapNextTx中移除，并且增加交易对应的更新次数
    bool RemoveFromMemoryPool();
};


} // end namespace

#endif /* EZ_BT_CTRANSACTION_H */
/* EOF */

