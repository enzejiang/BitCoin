/*
 * =====================================================================================
 *
 *       Filename:  NetWorkServ.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/06/2018 10:54:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <boost/algorithm/string.hpp>
#include "headers.h"
#include "market.h"
#include "sha.h"
#include "DAO/DaoServ.h"
#include "WalletService/CWalletTx.h"
#include "WalletService/WalletServ.h"
#include "BlockEngine/CBlock.h"
#include "BlockEngine/CBlockIndex.h"
#include "BlockEngine/CBlockLocator.h"
#include "BlockEngine/BlockEngine.h"
#include "NetWorkServ.h"
using namespace Enze;
NetWorkServ* NetWorkServ::m_pInstance = NULL;
pthread_mutex_t NetWorkServ:: m_NWSLock;
bool fClient = false;
NetWorkServ* NetWorkServ::getInstance()
{
    if (NULL == m_pInstance) 
    {
        pthread_mutex_lock(&m_NWSLock);
        if (NULL == m_pInstance)
        {
            m_pInstance = new NetWorkServ();
        }
        pthread_mutex_unlock(&m_NWSLock);
        
    }
    
    return m_pInstance;
}

void NetWorkServ::Destory()
{
        pthread_mutex_lock(&m_NWSLock);
        if (NULL == m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
        pthread_mutex_unlock(&m_NWSLock);
    
}

void NetWorkServ::initiation()
{
    string strErrors;
    printf("Loading addresses...\n");
    if (!LoadAddresses())
        strErrors += "Error loading addr.dat      \n";

}


//socket 连接，根据地址信息创建对应的socket信息
bool NetWorkServ::ConnectSocket(const CAddress& addrConnect, SOCKET& hSocketRet)
{
    hSocketRet = INVALID_SOCKET;

    SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET)
        return false;
	// 判断是否能够路由：条件是ip地址对应的是10.x.x.x或者对应的是192.168.x.x都不能进行路由
    bool fRoutable = !(addrConnect.GetByte(3) == 10 || (addrConnect.GetByte(3) == 192 && addrConnect.GetByte(2) == 168));
    // 代理标记：不能够路由肯定不能够代理，能够路由还要看对应的ip地址不能是全0
	bool fProxy = (addrProxy.ip && fRoutable);
	// 能够代理就使用代理地址，不能代理就是连接地址
    struct sockaddr_in sockaddr = (fProxy ? addrProxy.GetSockAddr() : addrConnect.GetSockAddr());
	// 对应指定的地址进行socket连接，并返回对应的socket句柄
    if (connect(hSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        close(hSocket);
        return false;
    }
	// 如果能够代理，则上面连接地址返回的socket句柄就是代理地址对应的句柄，不是连接地址对应的句柄
    if (fProxy)
    {
        printf("Proxy connecting to %s\n", addrConnect.ToString().c_str());
        char pszSocks4IP[] = "\4\1\0\0\0\0\0\0user";
        memcpy(pszSocks4IP + 2, &addrConnect.port, 2); // 设置pszSocks4IP为"\4\1port\0\0\0\0\0user"
        memcpy(pszSocks4IP + 4, &addrConnect.ip, 4);// 设置pszSocks4IP为"\4\1port+ip\0user"
        char* pszSocks4 = pszSocks4IP;
        int nSize = sizeof(pszSocks4IP);

        int ret = send(hSocket, pszSocks4, nSize, 0);
        if (ret != nSize)
        {
            close(hSocket);
            return error("Error sending to proxy\n");
        }
        char pchRet[8];
        if (recv(hSocket, pchRet, 8, 0) != 8)
        {
            close(hSocket);
            return error("Error reading proxy response\n");
        }
        if (pchRet[1] != 0x5a)
        {
            close(hSocket);
            return error("Proxy returned error %d\n", pchRet[1]);
        }
        printf("Proxy connection established %s\n", addrConnect.ToString().c_str());
    }

    hSocketRet = hSocket;
    return true;
}



bool NetWorkServ::GetMyExternalIP2(const CAddress& addrConnect, const char* pszGet, const char* pszKeyword, unsigned int& ipRet)
{
    SOCKET hSocket;
    if (!ConnectSocket(addrConnect, hSocket))
        return error("GetMyExternalIP() : connection to %s failed\n", addrConnect.ToString().c_str());

    send(hSocket, pszGet, strlen(pszGet), 0);

    char Buf[512] = {0};
    int len = recv(hSocket, Buf, 512, 0);
    if (len > 0)
    {
        //printf("GetMyExternalIP() received %s\n", Buf);
        char BufIP[32] = {0};
        sscanf(strstr(Buf,"utf-8")+9,"%*[^\n]\n%[^\n]",BufIP);
       // printf("GetMyExternalIP() received IP %s\n", BufIP);
        CAddress addr(BufIP);
        if (addr.ip == 0 || !addr.IsRoutable())
            return false;
        ipRet = addr.ip;
        return true;
    }
    else {
        close(hSocket);
        return false;
    }
    close(hSocket);
    return error("GetMyExternalIP() : connection closed\n");
}


bool NetWorkServ::GetMyExternalIP(unsigned int& ipRet)
{
    return false;
    CAddress addrConnect;
    char* pszGet;
    char* pszKeyword;

    for (int nLookup = 0; nLookup <= 1; nLookup++)
    for (int nHost = 1; nHost <= 2; nHost++)
    {
        if (nHost == 1)
        {
            addrConnect = CAddress("118.184.176.13:80"); // www.3322.org

            if (nLookup == 1)
            {
                struct hostent* phostent = gethostbyname("www.3322.org");
                if (phostent && phostent->h_addr_list && phostent->h_addr_list[0])
                    addrConnect = CAddress(*(u_long*)phostent->h_addr_list[0], htons(80));
            }

            pszGet = "GET /dyndns/getip HTTP/1.1\r\n"
                     "Host:www.3322.org\r\n"
                     "ACCEPT:*/*\r\n"
                     "Connection: close\r\n"
                     "\r\n";

            pszKeyword = "IP:";
        }
        else if (nHost == 2)
        {
            addrConnect = CAddress("162.88.100.200:80"); // checkip.dyndns.org

            if (nLookup == 1)
            {
                struct hostent* phostent = gethostbyname("checkip.dyndns.org");
                if (phostent && phostent->h_addr_list && phostent->h_addr_list[0])
                    addrConnect = CAddress(*(u_long*)phostent->h_addr_list[0], htons(80));
            }

            pszGet = "GET / HTTP/1.1\r\n"
                     "Host: checkip.dyndns.org\r\n"
                     "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)\r\n"
                     "Connection: close\r\n"
                     "\r\n";

            pszKeyword = "Address:";
        }

        if (GetMyExternalIP2(addrConnect, pszGet, pszKeyword, ipRet))
            return true;
    }

    return false;
}





