/*
 * =====================================================================================
 *
 *       Filename:  CTxInPoint.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 03:30:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CXX_BT_CTXINPOINT_H
#define CXX_BT_CTXINPOINT_H
class CTransaction;
class CInPoint
{
public:
    CTransaction* m_pcTrans; // 交易指针
    unsigned int m_nIndex; // 对应交易当前的第几个输入

    CInPoint() { SetNull(); }
    CInPoint(CTransaction* ptxIn, unsigned int nIn) { m_pcTrans = ptxIn; m_nIndex = nIn; }
    void SetNull() { m_pcTrans = NULL; m_nIndex = -1; }
    bool IsNull() const { return (m_pcTrans == NULL && m_nIndex == -1); }
};


#endif /* CXX_BT_CTXINPOINT_H */
/* EOF */

