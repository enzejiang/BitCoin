/*
 * =====================================================================================
 *
 *       Filename:  CTxOutPoint.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 03:28:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CTXOUTPOINT_H
#define EZ_BT_CTXOUTPOINT_H
#include "headers.h"
#include "CommonBase/uint256.h"
namespace Enze 
{

class COutPoint
{
public:
    uint256 m_u256Hash; // 交易对应的hash
    unsigned int m_nIndex; // 交易对应的第几个输出

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { m_u256Hash = hashIn; m_nIndex = nIn; }
    void SetNull() { m_u256Hash = 0; m_nIndex = -1; }
    bool IsNull() const { return (m_u256Hash == 0 && m_nIndex == -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.m_u256Hash < b.m_u256Hash || (a.m_u256Hash == b.m_u256Hash && a.m_nIndex < b.m_nIndex));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.m_u256Hash == b.m_u256Hash && a.m_nIndex == b.m_nIndex);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        return strprintf("COutPoint(%s, %d)", m_u256Hash.ToString().substr(0,6).c_str(), m_nIndex);
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};



}// end namespace
#endif /* EZ_BT_CTXOUTPOINT_H */
/* EOF */

