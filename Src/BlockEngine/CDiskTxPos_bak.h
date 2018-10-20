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

#include "ProtocSrc/DiskTxPos.pb.h"

namespace Enze
{

class CDiskTxPos: public DiskTxPos
{
public:
#if 0
    unsigned int m_nFile; // 块所在文件的信息，而且块文件的命名一般是blk${nFile}.dat
    unsigned int m_nBlockPos; // 当前块在对应块文件中的偏移
    unsigned int m_nTxPos; // 交易在对应块中的偏移
#endif 
    CDiskTxPos();

    CDiskTxPos(unsigned int nFileIn, unsigned int nBlockPosIn, unsigned int nTxPosIn);

    void SetNull();
    bool IsNull() const;

    string ToString() const;
    void print() const;
    
    friend bool operator==(const CDiskTxPos& a, const CDiskTxPos& b);
    friend bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b);

};


}
#endif /* EZ_CDISKTXPOS_H */ 
/* EOF */

