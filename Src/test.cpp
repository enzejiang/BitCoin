/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/03/2018 11:44:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "CommonBase/headers.h"
#include "CommonBase/BlockEngine.h"
#include "CommonBase/base58.h"
#include "CommonBase/market.h"
#include "Network/net.h"
#include "Db/db.h"
#include "TestTxSend.h"
#include <iostream>
using namespace std;
bool fRandSendTest = false;
void SendMoneyByIPAddr(const string& strAddress);
void SendMoneyByBitAddr(const string& strAddress);
int ThreadRandSendTest(const string & strAddress);
bool OnInit(int argc, char* argv[]);
vector<pthread_t> gThreadList;

extern vector<pthread_t> gThreadList;

int main(int argc, char* argv[])
{
    uint256 test = ~uint256(0)>>12;
    printf("uint256[%s]\n", test.ToString().c_str());
    return OnInit(argc, argv);
}

map<string, string> ParseParameters(int argc, char* argv[])
{
    map<string, string> mapArgs;
    for (int i = 0; i < argc; i++)
    {
        char psz[10000];
        strcpy(psz, argv[i]);
        char* pszValue = "";
        if (strchr(psz, '='))
        {
            pszValue = strchr(psz, '=');
            *pszValue++ = '\0';
        }
        strlwr(psz);
        if (psz[0] == '-')
            psz[0] = '/';
        mapArgs[psz] = pszValue;
    }
    return mapArgs;
}

bool OnInit(int argc, char* argv[])
{


    BlockEngine::getInstance()->initiation();
	// 将不在块中的钱包交易放入到对应的内存交易对象mapTransactions中
    // Add wallet transactions that aren't already in a block to mapTransactions
    BlockEngine::getInstance()->ReacceptWalletTransactions();


    //
    // Create the main frame window
    //

    if (!BlockEngine::getInstance()->CheckDiskSpace())
    {
        //OnExit();
        return false;
    }
    
    string strErrors;
	// 启动对应节点的链接（接收消息和发送消息）
    if (StartNode(strErrors));
        printf("Start Bitcoin Node\n");
    if (BlockEngine::getInstance()->fGenerateBitcoins) //节点进行挖矿
    {

        pthread_t pid = 0;
        if (pthread_create(&pid, NULL, ThreadBitcoinMiner,  NULL) == -1)
            printf("Error: pthread_create(ThreadBitcoinMiner) failed\n");
        
    
        gThreadList.push_back(pid);

    }
    sleep(30);
    printf("mapWallet.size() = %d\n", BlockEngine::getInstance()->mapWallet.size());
    
    if (argc == 2) {
        ThreadRandSendTest(argv[1]);
    }

    int* ret = 0;
    foreach(pthread_t pid, gThreadList) {
        pthread_join(pid, (void**)&ret);
    }

    return true;
}
#if 1
// randsendtest to bitcoin address
void* ThreadRandSendTest(void* parg)
{
    string strAddress = *(string*)parg;
    ThreadRandSendTest(strAddress);
}
#endif
int ThreadRandSendTest(const string & strAddress)
{
    printf("ThreadRandSendTest: Bitcoin address '%s' start mapWallet.size() %d \n", strAddress.c_str(), BlockEngine::getInstance()->mapWallet.size());
    uint160 hash160;
    if (!AddressToHash160(strAddress, hash160))
    {
        SendMoneyByIPAddr(strAddress);
    }
    else {
        SendMoneyByBitAddr(strAddress);
    }
    printf("send test--endl\n");
    return 0;
   // sleep(1);
}


void SendMoneyByIPAddr(const string& strAddress)
{
    // Parse IP address
    CAddress addr(strAddress.c_str());
    if (addr.ip == 0)
    {
        printf("Invalid address[%s]--to Send Coins\n",strAddress.c_str());
        return;
    }
    BlockEngine::getInstance()->mapWallet.count(18);
    //CRITICAL_BLOCK(cs_mapWallet)
    // Message
    CWalletTx wtx;
    wtx.m_mapValue["to"] = strAddress;
    wtx.m_mapValue["from"] = addrLocalHost.ToString();
    wtx.m_mapValue["message"] = "Enze's First Tx";
    int64 nValue =  100 * CENT;
    printf("SendMoneyByIPAddr WalletSize[%d]\n", BlockEngine::getInstance()->mapWallet.size());
    TestTxSend* pSend = new TestTxSend(addr, nValue, wtx);
    
    map<string, string>& mapAddressBook = BlockEngine::getInstance()->mapAddressBook;
    if (!mapAddressBook.count(strAddress))
        SetAddressBookName(strAddress, "");


}

void SendMoneyByBitAddr(const string& strAddress) 
{
    uint160 hash160;
    if (!AddressToHash160(strAddress, hash160))
        return;
    
    loop
    {
        usleep(GetRand(30) * 1000 + 100);

        // Message
        CWalletTx wtx;
        wtx.m_mapValue["to"] = strAddress;
        wtx.m_mapValue["from"] = addrLocalHost.ToString();
        static int nRep;
        wtx.m_mapValue["message"] = strprintf("randsendtest %d\n", ++nRep);

        // Value
        int64 nValue = (GetRand(9) + 1) * 100 * CENT;
        if (BlockEngine::getInstance()->GetBalance() < nValue)
        {
            //wxMessageBox("Out of money  ");
            printf("Out of money, SendValue[%d]--Blance[%d]\n", nValue, BlockEngine::getInstance()->GetBalance());
            return ;
        }
        nValue += (nRep % 100) * CENT;

        // Send to bitcoin address
        CScript scriptPubKey;
        scriptPubKey << OP_DUP << OP_HASH160 << hash160 << OP_EQUALVERIFY << OP_CHECKSIG;

        if (!BlockEngine::getInstance()->SendMoney(scriptPubKey, nValue, wtx))
            return;
    }

}

void* ThreadRequestProductDetails(void* parg)
{
    CProduct product;
#if 0
    // Extract parameters
    pair<CProduct, wxEvtHandler*>* pitem = (pair<CProduct, wxEvtHandler*>*)parg;
    CProduct product = pitem->first;
    wxEvtHandler* pevthandler = pitem->second;
    delete pitem;

    // Connect to seller
    CNode* pnode = ConnectNode(product.addr, 5 * 60);
    if (!pnode)
    {
        CDataStream ssEmpty;
        AddPendingReplyEvent1(pevthandler, ssEmpty);
        return;
    }

    // Request detailed product, with response going to OnReply1 via dialog's event handler
    pnode->PushRequest("getdetails", product.GetHash(), AddPendingReplyEvent1, (void*)pevthandler);
#endif
}
/* EOF */

