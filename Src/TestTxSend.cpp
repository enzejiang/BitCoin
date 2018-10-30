/*
 * =====================================================================================
 *
 *       Filename:  TestTxSend.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/01/2018 10:39:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "TestTxSend.h"
#include "WalletServ.h"
#include "NetWorkService/NetWorkServ.h"
//////////////////////////////////////////////////////////////////////////////
//
// TestTxSend
//
using namespace Enze;
TestTxSend::TestTxSend(const CAddress& addrIn, int64 nPriceIn, const CWalletTx& wtxIn) // we have to give null so parent can't destroy us
{
    addr = addrIn;
    nPrice = nPriceIn;
    wtx = wtxIn;
    strStatus = "";
    fCanCancel = true;
    fAbort = false;
    fSuccess = false;
    fUIDone = false;
    fWorkDone = false;

    pthread_t pid = 0;
    pthread_create(&pid, NULL, SendingDialogStartTransfer, (void*)this);
}

TestTxSend::~TestTxSend()
{
    printf("~TestTxSend()\n");
}

void TestTxSend::Close()
{
    // Last one out turn out the lights.
    // fWorkDone signals that work side is done and UI thread should call destroy.
    // fUIDone signals that UI window has closed and work thread should call destroy.
    // This allows the window to disappear and end modality when cancelled
    // without making the user wait for ConnectNode to return.  The dialog object
    // hangs around in the background until the work thread exits.
    fUIDone = true;
}


//
// Everything from here on is not in the UI thread and must only communicate
// with the rest of the dialog through variables and calling repaint.
//


bool TestTxSend::Status()
{
    if (fUIDone)
    {
        return false;
    }
    if (fAbort && fCanCancel)
    {
        strStatus = "CANCELLED";
        fWorkDone = true;
        return false;
    }
    return true;
}

bool TestTxSend::Status(const string& str)
{
    if (!Status())
        return false;
    strStatus = str;
    return true;
}

bool TestTxSend::Error(const string& str)
{
    fCanCancel = false;
    fWorkDone = true;
    Status(string("error: ") + str);
    return false;
}

void* SendingDialogStartTransfer(void* parg)
{
    TestTxSend* pSend = (TestTxSend*)parg;
    pSend->StartTransfer();
}

void TestTxSend::StartTransfer()
{
    printf("start TestTxSend::StartTransfer\n");
    // Make sure we have enough money
    int64 nTransactionFee = WalletServ::getInstance()->nTransactionFee; 
    if (nPrice + nTransactionFee > WalletServ::getInstance()->GetBalance())
    {
        error("You don't have enough money");
        return;
    }

    // We may have connected already for product details
    if (!Status("Connecting..."))
        return;
    ZNode* pnode = Enze::NetWorkServ::getInstance()->ConnectNode(addr, 5 * 60);
    if (!pnode)
    {
        error("Unable to connect");
        return;
    }
     printf("start TestTxSend::StartTransfer--push checkorder\n");
    // Send order to seller, with response going to OnReply2 via event handler
    if (!Status("Requesting public key..."))
        return;
 //   pnode->PushRequest("checkorder", wtx, SendingDialogOnReply2, this);
}

void SendingDialogOnReply2(void* parg, CDataStream& vRecv)
{
   ((TestTxSend*)parg)->OnReply2(vRecv);
}

void TestTxSend::OnReply2(CDataStream& vRecv)
{
    if (!Status("Received public key..."))
        return;

    CScript scriptPubKey;
    int nRet;
    try
    {
        vRecv >> nRet;
        if (nRet > 0)
        {
            string strMessage;
            vRecv >> strMessage;
            error("Transfer was not accepted");
            //// todo: enlarge the window and enable a hidden white box to put seller's message
            return;
        }
        vRecv >> scriptPubKey;
    }
    catch (...)
    {
        //// what do we want to do about this?
        error("Invalid response received");
        return;
    }

    // Should already be connected
    ZNode* pnode = Enze::NetWorkServ::getInstance()->ConnectNode(addr, 5 * 60);
    if (!pnode)
    {
        error("Lost connection");
        return;
    }

    // Pause to give the user a chance to cancel

//    CRITICAL_BLOCK(cs_main)
    {
        // Pay
        if (!Status("Creating transaction..."))
            return;
        int64 nTransactionFee = WalletServ::getInstance()->nTransactionFee; 
        
        if (nPrice + nTransactionFee > WalletServ::getInstance()->GetBalance())
        {
            error("You don't have enough money");
            return;
        }
        int64 nFeeRequired;
        if (!WalletServ::getInstance()->CreateTransaction(scriptPubKey, nPrice, wtx, nFeeRequired))
        {
            if (nPrice + nFeeRequired > WalletServ::getInstance()->GetBalance())
                printf("This is an oversized transaction that requires a transaction fee of %s", FormatMoney(nFeeRequired).c_str());
            else
                error("TestTxSend::OnReply2--Transaction creation failed");
            return;
        }

        // Commit
        if (!WalletServ::getInstance()->CommitTransactionSpent(wtx))
        {
            error("error finalizing payment");
            return;
        }

        // Send payment tx to seller, with response going to OnReply3 via event handler
//        pnode->PushRequest("submitorder", wtx, SendingDialogOnReply3, this);

        // Accept and broadcast transaction
        if (!wtx.AcceptTransaction())
            printf("ERROR: TestTxSend : wtxNew.AcceptTransaction() %s failed\n", wtx.GetHash().ToString().c_str());
        wtx.RelayWalletTransaction();

    }
}

void SendingDialogOnReply3(void* parg, CDataStream& vRecv)
{
    ((TestTxSend*)parg)->OnReply3(vRecv);
}

void TestTxSend::OnReply3(CDataStream& vRecv)
{
    int nRet;
    try
    {
        vRecv >> nRet;
        if (nRet > 0)
        {
            error("The payment was sent, but the recipient was unable to verify it.\n"
                  "The transaction is recorded and will credit to the recipient if it is valid,\n"
                  "but without comment information.");
            return;
        }
    }
    catch (...)
    {
        //// what do we want to do about this?
        error("Payment was sent, but an invalid response was received");
        return;
    }

    fSuccess = true;
    fWorkDone = true;
    Status("Payment completed");
}


/* EOF */