bool NetWorkServ::AddUserProviedAddress()
{
    // Load user provided addresses
    CAutoFile filein = fopen("addr.txt", "a+");
    if (filein)
    {
        try
        {
            char psz[1000];
            while (fgets(psz, sizeof(psz), filein))
            {
                CAddress addr(psz, NODE_NETWORK);
                // 地址不能路由，则不将此地址加入地址库中
                if (!addr.IsRoutable())
                    continue;
                if (addr.ip == m_cAddrLocalHost.ip)
                    continue;
                
                // 根据地址的ip+port来查询对应的内存对象m_cMapAddresses，
                map<string, CAddress>::iterator it = m_cMapAddresses.find(addr.GetKey());
                if (it == m_cMapAddresses.end())
                {
                    printf("NetWorkServ::AddUserProviedAddress--add-into db\n");
                    // New address
                    m_cMapAddresses.insert(make_pair(addr.GetKey(), addr));
                    DaoServ::getInstance()->WriteAddress(addr);
                    continue;
                }
                else
                {
                    CAddress& addrFound = (*it).second;
                    if ((addrFound.m_nServices | addr.m_nServices) != addrFound.m_nServices)
                    {
                          printf("NetWorkServ::AddUserProviedAddress--update-into db\n");
                        // 增加地址对应的服务类型，并将其写入数据库
                        // Services have been added
                        addrFound.m_nServices |= addr.m_nServices;
                        DaoServ::getInstance()->WriteAddress(addrFound);
                    }
                    continue;
                }
            }
        }
        catch (...) { }
    }
    return true;
}

