/*
 * =====================================================================================
 *
 *       Filename:  CTxIn.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 03:17:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "util.h"
#include "WalletService/CTxIn.h"
#include "WalletService/CWalletTx.h"
#include "WalletService/WalletServ.h"

using namespace Enze;
// 判断当前交易是是否对应本节点的交易
bool CTxIn::IsMine() const
{
    map<uint256, CWalletTx>& mapWallet = WalletServ::getInstance()->mapWallet;
    {
        map<uint256, CWalletTx>::iterator mi = mapWallet.find(m_cPrevOut.m_u256Hash);
        if (mi != mapWallet.end())
        {
            const CWalletTx& prev = (*mi).second;
            if (m_cPrevOut.m_nIndex < prev.m_vTxOut.size())
                if (prev.m_vTxOut[m_cPrevOut.m_nIndex].IsMine())
                    return true;
        }
    }
    return false;
}

// 获取当前节点对于此笔交易对应的输入金额，如果输入对应的不是当前节点则对应的借方金额为0
int64 CTxIn::GetDebit() const
{
    map<uint256, CWalletTx>& mapWallet = WalletServ::getInstance()->mapWallet;
    {
        map<uint256, CWalletTx>::iterator mi = mapWallet.find(m_cPrevOut.m_u256Hash);
        if (mi != mapWallet.end())
        {
            const CWalletTx& prev = (*mi).second;
            if (m_cPrevOut.m_nIndex < prev.m_vTxOut.size())
                if (prev.m_vTxOut[m_cPrevOut.m_nIndex].IsMine())
                    return prev.m_vTxOut[m_cPrevOut.m_nIndex].m_nValue;
        }
    }
    return 0;
}


/* EOF */

