/*
 * =====================================================================================
 *
 *       Filename:  ui.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/22/2018 10:39:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "headers.h"
#include "ui.h"
#include "TestTxSend.h"
map<string, string> mapAddressBook; // 地址和名称的映射，其中key为地址，value为名称
bool fRandSendTest = false;
void SendMoneyByIPAddr(const string& strAddress);
void SendMoneyByBitAddr(const string& strAddress);
int ThreadRandSendTest(const string & strAddress);
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
    map<string, string> mapArgs = ParseParameters(argc, argv);

    if (mapArgs.count("/datadir"))
        strSetDataDir = mapArgs["/datadir"];

    if (mapArgs.count("/proxy"))
        addrProxy = CAddress(mapArgs["/proxy"].c_str());

    if (mapArgs.count("/debug"))
        fDebug = true;

    if (mapArgs.count("/dropmessages"))
    {
        nDropMessagesTest = atoi(mapArgs["/dropmessages"]);
        if (nDropMessagesTest == 0)
            nDropMessagesTest = 20;
    }

    if (mapArgs.count("/loadblockindextest"))
    {
        CTxDB txdb("r");
        txdb.LoadBlockIndex();
        PrintBlockTree();
        exit(0);
    }

    //
    // Load data files
    //
    string strErrors;
    int64 nStart, nEnd;

    printf("Loading addresses...\n");
    //QueryPerformanceCounter((LARGE_INTEGER*)&nStart);
    if (!LoadAddresses())
        strErrors += "Error loading addr.dat      \n";
    //QueryPerformanceCounter((LARGE_INTEGER*)&nEnd);
    printf(" addresses   %20I64d\n", nEnd - nStart);

    printf("Loading block index...\n");
    //QueryPerformanceCounter((LARGE_INTEGER*)&nStart);
    if (!LoadBlockIndex())
        strErrors += "Error loading blkindex.dat      \n";
    //QueryPerformanceCounter((LARGE_INTEGER*)&nEnd);
    printf(" block index %20I64d\n", nEnd - nStart);

    printf("Loading wallet...\n");
    //QueryPerformanceCounter((LARGE_INTEGER*)&nStart);
    if (!LoadWallet())
        strErrors += "Error loading wallet.dat      \n";
    //QueryPerformanceCounter((LARGE_INTEGER*)&nEnd);
    printf(" wallet      %20I64d\n", nEnd - nStart);

    printf("Done loading\n");

        //// debug print
        printf("mapBlockIndex.size() = %d\n",   mapBlockIndex.size());
        printf("nBestHeight = %d\n",            nBestHeight);
        printf("mapKeys.size() = %d\n",         mapKeys.size());
        printf("mapPubKeys.size() = %d\n",      mapPubKeys.size());
        printf("mapWallet.size() = %d\n",       mapWallet.size());
        printf("mapAddressBook.size() = %d\n",  mapAddressBook.size());
        map<string, string>::iterator it = mapAddressBook.begin();
        for (;it != mapAddressBook.end(); ++it) {
            printf("mapAddressBook, key[%s]--value[%s]\n", it->first.c_str(), it->second.c_str());
        
        }

    if (!strErrors.empty())
    {
        //wxMessageBox(strErrors, "Bitcoin");
        //OnExit();
        return false;
    }

	// 将不在块中的钱包交易放入到对应的内存交易对象mapTransactions中
    // Add wallet transactions that aren't already in a block to mapTransactions
    ReacceptWalletTransactions();

    //
    // Parameters
    //
    if (mapArgs.count("/printblockindex") || mapArgs.count("/printblocktree"))
    {
        PrintBlockTree();
        //OnExit();
        return false;
    }

    if (mapArgs.count("/gen"))
    {
        if (mapArgs["/gen"].empty())
            fGenerateBitcoins = true;
        else
            fGenerateBitcoins = atoi(mapArgs["/gen"].c_str());
    }

    //
    // Create the main frame window
    //
    {
        //pframeMain = new CMainFrame(NULL);
        //pframeMain->Show();

        if (!CheckDiskSpace())
        {
            //OnExit();
            return false;
        }
	// 启动对应节点的链接（接收消息和发送消息）
        if (!StartNode(strErrors));
            //wxMessageBox(strErrors, "Bitcoin");
#if 1
        if (fGenerateBitcoins) //节点进行挖矿
        {

            pthread_t pid = 0;
            if (pthread_create(&pid, NULL, ThreadBitcoinMiner,  NULL) == -1)
                printf("Error: pthread_create(ThreadBitcoinMiner) failed\n");
            
            gThreadList.push_back(pid);

        }
#endif
        sleep(20);
        if (argc = 2)
        {
            string addr = argv[1];
            ThreadRandSendTest(addr);
        }

        //
        // Tests
        //
        if (argc >= 2 && strcasecmp(argv[1], "/send") == 0)
        {
            int64 nValue = 1;
            if (argc >= 3)
                ParseMoney(argv[2], nValue);

            string strAddress;
            if (argc >= 4)
                strAddress = argv[3];
            CAddress addr(strAddress.c_str());

            CWalletTx wtx;
            wtx.m_mapValue["to"] = strAddress;
            wtx.m_mapValue["from"] = addrLocalHost.ToString();
            wtx.m_mapValue["message"] = "command line send";

            // Send to IP address
        }

        if (mapArgs.count("/randsendtest"))
        {
            pthread_t pid = 0;
            if (!mapArgs["/randsendtest"].empty()) // 随机发送测试，开启线程进行处理
                pthread_create(&pid, NULL, ThreadRandSendTest, (void*) new string(mapArgs["/randsendtest"]));
            else
                fRandSendTest = true;
            fDebug = true;
        }
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
    printf("ThreadRandSendTest: Bitcoin address '%s' start \n", strAddress.c_str());
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

    // Message
    CWalletTx wtx;
    wtx.m_mapValue["to"] = strAddress;
    wtx.m_mapValue["from"] = addrLocalHost.ToString();
    wtx.m_mapValue["message"] = "Enze's First Tx";
    int64 nValue = 0;//(GetRand(9) + 1) * 100 * CENT;

    TestTxSend* pSend = new TestTxSend(addr, nValue, wtx);
    
    if (!mapAddressBook.count(strAddress))
        SetAddressBookName(strAddress, "");

    // Make sure we have enough money
    if (nValue + nTransactionFee > GetBalance())
    {
        error("You don't have enough money");
        return;
    }
    CNode* pnode = ConnectNode(addr, 5 * 60);
    if (!pnode)
    {
        error("Unable to connect");
        return;
    }
    //pnode->PushRequest("checkorder", wtx, SendingDialogOnReply2, NULL);

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
        if (GetBalance() < nValue)
        {
            //wxMessageBox("Out of money  ");
            printf("Out of money, SendValue[%d]--Blance[%d]\n", nValue, GetBalance());
            return ;
        }
        nValue += (nRep % 100) * CENT;

        // Send to bitcoin address
        CScript scriptPubKey;
        scriptPubKey << OP_DUP << OP_HASH160 << hash160 << OP_EQUALVERIFY << OP_CHECKSIG;

        if (!SendMoney(scriptPubKey, nValue, wtx))
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