// 根据ip在本地存储的节点列表m_cNodeLst中查找对应的节点
CNode* NetWorkServ::FindNode(unsigned int ip)
{
 //   //CRITICAL_BLOCK(cs_m_cNodeLst)
    {
        foreach(CNode* pnode, m_cNodeLst)
            if (pnode->addr.ip == ip)
                return (pnode);
    }
    return NULL;
}

CNode* NetWorkServ::FindNode(const CAddress& addr)
{
    //CRITICAL_BLOCK(cs_m_cNodeLst)
    {
        foreach(CNode* pnode, m_cNodeLst)
            if (pnode->addr == addr)
                return (pnode);
    }
    return NULL;
}
// 链接对应地址的节点
CNode* NetWorkServ::ConnectNode(const CAddress& addrConnect, int64 nTimeout)
{
    if (addrConnect.ip == m_cAddrLocalHost.ip)
        return NULL;

	// 使用ip地址在本地对应的节点列表中查找对应的节点，如果存在则直接返回对应的节点
    // Look for an existing connection
    CNode* pnode = FindNode(addrConnect.ip);
    if (pnode)
    {
        if (nTimeout != 0)
            pnode->AddRef(nTimeout); // 推迟节点对应的释放时间
        else
            pnode->AddRef(); // 增加节点对应的引用
        return pnode;
    }

    /// debug print
 //   printf("trying %s\n", addrConnect.ToString().c_str());

	// 对请求的地址进行连接
    // Connect
    SOCKET hSocket;
    if (ConnectSocket(addrConnect, hSocket))
    {
        /// debug print
        printf("connected %s\n", addrConnect.ToString().c_str());

        // Set to nonblocking
        u_long nOne = 1;
#if 0
        if (ioctlsocket(hSocket, FIONBIO, &nOne) == SOCKET_ERROR)
            printf("ConnectSocket() : ioctlsocket nonblocking setting failed, error %m\n");
#endif
        int flag = 0;
        flag = fcntl(hSocket, F_GETFL, NULL);
        fcntl(hSocket, F_SETFL, flag|O_NONBLOCK);
        // Add node
        CNode* pnode = new CNode(hSocket, addrConnect, false);
        if (nTimeout != 0)
            pnode->AddRef(nTimeout);
        else
            pnode->AddRef();
   //     //CRITICAL_BLOCK(cs_m_cNodeLst)
            m_cNodeLst.push_back(pnode);

     //   //CRITICAL_BLOCK(cs_m_cMapAddresses)
            m_cMapAddresses[addrConnect.GetKey()].nLastFailed = 0;
        return pnode;
    }
    else
    {
        m_cMapAddresses[addrConnect.GetKey()].nLastFailed = GetTime();
        return NULL;
    }
}

