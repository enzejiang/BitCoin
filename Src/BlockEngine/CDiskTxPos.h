/*
 * =====================================================================================
 *
 *       Filename:  CDiskTxPos.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 05:39:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EZ_CDISKTXPOS_H
#define EZ_CDISKTXPOS_H

namespace Enze
{


class CDiskTxPos
{
public:
    unsigned int m_nFile; // 块所在文件的信息，而且块文件的命名一般是blk${nFile}.dat
    unsigned int m_nBlockPos; // 当前块在对应块文件中的偏移
    unsigned int m_nTxPos; // 交易在对应块中的偏移

    CDiskTxPos()
    {
        SetNull();
    }

    CDiskTxPos(unsigned int nFileIn, unsigned int nBlockPosIn, unsigned int nTxPosIn)
    {
        m_nFile = nFileIn;
        m_nBlockPos = nBlockPosIn;
        m_nTxPos = nTxPosIn;
    }

    void SetNull() { m_nFile = -1; m_nBlockPos = 0; m_nTxPos = 0; }
    bool IsNull() const { return (m_nFile == -1); }

    friend bool operator==(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return (a.m_nFile     == b.m_nFile &&
                a.m_nBlockPos == b.m_nBlockPos &&
                a.m_nTxPos    == b.m_nTxPos);
    }

    friend bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        if (IsNull())
            return strprintf("null");
        else
            return strprintf("(m_nFile=%d, m_nBlockPos=%d, m_nTxPos=%d)", m_nFile, m_nBlockPos, m_nTxPos);
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }
};


} //end namespace
#endif /* EZ_CDISKTXPOS_H */ 
/* EOF */

