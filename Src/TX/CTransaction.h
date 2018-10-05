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
#ifndef CXX_BT_CTRANSACTION_H
#define CXX_BT_CTRANSACTION_H
#include "script.h"
#include "BlockEngine.h"
#include "TX/CTxIn.h"
#include "TX/CTxOut.h"
#include "Block/CDiskTxPos.h"

class CTxDB;
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

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->m_nCurVersion);
        nVersion = this->m_nCurVersion;
        READWRITE(m_vTxIn);
        READWRITE(m_vTxOut);
        READWRITE(m_nLockTime);
    )

    void SetNull()
    {
        m_nCurVersion = 1;
        m_vTxIn.clear();
        m_vTxOut.clear();
        m_nLockTime = 0;
    }

    bool IsNull() const
    {
        return (m_vTxIn.empty() && m_vTxOut.empty());
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    // 判断是否是最终的交易
    bool IsFinal() const
    {
        // 如果锁定时间等于0或者锁定时间小于主链的长度，则说明是最终的交易
        const int& nBestHeight = BlockEngine::getInstance()->nBestHeight;
        if (m_nLockTime == 0 || m_nLockTime < nBestHeight)
            return true;
        foreach(const CTxIn& txin, m_vTxIn)
            if (!txin.IsFinal())
                return false;
        return true;
    }
	// 对比相同的交易哪一个更新点：对于相同输入的交易判断哪一个更新
    bool IsNewerThan(const CTransaction& old) const
    {
        if (m_vTxIn.size() != old.m_vTxIn.size())
            return false;
        for (int i = 0; i < m_vTxIn.size(); i++)
            if (m_vTxIn[i].m_cPrevOut != old.m_vTxIn[i].m_cPrevOut)
                return false;

        bool fNewer = false;
        unsigned int nLowest = UINT_MAX;
        for (int i = 0; i < m_vTxIn.size(); i++)
        {
            if (m_vTxIn[i].m_uSequence != old.m_vTxIn[i].m_uSequence)
            {
                if (m_vTxIn[i].m_uSequence <= nLowest)
                {
                    fNewer = false;
                    nLowest = m_vTxIn[i].m_uSequence;
                }
                if (old.m_vTxIn[i].m_uSequence < nLowest)
                {
                    fNewer = true;
                    nLowest = old.m_vTxIn[i].m_uSequence;
                }
            }
        }
        return fNewer;
    }

	// 判断当前交易是否是币基交易：币基交易的特点是交易输入大小为1，但是对应的输入的输出为空
    bool IsCoinBase() const
    {
        return (m_vTxIn.size() == 1 && m_vTxIn[0].m_cPrevOut.IsNull());
    }
	/* 对这边交易进行检查：
	(1)交易对应的输入或者输出列表都不能为空
	(2)交易对应的输出金额不能小于0
	(3)如果是币基交易，则对应的输入只能为1，且对应的输入签名大小不能大于100
	(4)如果是非币基交易，则对应的交易输入的输出不能为空
	*/
    bool CheckTransaction() const
    {
        // Basic checks that don't depend on any context
        if (m_vTxIn.empty() || m_vTxOut.empty())
            return error("CTransaction::CheckTransaction() : m_vTxIn or m_vTxOut empty");

        // Check for negative values
        foreach(const CTxOut& txout, m_vTxOut)
            if (txout.m_nValue < 0)
                return error("CTransaction::CheckTransaction() : txout.nValue negative");

        if (IsCoinBase())
        {
            if (m_vTxIn[0].m_cScriptSig.size() < 2 || m_vTxIn[0].m_cScriptSig.size() > 100)
                return error("CTransaction::CheckTransaction() : coinbase script size");
        }
        else
        {
            foreach(const CTxIn& txin, m_vTxIn)
                if (txin.m_cPrevOut.IsNull())
                    return error("CTransaction::CheckTransaction() : preout is null");
        }

        return true;
    }

	// 判断当前的交易是否包含节点本身的交易（在输出列表中）
    bool IsMine() const
    {
        foreach(const CTxOut& txout, m_vTxOut)
            if (txout.IsMine())
                return true;
        return false;
    }

	// 获得当前交易总的输入：借方
    int64 GetDebit() const
    {
        int64 nDebit = 0;
        foreach(const CTxIn& txin, m_vTxIn)
            nDebit += txin.GetDebit();
        return nDebit;
    }

	// 获得当前交易总的贷方金额：属于节点自身的
    int64 GetCredit() const
    {
        int64 nCredit = 0;
        foreach(const CTxOut& txout, m_vTxOut)
            nCredit += txout.GetCredit();
        return nCredit;
    }
	// 获取交易对应所有输出金额之和
    int64 GetValueOut() const
    {
        int64 nValueOut = 0;
        foreach(const CTxOut& txout, m_vTxOut)
        {
            if (txout.m_nValue < 0)
                throw runtime_error("CTransaction::GetValueOut() : negative value");
            nValueOut += txout.m_nValue;
        }
        return nValueOut;
    }
	// 获取交易对应的最小交易费：大小小于10000字节则对应的最小交易费为0，否则为按照大小进行计算交易费
	// Transaction fee requirements, mainly only needed for flood control
	// Under 10K (about 80 inputs) is free for first 100 transactions
	// Base rate is 0.01 per KB
    int64 GetMinFee(bool fDiscount=false) const
    {
        return 0;
        // Base fee is 1 cent per kilobyte
        unsigned int nBytes = ::GetSerializeSize(*this, SER_NETWORK);
        int64 nMinFee = (1 + (int64)nBytes / 1000) * CENT;

        // First 100 transactions in a block are free
        if (fDiscount && nBytes < 10000)
            nMinFee = 0;

        // To limit dust spam, require a 0.01 fee if any output is less than 0.01
        if (nMinFee < CENT)
            foreach(const CTxOut& txout, m_vTxOut)
                if (txout.m_nValue < CENT)
                    nMinFee = CENT;

        return nMinFee;
    }

	// 从硬盘中进行读取
    bool ReadFromDisk(const CDiskTxPos& pos, FILE** pfileRet=NULL)
    {
        CAutoFile filein = BlockEngine::getInstance()->OpenBlockFile(pos.m_nFile, 0, pfileRet ? "rb+" : "rb");
        if (!filein)
            return error("CTransaction::ReadFromDisk() : OpenBlockFile failed");

        // Read transaction
        if (fseek(filein, pos.m_nTxPos, SEEK_SET) != 0)
            return error("CTransaction::ReadFromDisk() : fseek failed");
        filein >> *this;

        // Return file pointer
        if (pfileRet)
        {
            if (fseek(filein, pos.m_nTxPos, SEEK_SET) != 0)
                return error("CTransaction::ReadFromDisk() : second fseek failed");
            *pfileRet = filein.release();
        }
        return true;
    }


    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return (a.m_nCurVersion  == b.m_nCurVersion &&
                a.m_vTxIn       == b.m_vTxIn &&
                a.m_vTxOut      == b.m_vTxOut &&
                a.m_nLockTime == b.m_nLockTime);
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return !(a == b);
    }


    string ToString() const
    {
        string str;
        str += strprintf("CTransaction(hash=%s, ver=%d, m_vTxIn.size=%d, m_vTxOut.size=%d, nLockTime=%d)\n",
            GetHash().ToString().substr(0,6).c_str(),
            m_nCurVersion,
            m_vTxIn.size(),
            m_vTxOut.size(),
            m_nLockTime);
        for (int i = 0; i < m_vTxIn.size(); i++)
            str += "    " + m_vTxIn[i].ToString() + "\n";
        for (int i = 0; i < m_vTxOut.size(); i++)
            str += "    " + m_vTxOut[i].ToString() + "\n";
        return str;
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }


	// 断开连接：释放交易对应输入的占用和将交易从对应的交易索引表中释放掉
    bool DisconnectInputs(CTxDB& txdb);
	// 交易输入链接，将对应的交易输入占用对应的交易输入的花费标记
    bool ConnectInputs(CTxDB& txdb, map<uint256, CTxIndex>& mapTestPool, const CDiskTxPos& posThisTx, int m_nCurHeight, int64& nFees, bool fBlock, bool fMiner, int64 nMinFee=0);
	// 客户端连接输入，对交易本身进行验证
	bool ClientConnectInputs();
	// 判断这笔交易是否应该被接受
    bool AcceptTransaction(CTxDB& txdb, bool fCheckInputs=true, bool* pfMissingInputs=NULL);

    bool AcceptTransaction(bool fCheckInputs=true, bool* pfMissingInputs=NULL);

protected:
	// 将当前交易增加到内存池mapTransactions,mapNextTx中，并且更新交易更新的次数
    bool AddToMemoryPool();
public:
	// 将当前交易从内存对象mapTransactions，mapNextTx中移除，并且增加交易对应的更新次数
    bool RemoveFromMemoryPool();
};



#endif /* CXX_BT_CTRANSACTION_H */
/* EOF */