// 处理单个节点对应的消息：单个节点接收到的消息进行处理
bool NetWorkServ::ProcessMessages(CNode* pfrom)
{
    printf("error %s_%d\n", __FILE__, __LINE__);
    //CDataStream& vRecv pfrom->vRecv;
    CDataStream vRecv;// = pfrom->vRecv;
    if (vRecv.empty())
        return true;
    printf("ProcessMessages(%d bytes)\n", vRecv.size());

    // 同一个的消息格式
    // Message format
    //  (4) message start
    //  (12) command
    //  (4) size
    //  (x) data
    //
	// 消息头包含：message start;command;size;

    loop
    {
        // Scan for message start
        CDataStream::iterator pstart = search(vRecv.begin(), vRecv.end(), BEGIN(pchMessageStart), END(pchMessageStart));
        // 删除无效的消息： 就是在对应的消息开始前面还有一些信息
	    if (vRecv.end() - pstart < sizeof(CMessageHeader))
        {
            if (vRecv.size() > sizeof(CMessageHeader))
            {
                printf("\n\nPROCESSMESSAGE MESSAGESTART NOT FOUND\n\n");
                vRecv.erase(vRecv.begin(), vRecv.end() - sizeof(CMessageHeader));
            }
            break;
        }
        if (pstart - vRecv.begin() > 0)
            printf("\n\nPROCESSMESSAGE SKIPPED %d BYTES\n\n", pstart - vRecv.begin());
        vRecv.erase(vRecv.begin(), pstart); // 移除消息开始信息和接收缓冲区开头之间

		// 读取消息头
        // Read header
        CMessageHeader hdr;
        vRecv >> hdr; // 指针已经偏移了
        if (!hdr.IsValid())
        {
            printf("\n\nPROCESSMESSAGE: ERRORS IN HEADER %s\n\n\n", hdr.GetCommand().c_str());
            continue;
        }
        string strCommand = hdr.GetCommand();

        // Message size
        unsigned int nMessageSize = hdr.m_nMessageSize;
        if (nMessageSize > vRecv.size())
        {
            // Rewind and wait for rest of message
            ///// need a mechanism to give up waiting for overlong message size error
            printf("MESSAGE-BREAK 2\n");
            vRecv.insert(vRecv.begin(), BEGIN(hdr), END(hdr));
            sleep(100);
            break;
        }

        // Copy message to its own buffer
        CDataStream vMsg(vRecv.begin(), vRecv.begin() + nMessageSize, vRecv.nType, vRecv.nVersion);
        vRecv.ignore(nMessageSize);

        // Process message
        bool fRet = false;
        try
        {
            fRet = ProcessMessage(pfrom, strCommand, vMsg);
        }
        CATCH_PRINT_EXCEPTION("ProcessMessage()")
        const CAddress& addrLocalHost = Enze::NetWorkServ::getInstance()->getLocakAddr();
        if (!fRet)
            printf("ProcessMessage(%s, %d bytes) from %s to %s FAILED\n", strCommand.c_str(), nMessageSize, pfrom->addr.ToString().c_str(), addrLocalHost.ToString().c_str());
    }

    vRecv.Compact();
    return true;
}



