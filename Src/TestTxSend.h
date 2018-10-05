/*
 * =====================================================================================
 *
 *       Filename:  TestTxSend.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/01/2018 10:32:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_BT_TEST_TX_SEND_H
#define CXX_BT_TEST_TX_SEND_H

#include "headers.h"
#include "Network/CAddress.h"
#include "TX/CWalletTx.h"
class CDataStream;
class TestTxSend
{
	
public:
    /** Constructor */
    TestTxSend(const CAddress& addrIn, int64 nPriceIn, const CWalletTx& wtxIn);
    ~TestTxSend();

    // State
    CAddress addr;
    int64 nPrice;
    CWalletTx wtx;
    string strStatus;
    bool fCanCancel;
    bool fAbort;
    bool fSuccess;
    bool fUIDone;
    bool fWorkDone;

    void Close();
    bool Status();
    bool Status(const string& str);
    bool Error(const string& str);
    void StartTransfer();
    void OnReply2(CDataStream& vRecv);
    void OnReply3(CDataStream& vRecv);
};

void* SendingDialogStartTransfer(void* parg);
void SendingDialogOnReply2(void* parg, CDataStream& vRecv);
void SendingDialogOnReply3(void* parg, CDataStream& vRecv);


#endif /* CXX_BT_SENDMONEY_H */
/* EOF */

