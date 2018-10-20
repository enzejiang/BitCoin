/*
 * =====================================================================================
 *
 *       Filename:  CDiskTxPos.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/07/2018 07:50:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "CDiskTxPos.h"

namespace Enze
{

    CDiskTxPos::CDiskTxPos()
        :DiskTxPos()
    {
        SetNull();
    }

    CDiskTxPos::CDiskTxPos(unsigned int nFileIn, unsigned int nBlockPosIn, unsigned int nTxPosIn)
        :DiskTxPos()
    {
        set_nfile(nFileIn);
        set_nblockpos(nBlockPosIn);
        set_mtxpos(nTxPosIn);
    }

    void CDiskTxPos::SetNull()
    {
        clear();
        set_nfile(-1);
    }
    
    bool CDiskTxPos::IsNull() const
    { 
        return (-1 == nfile());
    }


    string CDiskTxPos::ToString() const
    {
        if (IsNull())
            return strprintf("null");
        else
            return strprintf("(nfile()=%d, nblockpos()=%d, ntxpos()=%d)", nfile(), nblockPos(), ntxpos());
    }

    void CDiskTxPos::print() const
    {
        printf("%s", ToString().c_str());
    }
    
    bool operator==(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return (a.nfile()     == b.nfile() &&
                a.nblockpos() == b.nblockpos() &&
                a.ntxpos()    == b.ntxpos());
    }

    bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return !(a == b);
    }
};
/* EOF */