// 对节点pFrom处理命令strCommand对应的消息内容为vRecv
bool NetWorkServ::ProcessMessage(CNode* pfrom, string strCommand, CDataStream& vRecv)
{
    static map<unsigned int, vector<unsigned char> > mapReuseKey;
    printf("received: %-12s (%d bytes)  ", strCommand.c_str(), vRecv.size());
    for (int i = 0; i < min(vRecv.size(), (unsigned int)20); i++)
        printf("%02x ", vRecv[i] & 0xff);
    printf("\n");
    // 消息采集频率进行处理
    if (nDropMessagesTest > 0 && GetRand(nDropMessagesTest) == 0)
    {
        printf("dropmessages DROPPING RECV MESSAGE\n");
        return true;
    }
	// 如果命令是版本：节点对应的版本
    if (strCommand == "version")
    {
		// 节点对应的版本只能更新一次，初始为0，后面进行更新
        // Can only do this once
        if (pfrom->nVersion != 0)
            return false;

        int64 m_uTime;
        CAddress addrMe; // 读取消息对应的内容
        vRecv >> pfrom->nVersion >> pfrom->m_nServices >> m_uTime >> addrMe;
        if (pfrom->nVersion == 0)
            return false;
		// 更新发送和接收缓冲区中的对应的版本
        printf("error %s_%d\n", __FILE__, __LINE__);
        //pfrom->vSend.SetVersion(min(pfrom->nVersion, VERSION));
        //pfrom->vRecv.SetVersion(min(pfrom->nVersion, VERSION));

		// 如果节点对应的服务类型是节点网络，则对应节点的客户端标记就是false
        pfrom->fClient = !(pfrom->m_nServices & NODE_NETWORK);
        if (pfrom->fClient)
        {
            printf("error %s_%d\n", __FILE__, __LINE__);
			// 如果不是节点网络，可能仅仅是一些节点不要保存对应的完整区块信息，仅仅需要区块的头部进行校验就可以了
            //pfrom->vSend.nType |= SER_BLOCKHEADERONLY;
            //pfrom->vRecv.nType |= SER_BLOCKHEADERONLY;
        }
		// 增加时间样本数据：没有什么用处，仅仅用于输出
        AddTimeData(pfrom->addr.ip, m_uTime);

		// 对第一个进来的节点请求block信息
        // Ask the first connected node for block updates
        static bool fAskedForBlocks;
        if (!fAskedForBlocks && !pfrom->fClient)
        {
            fAskedForBlocks = true;
            pfrom->PushMessage("getblocks", CBlockLocator(BlockEngine::getInstance()->getBestIndex()), uint256(0));
        }

        printf("version message: %s has version %d, addrMe=%s\n", pfrom->addr.ToString().c_str(), pfrom->nVersion, addrMe.ToString().c_str());
    }


    else if (pfrom->nVersion == 0)
    {
		// 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        return false;
    }

	// 地址消息
    else if (strCommand == "addr")
    {
        vector<CAddress> vAddr;
        vRecv >> vAddr;

        // Store the new addresses
        CAddrDB addrdb;
        foreach(const CAddress& addr, vAddr)
        {
			// 将地址增加到数据库中
            if (DaoServ::getInstance()->WriteAddress(addr))
            {
                // Put on lists to send to other nodes
                pfrom->setAddrKnown.insert(addr); // 将对应的地址插入到已知地址集合中
                //CRITICAL_BLOCK(cs_m_cNodeLst)
                    foreach(CNode* pnode, m_cNodeLst)
                        if (!pnode->setAddrKnown.count(addr))
                            pnode->vAddrToSend.push_back(addr);// 地址的广播
            }
        }
    }

	// 库存消息
    else if (strCommand == "inv")
    {
        vector<CInv> vInv;
        vRecv >> vInv;

        foreach(const CInv& inv, vInv)
        {
            pfrom->AddInventoryKnown(inv); // 将对应的库存发送消息增加到库存发送已知中

            bool fAlreadyHave = WalletServ::getInstance()->AlreadyHave(inv);
            printf("  got inventory: %s  %s\n", inv.ToString().c_str(), fAlreadyHave ? "have" : "new");

            if (!fAlreadyHave)
                pfrom->AskFor(inv);// 如果不存在，则请求咨询，这里会在线程中发送getdata消息
            else if (inv.type == MSG_BLOCK && BlockEngine::getInstance()->mapOrphanBlocks.count(inv.hash))
                pfrom->PushMessage("getblocks", CBlockLocator(BlockEngine::getInstance()->getBestIndex()), BlockEngine::getInstance()->GetOrphanRoot(BlockEngine::getInstance()->mapOrphanBlocks[inv.hash]));
        }
    }

	// 获取数据
    else if (strCommand == "getdata")
    {
        vector<CInv> vInv;
        vRecv >> vInv;

        foreach(const CInv& inv, vInv)
        {
            printf("received getdata for: %s\n", inv.ToString().c_str());

            if (inv.type == MSG_BLOCK)
            {
                // Send block from disk
                map<uint256, CBlockIndex*>::const_iterator mi = BlockEngine::getInstance()->getMapBlockIndex().find(inv.hash);
                if (mi != BlockEngine::getInstance()->getMapBlockIndex().end())
                {
                    //// could optimize this to send header straight from blockindex for client
                    CBlock block;
                    block.ReadFromDisk((*mi).second, !pfrom->fClient);
                    pfrom->PushMessage("block", block);// 获取数据对应的类型是block，则发送对应的块信息
                }
            }
            else if (inv.IsKnownType())
            {
                // Send stream from relay memory
                //CRITICAL_BLOCK(cs_mapRelay)
                {
                    map<CInv, CDataStream>::iterator mi = mapRelay.find(inv); // 重新转播的内容
                    if (mi != mapRelay.end())
                        pfrom->PushMessage(inv.GetCommand(), (*mi).second);
                }
            }
        }
    }


    else if (strCommand == "getblocks")
    {
        CBlockLocator locator;
        uint256 hashStop;
        vRecv >> locator >> hashStop;

		//找到本地有的且在主链上的
        // Find the first block the caller has in the main chain
        CBlockIndex* pindex = const_cast<CBlockIndex*>(locator.GetBlockIndex());

		// 将匹配得到的块索引之后的所有在主链上的块发送出去
        // Send the rest of the chain
        if (pindex)
            pindex = pindex->m_pNextBlkIndex;
        printf("getblocks %d to %s\n", (pindex ? pindex->m_nCurHeight : -1), hashStop.ToString().substr(0,14).c_str());
        for (; pindex; pindex = pindex->m_pNextBlkIndex)
        {
            if (pindex->GetBlockHash() == hashStop)
            {
                printf("  getblocks stopping at %d %s\n", pindex->m_nCurHeight, pindex->GetBlockHash().ToString().substr(0,14).c_str());
                break;
            }

            // Bypass setInventoryKnown in case an inventory message got lost
            //CRITICAL_BLOCK(pfrom->cs_inventory)
            {
                CInv inv(MSG_BLOCK, pindex->GetBlockHash());
				// 判断在已知库存2中是否存在
                // returns true if wasn't already contained in the set
                if (pfrom->setInventoryKnown2.insert(inv).second)
                {
                    pfrom->setInventoryKnown.erase(inv);
                    pfrom->vInventoryToSend.push_back(inv);// 插入对应的库存发送集合中准备发送，在另一个线程中进行发送，发送的消息为inv
                }
            }
        }
    }

	// 交易命令
    else if (strCommand == "tx")
    {
        vector<uint256> vWorkQueue;
        CDataStream vMsg(vRecv);
        CTransaction tx;
        vRecv >> tx;

        CInv inv(MSG_TX, tx.GetHash());
        pfrom->AddInventoryKnown(inv);// 将交易消息放入到对应的已知库存中

        bool fMissingInputs = false;
		// 如果交易能够被接受
        if (tx.AcceptTransaction(true, &fMissingInputs))
        {
            WalletServ::getInstance()->AddToWalletIfMine(tx, NULL);
            RelayMessage(inv, vMsg);// 转播消息
            mapAlreadyAskedFor.erase(inv);
            vWorkQueue.push_back(inv.hash);

			// 递归处理所有依赖这个交易对应的孤儿交易
            // Recursively process any orphan transactions that depended on this one
            for (int i = 0; i < vWorkQueue.size(); i++)
            {
                uint256 hashPrev = vWorkQueue[i];
                for (multimap<uint256, CDataStream*>::iterator mi = WalletServ::getInstance()->mapOrphanTransactionsByPrev.lower_bound(hashPrev);
                     mi != WalletServ::getInstance()->mapOrphanTransactionsByPrev.upper_bound(hashPrev);
                     ++mi)
                {
                    const CDataStream& vMsg = *((*mi).second);
                    CTransaction tx;
                    CDataStream(vMsg) >> tx;
                    CInv inv(MSG_TX, tx.GetHash());

                    if (tx.AcceptTransaction(true))
                    {
                        printf("   accepted orphan tx %s\n", inv.hash.ToString().substr(0,6).c_str());
                        WalletServ::getInstance()->AddToWalletIfMine(tx, NULL);
                        RelayMessage(inv, vMsg);
                        mapAlreadyAskedFor.erase(inv);
                        vWorkQueue.push_back(inv.hash);
                    }
                }
            }

            foreach(uint256 hash, vWorkQueue)
                WalletServ::getInstance()->EraseOrphanTx(hash);
        }
        else if (fMissingInputs)
        {
            printf("storing orphan tx %s\n", inv.hash.ToString().substr(0,6).c_str());
            WalletServ::getInstance()->AddOrphanTx(vMsg); // 如果交易当前不被接受则对应的孤儿交易
        }
    }


    else if (strCommand == "review")
    {
        CDataStream vMsg(vRecv);
        CReview review;
        vRecv >> review;

        CInv inv(MSG_REVIEW, review.GetHash());
        pfrom->AddInventoryKnown(inv);

        if (review.AcceptReview())
        {
            // Relay the original message as-is in case it's a higher version than we know how to parse
            RelayMessage(inv, vMsg);
            mapAlreadyAskedFor.erase(inv);
        }
    }


    else if (strCommand == "block")
    {
        auto_ptr<CBlock> pblock(new CBlock);
        vRecv >> *pblock;

        //// debug print
        printf("received block:\n"); pblock->print();

        CInv inv(MSG_BLOCK, pblock->GetHash());
        pfrom->AddInventoryKnown(inv);// 增加库存

        if (BlockEngine::getInstance()->ProcessBlock(pfrom, pblock.release()))
            mapAlreadyAskedFor.erase(inv);
    }


    else if (strCommand == "getaddr")
    {
        pfrom->vAddrToSend.clear();
        int64 nSince = GetAdjustedTime() - 5 * 24 * 60 * 60; // in the last 5 days
        //CRITICAL_BLOCK(cs_m_cMapAddresses)
        {
            unsigned int nSize = m_cMapAddresses.size();
            foreach(const PAIRTYPE(string, CAddress)& item, m_cMapAddresses)
            {
                const CAddress& addr = item.second;
                //// will need this if we lose IRC
                //if (addr.nTime > nSince || (rand() % nSize) < 500)
                if (addr.nTime > nSince)
                    pfrom->vAddrToSend.push_back(addr);
            }
        }
    }


    else if (strCommand == "checkorder")
    {
        uint256 hashReply;
        CWalletTx order;
        vRecv >> hashReply >> order;

        /// we have a chance to check the order here

        // Keep giving the same key to the same ip until they use it
        if (!mapReuseKey.count(pfrom->addr.ip))
            mapReuseKey[pfrom->addr.ip] = WalletServ::getInstance()->GenerateNewKey();

        // Send back approval of order and pubkey to use
        CScript scriptPubKey;
        scriptPubKey << mapReuseKey[pfrom->addr.ip] << OP_CHECKSIG;
        pfrom->PushMessage("reply", hashReply, (int)0, scriptPubKey);
    }


    else if (strCommand == "submitorder")
    {
        uint256 hashReply;
        CWalletTx wtxNew;
        vRecv >> hashReply >> wtxNew;

        // Broadcast
        if (!wtxNew.AcceptWalletTransaction())
        {
            pfrom->PushMessage("reply", hashReply, (int)1);
            return error("submitorder AcceptWalletTransaction() failed, returning error 1");
        }
        wtxNew.m_bTimeReceivedIsTxTime = true;
        WalletServ::getInstance()->AddToWallet(wtxNew);
        wtxNew.RelayWalletTransaction();
        mapReuseKey.erase(pfrom->addr.ip);

        // Send back confirmation
        pfrom->PushMessage("reply", hashReply, (int)0);
    }


    else if (strCommand == "reply")
    {
        uint256 hashReply;
        vRecv >> hashReply;

        CRequestTracker tracker;
     //   CRITICAL_BLOCK(pfrom->cs_mapRequests)
        {
            map<uint256, CRequestTracker>::iterator mi = pfrom->mapRequests.find(hashReply);
            if (mi != pfrom->mapRequests.end())
            {
                tracker = (*mi).second;
                pfrom->mapRequests.erase(mi);
            }
        }
        if (!tracker.IsNull())
            tracker.fn(tracker.param1, vRecv);
    }


    else
    {
        // Ignore unknown commands for extensibility
        printf("ProcessMessage(%s) : Ignored unknown message\n", strCommand.c_str());
    }


    if (!vRecv.empty())
        printf("ProcessMessage(%s) : %d extra bytes\n", strCommand.c_str(), vRecv.size());

    return true;
}








