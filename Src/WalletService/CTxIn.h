/*
 * =====================================================================================
 *
 *       Filename:  CTxIn.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 11:12:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CWalletServiceIN_H
#define EZ_BT_CWalletServiceIN_H

#include "script.h"
#include "WalletService/CTxOutPoint.h"

namespace Enze
{

//
// An input of a transaction.  It contains the location of the previous
// transaction's output that it claims and a signature that matches the
// output's public key.
//
class CTxIn
{
public:
    COutPoint m_cPrevOut; // 前一个交易对应的输出（叫一个交易对应的hash值和对应的第几个输出）
    CScript m_cScriptSig; // 输入脚本对应的签名
    unsigned int m_uSequence;// 主要是用于判断相同输入的交易哪一个更新，值越大越新

    CTxIn()
    {
        m_uSequence = UINT_MAX;
    }

    explicit CTxIn(COutPoint preoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=UINT_MAX)
    {
        m_cPrevOut = preoutIn;
        m_cScriptSig = scriptSigIn;
        m_uSequence = nSequenceIn;
    }

    CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=UINT_MAX)
    {
        m_cPrevOut = COutPoint(hashPrevTx, nOut);
        m_cScriptSig = scriptSigIn;
        m_uSequence = nSequenceIn;
    }

    // 交易对应nSequence是最大，已经是最新了，是最终的
    bool IsFinal() const
    {
        return (m_uSequence == UINT_MAX);
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.m_cPrevOut   == b.m_cPrevOut &&
                a.m_cScriptSig == b.m_cScriptSig &&
                a.m_uSequence == b.m_uSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        string str;
        str += strprintf("CTxIn(");
        str += m_cPrevOut.ToString();
        if (m_cPrevOut.IsNull())
            str += strprintf(", coinbase %s", HexStr(m_cScriptSig.begin(), m_cScriptSig.end(), false).c_str());
        else
            str += strprintf(", scriptSig=%s", m_cScriptSig.ToString().substr(0,24).c_str());
        if (m_uSequence != UINT_MAX)
            str += strprintf(", nSequence=%u", m_uSequence);
        str += ")";
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }

	// 判断当前输入的交易是否属于本节点，就是对应的脚本签名是否在本地能够找到
    bool IsMine() const;
	// 获得对应交易的借方金额，如果对应的输入是本节点的账号，则借方金额就是交易输入金额
    int64 GetDebit() const;
};

} //end namespace 
#endif /* EZ_BT_CTXIN_H */
/* EOF */