// 处理节点对应的消息发送
bool NetWorkServ::SendMessages(CNode* pto)
{
    CheckForShutdown(2);
    //CRITICAL_BLOCK(cs_main)
    {
        // Don't send anything until we get their version message
        if (pto->nVersion == 0)
            return true;


        // 消息发送的地址
        // Message: addr
        //
        vector<CAddress> vAddrToSend;
        vAddrToSend.reserve(pto->vAddrToSend.size());
		// 如果发送的地址不在已知地址的集合中，则将其放入临时地址发送数组中
        foreach(const CAddress& addr, pto->vAddrToSend)
            if (!pto->setAddrKnown.count(addr))
                vAddrToSend.push_back(addr);
		// 清空地址发送的数组
        pto->vAddrToSend.clear();
		// 如果临时地址发送数组不为空，则进行地址的消息的发送
        if (!vAddrToSend.empty())
            pto->PushMessage("addr", vAddrToSend);


        // 库存消息处理
        // Message: inventory
        //
        vector<CInv> vInventoryToSend;
        //CRITICAL_BLOCK(pto->cs_inventory)
        {
            vInventoryToSend.reserve(pto->vInventoryToSend.size());
            foreach(const CInv& inv, pto->vInventoryToSend)
            {
                // returns true if wasn't already contained in the set
                if (pto->setInventoryKnown.insert(inv).second)
                    vInventoryToSend.push_back(inv);
            }
            pto->vInventoryToSend.clear();
            pto->setInventoryKnown2.clear();
        }
		// 库存消息发送
        if (!vInventoryToSend.empty())
            pto->PushMessage("inv", vInventoryToSend);


        // getdata消息发送
        // Message: getdata
        //
        vector<CInv> vAskFor;
        int64 nNow = GetTime() * 1000000;
		// 判断节点对应的请求消息map是否为空，且对应的请求map中的消息对应的最早请求时间是否小于当前时间
        while (!pto->mapAskFor.empty() && (*pto->mapAskFor.begin()).first <= nNow)
        {
            const CInv& inv = (*pto->mapAskFor.begin()).second;
            printf("sending getdata: %s\n", inv.ToString().c_str());
            if (!WalletServ::getInstance()->AlreadyHave(inv))
                vAskFor.push_back(inv);// 不存在才需要进行消息发送
            pto->mapAskFor.erase(pto->mapAskFor.begin());// 请求消息处理完一条就删除一条
        }
        if (!vAskFor.empty())
            pto->PushMessage("getdata", vAskFor);

    }
    return true;
}


void NetWorkServ::AbandonRequests(void (*fn)(void*, CDataStream&), void* param1)
{
    // If the dialog might get closed before the reply comes back,
    // call this in the destructor so it doesn't get called after it's deleted.
    foreach(CNode* pnode, m_cNodeLst)
    {
        for (map<uint256, CRequestTracker>::iterator mi = pnode->mapRequests.begin(); mi != pnode->mapRequests.end();)
        {
            CRequestTracker& tracker = (*mi).second;
            if (tracker.fn == fn && tracker.param1 == param1)
                pnode->mapRequests.erase(mi++);
            else
                mi++;
        }
    }
}

bool NetWorkServ::AnySubscribed(unsigned int nChannel)
{
    if (m_pcNodeLocalHost->IsSubscribed(nChannel))
        return true;
    foreach(CNode* pnode, m_cNodeLst)
        if (pnode->IsSubscribed(nChannel))
            return true;
    return false;
}

bool NetWorkServ::LoadAddresses()
{
    DaoServ::getInstance()->LoadAddresses(m_cMapAddresses);
    AddUserProviedAddress();
}

NetWorkServ::NetWorkServ()
:m_cAddrLocalHost(0, DEFAULT_PORT, m_nLocalServices)
{

}

NetWorkServ::~NetWorkServ()
{
    if (NULL != m_pcNodeLocalHost) 
    {
        delete m_pcNodeLocalHost;
        m_pcNodeLocalHost = NULL;
    }
    
}
/* EOF */

