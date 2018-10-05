// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

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
#include "BlockEngine.h"
#include "Network/net.h"
#include "Db/CAddrDB.h"
#include "Db/CWalletDB.h"
#include "TX/CWalletTx.h"

using namespace boost;
void ThreadMessageHandler2(void* parg);
void ThreadSocketHandler2(void* parg);
void ThreadOpenConnections2(void* parg);
extern vector<pthread_t> gThreadList;





//
// Global state variables
//
bool fClient = false;
uint64 nLocalServices = (fClient ? 0 : NODE_NETWORK);
CAddress addrLocalHost(0, DEFAULT_PORT, nLocalServices);// 本地主机地址
CNode nodeLocalHost(INVALID_SOCKET, CAddress("127.0.0.1", nLocalServices));
CNode* pnodeLocalHost = &nodeLocalHost; // 本地节点
bool fShutdown = false; // 标记是否关闭
boost::array<bool, 10> vfThreadRunning; // 线程运行状态标记，0索引表示sockethandler，1索引表示openConnection，2索引表示处理消息
vector<CNode*> vNodes; // 与当前节点相连的节点列表
//CCriticalSection cs_vNodes;
map<vector<unsigned char>, CAddress> mapAddresses; // 节点地址映射：key对应的是ip地址+端口，value是CAddress对象
//CCriticalSection cs_mapAddresses;
map<CInv, CDataStream> mapRelay; // 重新转播的内容
deque<pair<int64, CInv> > vRelayExpiration; // 重播过期记录
//CCriticalSection cs_mapRelay;
map<CInv, int64> mapAlreadyAskedFor; // 已经请求过的请求：对应的value为请求时间（单位到微秒）



CAddress addrProxy; // 代理地址

//socket 连接，根据地址信息创建对应的socket信息
bool ConnectSocket(const CAddress& addrConnect, SOCKET& hSocketRet)
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



bool GetMyExternalIP2(const CAddress& addrConnect, const char* pszGet, const char* pszKeyword, unsigned int& ipRet)
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


bool GetMyExternalIP(unsigned int& ipRet)
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





bool AddAddress(CAddrDB& addrdb, const CAddress& addr)
{
	// 地址不能路由，则不将此地址加入地址库中
    if (!addr.IsRoutable())
        return false;
    if (addr.ip == addrLocalHost.ip)
        return false;
    ////CRITICAL_BLOCK(cs_mapAddresses)
    {
		// 根据地址的ip+port来查询对应的内存对象mapAddresses，
        map<vector<unsigned char>, CAddress>::iterator it = mapAddresses.find(addr.GetKey());
        if (it == mapAddresses.end())
        {
            // New address
            mapAddresses.insert(make_pair(addr.GetKey(), addr));
            addrdb.WriteAddress(addr);
            return true;
        }
        else
        {
            CAddress& addrFound = (*it).second;
            if ((addrFound.m_nServices | addr.m_nServices) != addrFound.m_nServices)
            {
				// 增加地址对应的服务类型，并将其写入数据库
                // Services have been added
                addrFound.m_nServices |= addr.m_nServices;
                addrdb.WriteAddress(addrFound);
                return true;
            }
        }
    }
    return false;
}





void AbandonRequests(void (*fn)(void*, CDataStream&), void* param1)
{
    // If the dialog might get closed before the reply comes back,
    // call this in the destructor so it doesn't get called after it's deleted.
    ////CRITICAL_BLOCK(cs_vNodes)
    {
        foreach(CNode* pnode, vNodes)
        {
     //       //CRITICAL_BLOCK(pnode->cs_mapRequests)
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
    }
}







//
// Subscription methods for the broadcast and subscription system.
// Channel numbers are message numbers, i.e. MSG_TABLE and MSG_PRODUCT.
//
// The subscription system uses a meet-in-the-middle strategy.
// With 100,000 nodes, if senders broadcast to 1000 random nodes and receivers
// subscribe to 1000 random nodes, 99.995% (1 - 0.99^1000) of messages will get through.
//

bool AnySubscribed(unsigned int nChannel)
{
    if (pnodeLocalHost->IsSubscribed(nChannel))
        return true;
   // //CRITICAL_BLOCK(cs_vNodes)
        foreach(CNode* pnode, vNodes)
            if (pnode->IsSubscribed(nChannel))
                return true;
    return false;
}

bool CNode::IsSubscribed(unsigned int nChannel)
{
    if (nChannel >= vfSubscribe.size())
        return false;
    return vfSubscribe[nChannel];
}

void CNode::Subscribe(unsigned int nChannel, unsigned int nHops)
{
    if (nChannel >= vfSubscribe.size())
        return;

    if (!AnySubscribed(nChannel))
    {
        // Relay subscribe
     //   //CRITICAL_BLOCK(cs_vNodes)
            foreach(CNode* pnode, vNodes)
                if (pnode != this)
                    pnode->PushMessage("subscribe", nChannel, nHops);
    }

    vfSubscribe[nChannel] = true;
}

void CNode::CancelSubscribe(unsigned int nChannel)
{
    if (nChannel >= vfSubscribe.size())
        return;

    // Prevent from relaying cancel if wasn't subscribed
    if (!vfSubscribe[nChannel])
        return;
    vfSubscribe[nChannel] = false;

    if (!AnySubscribed(nChannel))
    {
        // Relay subscription cancel
       // //CRITICAL_BLOCK(cs_vNodes)
            foreach(CNode* pnode, vNodes)
                if (pnode != this)
                    pnode->PushMessage("sub-cancel", nChannel);

        // Clear memory, no longer subscribed
        if (nChannel == MSG_PRODUCT)
         //   //CRITICAL_BLOCK(cs_mapProducts)
                mapProducts.clear();
    }
}








// 根据ip在本地存储的节点列表vNodes中查找对应的节点
CNode* FindNode(unsigned int ip)
{
 //   //CRITICAL_BLOCK(cs_vNodes)
    {
        foreach(CNode* pnode, vNodes)
            if (pnode->addr.ip == ip)
                return (pnode);
    }
    return NULL;
}

CNode* FindNode(CAddress addr)
{
    //CRITICAL_BLOCK(cs_vNodes)
    {
        foreach(CNode* pnode, vNodes)
            if (pnode->addr == addr)
                return (pnode);
    }
    return NULL;
}
// 链接对应地址的节点
CNode* ConnectNode(CAddress addrConnect, int64 nTimeout)
{
    if (addrConnect.ip == addrLocalHost.ip)
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
   //     //CRITICAL_BLOCK(cs_vNodes)
            vNodes.push_back(pnode);

     //   //CRITICAL_BLOCK(cs_mapAddresses)
            mapAddresses[addrConnect.GetKey()].nLastFailed = 0;
        return pnode;
    }
    else
    {
        //CRITICAL_BLOCK(cs_mapAddresses)
            mapAddresses[addrConnect.GetKey()].nLastFailed = GetTime();
        return NULL;
    }
}

void CNode::Disconnect()
{
    printf("disconnecting node %s\n", addr.ToString().c_str());

    close(hSocket);

    // If outbound and never got version message, mark address as failed
    if (!fInbound && nVersion == 0)
       // //CRITICAL_BLOCK(cs_mapAddresses)
            mapAddresses[addr.GetKey()].nLastFailed = GetTime();

    // All of a nodes broadcasts and subscriptions are automatically torn down
    // when it goes down, so a node has to stay up to keep its broadcast going.

    ////CRITICAL_BLOCK(cs_mapProducts)
        for (map<uint256, CProduct>::iterator mi = mapProducts.begin(); mi != mapProducts.end();)
            AdvertRemoveSource(this, MSG_PRODUCT, 0, (*(mi++)).second);

    // Cancel subscriptions
    for (unsigned int nChannel = 0; nChannel < vfSubscribe.size(); nChannel++)
        if (vfSubscribe[nChannel])
            CancelSubscribe(nChannel);
}












// socket 处理，parag对应的是本地节点开启的监听socket
void* ThreadSocketHandler(void* parg)
{
//  IMPLEMENT_RANDOMIZE_STACK(ThreadSocketHandler(parg));

    loop
    {
        vfThreadRunning[0] = true;
        CheckForShutdown(0);
        try
        {
            ThreadSocketHandler2(parg);
        }
        CATCH_PRINT_EXCEPTION("ThreadSocketHandler()")
        vfThreadRunning[0] = false;
        sleep(5000);
    }
}
// socket 处理，parag对应的是本地节点开启的监听socket
void ThreadSocketHandler2(void* parg)
{
    printf("ThreadSocketHandler started\n");
    SOCKET hListenSocket = *(SOCKET*)parg; // 获得监听socket
    list<CNode*> vNodesDisconnected;
    int nPrevNodeCount = 0;

    loop
    {
        // Disconnect nodes
        {
            // Disconnect duplicate connections 释放同一个ip重复链接对应的节点，可能是不同端口
            map<unsigned int, CNode*> mapFirst;
            foreach(CNode* pnode, vNodes)
            {
                if (pnode->fDisconnect)
                    continue;
                unsigned int ip = pnode->addr.ip;
				// 本地主机ip地址对应的是0，所以所有的ip地址都应该大于这个ip
                if (mapFirst.count(ip) && addrLocalHost.ip < ip)
                {
                    // In case two nodes connect to each other at once,
                    // the lower ip disconnects its outbound connection
                    CNode* pnodeExtra = mapFirst[ip];

                    if (pnodeExtra->GetRefCount() > (pnodeExtra->fNetworkNode ? 1 : 0))
                        swap(pnodeExtra, pnode);

                    if (pnodeExtra->GetRefCount() <= (pnodeExtra->fNetworkNode ? 1 : 0))
                    {
                        printf("(%d nodes) disconnecting duplicate: %s\n", vNodes.size(), pnodeExtra->addr.ToString().c_str());
                        if (pnodeExtra->fNetworkNode && !pnode->fNetworkNode)
                        {
                            pnode->AddRef();
                            swap(pnodeExtra->fNetworkNode, pnode->fNetworkNode);
                            pnodeExtra->Release();
                        }
                        pnodeExtra->fDisconnect = true;
                    }
                }
                mapFirst[ip] = pnode;
            }
			// 断开不使用的节点
            // Disconnect unused nodes
            vector<CNode*> vNodesCopy = vNodes;
            foreach(CNode* pnode, vNodesCopy)
            {
				// 节点准备释放链接，并且对应的接收和发送缓存区都是空
                if (pnode->ReadyToDisconnect() && pnode->vRecv.empty() && pnode->vSend.empty())
                {
					// 从节点列表中移除
                    // remove from vNodes
                    vNodes.erase(remove(vNodes.begin(), vNodes.end(), pnode), vNodes.end());
                    pnode->Disconnect();

					// 将对应准备释放的节点放在对应的节点释放链接池中，等待对应节点的所有引用释放
                    // hold in disconnected pool until all refs are released
                    pnode->nReleaseTime = max(pnode->nReleaseTime, GetTime() + 5 * 60); // 向后推迟5分钟
                    if (pnode->fNetworkNode)
                        pnode->Release();
                    vNodesDisconnected.push_back(pnode);
                }
            }

			// 删除端口的链接的节点：删除的条件是对应节点的引用小于等于0
            // Delete disconnected nodes
            list<CNode*> vNodesDisconnectedCopy = vNodesDisconnected;
            foreach(CNode* pnode, vNodesDisconnectedCopy)
            {
                // wait until threads are done using it
                if (pnode->GetRefCount() <= 0)
                {
                    bool fDelete = false;
       //             //TRY_CRITICAL_BLOCK(pnode->cs_vSend)
       //              //TRY_CRITICAL_BLOCK(pnode->cs_vRecv)
       //               //TRY_CRITICAL_BLOCK(pnode->cs_mapRequests)
       //                //TRY_CRITICAL_BLOCK(pnode->cs_inventory)
                        fDelete = true;
                    if (fDelete)
                    {
                        vNodesDisconnected.remove(pnode);
                        delete pnode;
                    }
                }
            }
        }
        if (vNodes.size() != nPrevNodeCount)
        {
            nPrevNodeCount = vNodes.size(); // 记录前一次节点对应的数量
            //MainFrameRepaint();
        }


        // 找出哪一个socket有数据要发送
        // Find which sockets have data to receive
        //
        struct timeval timeout;
        timeout.tv_sec  = 0;
        timeout.tv_usec = 50000; // frequency to poll pnode->vSend 咨询节点是否有数据要发送的频率

        fd_set fdsetRecv; // 记录所有节点对应的socket句柄和监听socket句柄
        fd_set fdsetSend; // 记录所有有待发送消息的节点对应的socket句柄
        FD_ZERO(&fdsetRecv);
        FD_ZERO(&fdsetSend);
        SOCKET hSocketMax = 0;
        FD_SET(hListenSocket, &fdsetRecv); // FD_SET将hListenSocket 放入fdsetRecv对应的数组的最后
        hSocketMax = max(hSocketMax, hListenSocket);
        //CRITICAL_BLOCK(cs_vNodes)
        {
            foreach(CNode* pnode, vNodes)
            {
                FD_SET(pnode->hSocket, &fdsetRecv);
                hSocketMax = max(hSocketMax, pnode->hSocket); // 找出所有节点对应的socket的最大值，包括监听socket
  //              TRY_CRITICAL_BLOCK(pnode->cs_vSend)
                    if (!pnode->vSend.empty())
                        FD_SET(pnode->hSocket, &fdsetSend); // FD_SET 字段设置
            }
        }

        vfThreadRunning[0] = false;
		// 函数参考：https://blog.csdn.net/rootusers/article/details/43604729
		/*确定一个或多个套接口的状态，本函数用于确定一个或多个套接口的状态，对每一个套接口，调用者可查询它的可读性、可写性及错误状态信息，用fd_set结构来表示一组等待检查的套接口，在调用返回时，这个结构存有满足一定条件的套接口组的子集，并且select()返回满足条件的套接口的数目。
			简单来说select用来填充一组可用的socket句柄，当满足下列之一条件时：
			1.可以读取的sockets。当这些socket被返回时，在这些socket上执行recv/accept等操作不会产生阻塞;
			2.可以写入的sockets。当这些socket被返回时，在这些socket上执行send等不会产生阻塞;
			3.返回有错误的sockets。
			select()的机制中提供一fd_set的数据结构，实际上市一long类型的数组，每一个数组元素都能与一打开的文件句柄(或者是其他的socket句柄，文件命名管道等)建立联系，建立联系的工作实际上由程序员完成，当调用select()的时候，由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程那一socket或文件可读。
		*/
        int nSelect = select(hSocketMax + 1, &fdsetRecv, &fdsetSend, NULL, &timeout);
        vfThreadRunning[0] = true;
        CheckForShutdown(0);
        if (nSelect == SOCKET_ERROR)
        {
            printf("select failed: %m\n");
            for (int i = 0; i <= hSocketMax; i++)
            {
                FD_SET(i, &fdsetRecv); // 所有的值设置一遍
                FD_SET(i, &fdsetSend);
            }
            sleep(timeout.tv_usec/1000);
        }
		// 随机增加种子：性能计数
        RandAddSeed();

        //// debug print
    /*
        foreach(CNode* pnode, vNodes)
        {
            printf("vRecv = %-5d ", pnode->vRecv.size());
            printf("vSend = %-5d    ", pnode->vSend.size());
        }
        printf("\n");
    */
        // 如果fdsetRecv中有监听socket，则接收改监听socket对应的链接请求，并将链接请求设置为新的节点
        // Accept new connections
        // 判断发送缓冲区中是否有对应的socket，如果有则接收新的交易
        if (FD_ISSET(hListenSocket, &fdsetRecv))
        {
            struct sockaddr_in sockaddr;
            socklen_t len = sizeof(sockaddr);
            SOCKET hSocket = accept(hListenSocket, (struct sockaddr*)&sockaddr, &len);
            CAddress addr(sockaddr);
            if (hSocket == INVALID_SOCKET)
            {
                //if (errno != EWOULDBLOCK)
                  if (errno == EWOULDBLOCK)
                    printf("ERROR ThreadSocketHandler accept failed: %m\n");
            }
            else
            {
                printf("accepted connection from %s\n", addr.ToString().c_str());
                CNode* pnode = new CNode(hSocket, addr, true); // 有新的socket链接，则新建对应的节点，并将节点在加入本地节点列表中
                pnode->AddRef();
                vNodes.push_back(pnode);
            }
        }


        // 对每一个socket进行服务处理
        // Service each socket
        //
        vector<CNode*> vNodesCopy;
        vNodesCopy = vNodes;
        foreach(CNode* pnode, vNodesCopy)
        {
            CheckForShutdown(0);
            SOCKET hSocket = pnode->hSocket; // 获取每一个节点对应的socket

            // 从节点对应的socket中读取对应的数据，将数据放入节点的接收缓冲区中
            // Receive
            //
            if (FD_ISSET(hSocket, &fdsetRecv))
            {
                {
                    CDataStream& vRecv = pnode->vRecv;
                    unsigned int nPos = vRecv.size();

                    const unsigned int nBufSize = 0x10000;
                    vRecv.resize(nPos + nBufSize);// 调整接收缓冲区的大小
                    int nBytes = recv(hSocket, &vRecv[nPos], nBufSize, MSG_DONTWAIT);// 从socket中接收对应的数据
                    vRecv.resize(nPos + max(nBytes, 0));
                    if (nBytes == 0)
                    {
                        // socket closed gracefully （socket优雅的关闭）
                        if (!pnode->fDisconnect)
                            printf("recv: socket closed\n");
                        pnode->fDisconnect = true;
                    }
                    else if (nBytes < 0)
                    {
                        // socket error
                        int nErr = errno;
                        if (nErr != EWOULDBLOCK && nErr != EAGAIN && nErr != EINTR && nErr != EINPROGRESS)
                        {
                            if (!pnode->fDisconnect)
                                printf("recv failed: %d\n", nErr);
                            pnode->fDisconnect = true;
                        }
                    }
                }
            }

            // 将节点对应的发送缓冲中的内容发送出去
            // Send
            //
            if (FD_ISSET(hSocket, &fdsetSend))
            {

                {
                    CDataStream& vSend = pnode->vSend;
                    if (!vSend.empty())
                    {
                        int nBytes = send(hSocket, &vSend[0], vSend.size(), 0); // 从节点对应的发送缓冲区中发送数据出去
                        if (nBytes > 0)
                        {
                            vSend.erase(vSend.begin(), vSend.begin() + nBytes);// 从发送缓冲区中移除发送过的内容
                        }
                        else if (nBytes == 0)
                        {
                            if (pnode->ReadyToDisconnect())
                                pnode->vSend.clear();
                        }
                        else
                        {
                            printf("send error %d\n", nBytes);
                            if (pnode->ReadyToDisconnect())
                                pnode->vSend.clear();
                        }
                    }
                }
            }

        }
        sleep(10);
    }
}










void* ThreadOpenConnections(void* parg)
{
//    IMPLEMENT_RANDOMIZE_STACK(ThreadOpenConnections(parg));

    loop
    {
        vfThreadRunning[1] = true;
        CheckForShutdown(1);
        try
        {
            ThreadOpenConnections2(parg);
        }
        CATCH_PRINT_EXCEPTION("ThreadOpenConnections()")
        vfThreadRunning[1] = false;
        sleep(5000);
    }
}
// 对于每一个打开节点的链接，进行节点之间信息通信，获得节点对应的最新信息，比如节点对应的知道地址进行交换等
void ThreadOpenConnections2(void* parg)
{
    printf("ThreadOpenConnections started\n");

	// 初始化网络连接
    // Initiate network connections
    int nTry = 0;
    bool fIRCOnly = false;
    const int nMaxConnections = 15; // 最大连接数
    loop
    {
        // Wait
        vfThreadRunning[1] = false;
        sleep(5);
        while (vNodes.size() >= nMaxConnections || vNodes.size() >= mapAddresses.size())
        {
            CheckForShutdown(1);
            sleep(50);
        }
        vfThreadRunning[1] = true;
        CheckForShutdown(1);


		// Ip对应的C类地址，相同的C类地址放在一起
        fIRCOnly = !fIRCOnly;
        if (mapIRCAddresses.empty())
            fIRCOnly = false;
        else if (nTry++ < 30 && vNodes.size() < nMaxConnections/2)
            fIRCOnly = true;

        // Make a list of unique class C's
        unsigned char pchIPCMask[4] = { 0xff, 0xff, 0xff, 0x00 };
        unsigned int nIPCMask = *(unsigned int*)pchIPCMask;
        vector<unsigned int> vIPC;
        //CRITICAL_BLOCK(cs_mapIRCAddresses)
        //CRITICAL_BLOCK(cs_mapAddresses)
        {
            vIPC.reserve(mapAddresses.size());
            unsigned int nPrev = 0;
			// mapAddress已经进行排序了，默认是生效排序
            foreach(const PAIRTYPE(vector<unsigned char>, CAddress)& item, mapAddresses)
            {
                const CAddress& addr = item.second;
                if (!addr.IsIPv4())
                    continue;
                if (fIRCOnly && !mapIRCAddresses.count(item.first))
                    continue;

                // Taking advantage of mapAddresses being in sorted order,
                // with IPs of the same class C grouped together.
                unsigned int ipC = addr.ip & nIPCMask;
                if (ipC != nPrev)
                    vIPC.push_back(nPrev = ipC);
            }
        }
        if (vIPC.empty())
            continue;

        // IP选择的过程
        // The IP selection process is designed to limit vulnerability致命性 to address flooding.
        // Any class C (a.b.c.?) has an equal chance of being chosen, then an IP is
        // chosen within the class C.  An attacker may be able to allocate many IPs, but
        // they would normally be concentrated in blocks of class C's.  They can hog独占 the
        // attention within their class C, but not the whole IP address space overall.
        // A lone node in a class C will get as much attention as someone holding all 255
        // IPs in another class C.
        //
        bool fSuccess = false;
        int nLimit = vIPC.size();
        while (!fSuccess && nLimit-- > 0)
        {
            // Choose a random class C 随机获取一个C级别的地址
            unsigned int ipC = vIPC[GetRand(vIPC.size())];

            // Organize all addresses in the class C by IP
            map<unsigned int, vector<CAddress> > mapIP;
            int64 nDelay = ((30 * 60) << vNodes.size());
            if (!fIRCOnly)
            {
                nDelay *= 2;
                if (vNodes.size() >= 3)
                    nDelay *= 4;
                if (!mapIRCAddresses.empty())
                    nDelay *= 100;  
            }
					
            /*
            map::lower_bound(key):返回map中第一个大于或等于key的迭代器指针
            map::upper_bound(key):返回map中第一个大于key的迭代器指针
            */
            for (map<vector<unsigned char>, CAddress>::iterator mi = mapAddresses.lower_bound(CAddress(ipC, 0).GetKey());
                 mi != mapAddresses.upper_bound(CAddress(ipC | ~nIPCMask, 0xffff).GetKey());
                 ++mi)
            {
                const CAddress& addr = (*mi).second;
                if (fIRCOnly && !mapIRCAddresses.count((*mi).first))
                    continue;

                int64 nRandomizer = (addr.nLastFailed * addr.ip * 7777U) % 20000;
                // 当前时间 - 地址连接最新失败的时间 要大于对应节点重连的间隔时间
                if (GetTime() - addr.nLastFailed > nDelay * nRandomizer / 10000)
                    mapIP[addr.ip].push_back(addr); //同一个地址区段不同地址： 同一个地址的不同端口，所有对应同一个ip会有多个地址
            }
            
            if (mapIP.empty())
                break;

            // Choose a random IP in the class C
            map<unsigned int, vector<CAddress> >::iterator mi = mapIP.begin();
			//boost::iterators::advance_adl_barrier::advance(mi, GetRand(mapIP.size())); // 将指针定位到随机位置
			advance(mi, GetRand(mapIP.size())); // 将指针定位到随机位置

			// 遍历同一个ip对应的所有不同端口
            // Once we've chosen an IP, we'll try every given port before moving on
            foreach(const CAddress& addrConnect, (*mi).second)
            {
               // printf("OpenConnection,LocalIP[%s], addrConnect_ip[%s]\n", addrLocalHost.ToStringIP().c_str(), addrConnect.ToStringIP().c_str());
				// ip不能是本地ip，且不能是非ipV4地址，对应的ip地址不在本地的节点列表中
                CheckForShutdown(1);
                if (addrConnect.ip == addrLocalHost.ip || !addrConnect.IsIPv4() || FindNode(addrConnect.ip))
                    continue;
				// 链接对应地址信息的节点
                vfThreadRunning[1] = false;
                CNode* pnode = ConnectNode(addrConnect);
                vfThreadRunning[1] = true;
                CheckForShutdown(1);
                if (!pnode)
                    continue;
                pnode->fNetworkNode = true;

				// 如果本地主机地址能够进行路由，则需要广播我们的地址
                if (addrLocalHost.IsRoutable())
                {
                    // Advertise our address
                    vector<CAddress> vAddrToSend;
                    vAddrToSend.push_back(addrLocalHost);
                    pnode->PushMessage("addr", vAddrToSend); // 将消息推送出去放入vsend中，在消息处理线程中进行处理
                }

				// 从创建的节点获得尽可能多的地址信息，发送消息，在消息处理线程中进行处理
                // Get as many addresses as we can
                pnode->PushMessage("getaddr");

                ////// should the one on the receiving end do this too?
                // Subscribe our local subscription list
				// 新建的节点要订阅我们本地主机订阅的对应通断
                const unsigned int nHops = 0;
                for (unsigned int nChannel = 0; nChannel < pnodeLocalHost->vfSubscribe.size(); nChannel++)
                    if (pnodeLocalHost->vfSubscribe[nChannel])
                        pnode->PushMessage("subscribe", nChannel, nHops);

                fSuccess = true;
                break;
            }
        }
    }
}







// 消息处理线程
void* ThreadMessageHandler(void* parg)
{
 //   IMPLEMENT_RANDOMIZE_STACK(ThreadMessageHandler(parg));

    loop
    {
        vfThreadRunning[2] = true;
        CheckForShutdown(2);
        try
        {
            ThreadMessageHandler2(parg);
        }
        CATCH_PRINT_EXCEPTION("ThreadMessageHandler()")
        vfThreadRunning[2] = false;
        sleep(5000);
    }
}
// 消息处理线程
void ThreadMessageHandler2(void* parg)
{
    printf("ThreadMessageHandler started\n");
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    loop
    {
		// 轮询链接的节点用于消息处理
        // Poll the connected nodes for messages
        vector<CNode*> vNodesCopy;
        vNodesCopy = vNodes;
		// 对每一个节点进行消息处理：发送消息和接收消息
        foreach(CNode* pnode, vNodesCopy)
        {

            pnode->AddRef();

            // Receive messages
            BlockEngine::getInstance()->ProcessMessages(pnode);

            // Send messages
            BlockEngine::getInstance()->SendMessages(pnode);

            pnode->Release();
        }

        // Wait and allow messages to bunch up
        vfThreadRunning[2] = false;
        sleep(10);
        vfThreadRunning[2] = true;
        CheckForShutdown(2);
    }
}



//// todo: start one thread per processor, use getenv("NUMBER_OF_PROCESSORS")
void* ThreadBitcoinMiner(void* parg)
{
    vfThreadRunning[3] = true;
    CheckForShutdown(3);
    try
    {
        bool fRet = BlockEngine::getInstance()->BitcoinMiner();
        printf("BitcoinMiner returned %s\n\n\n", fRet ? "true" : "false");
    }
    CATCH_PRINT_EXCEPTION("BitcoinMiner()")
    vfThreadRunning[3] = false;
}










// 启动节点
bool StartNode(string& strError)
{
    strError = "";
    // Get local host ip
    char pszHostName[255]= {0};
    if (gethostname(pszHostName, 255) == SOCKET_ERROR)
    {
        strError = strprintf("Error: Unable to get IP address of this computer (gethostname returned error %d)", errno);
        printf("%s\n", strError.c_str());
        return false;
    }
    struct hostent* phostent = gethostbyname(pszHostName);
    if (!phostent)
    {
        strError = strprintf("Error: Unable to get IP address of this computer (gethostbyname returned error %d)", errno);
        printf("%s\n", strError.c_str());
        return false;
    }
    addrLocalHost = CAddress(*(long*)(phostent->h_addr_list[0]),
                             DEFAULT_PORT,
                             nLocalServices);

    printf("addrLocalHost = %s\n", addrLocalHost.ToString().c_str());

    // Create socket for listening for incoming connections
    SOCKET hListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hListenSocket == INVALID_SOCKET)
    {
        strError = strprintf("Error: Couldn't open socket for incoming connections (socket returned error %d)", errno);
        printf("%s\n", strError.c_str());
        return false;
    }

    // Set to nonblocking, incoming connections will also inherit this
    int flag = 0;
    flag = fcntl(hListenSocket, F_GETFL, NULL);
    fcntl(hListenSocket, F_SETFL, flag|O_NONBLOCK);

    // Reuse Addr
    int opt = 1;
    socklen_t opt_len = sizeof(opt);
    setsockopt(hListenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len);
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound
    int nRetryLimit = 15;
    struct sockaddr_in sockaddr = addrLocalHost.GetSockAddr();
    if (bind(hListenSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        int nErr = errno;
        if (nErr == EADDRINUSE)
            strError = strprintf("Error: Unable to bind to port %s on this computer. The program is probably already running.", addrLocalHost.ToString().c_str());
        else
            strError = strprintf("Error: Unable to bind to port %s on this computer (bind returned error %d)", addrLocalHost.ToString().c_str(), nErr);
        printf("%s\n", strError.c_str());
        return false;
    }
    printf("bound to addrLocalHost = %s\n\n", addrLocalHost.ToString().c_str());

    // Listen for incoming connections
    if (listen(hListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        strError = strprintf("Error: Listening for incoming connections failed (listen returned error %d)", errno);
        printf("%s\n", strError.c_str());
        return false;
    }
    CAddress& addrIncoming = BlockEngine::getInstance()->addrIncoming;
    // Get our external IP address for incoming connections
    if (addrIncoming.ip)
        addrLocalHost.ip = addrIncoming.ip;
    if (GetMyExternalIP(addrLocalHost.ip))
    {
        addrIncoming = addrLocalHost;
        CWalletDB().WriteSetting("addrIncoming", addrIncoming);
    }

    /*IRC是Internet Relay Chat 的英文缩写，中文一般称为互联网中继聊天。
	它是由芬兰人Jarkko Oikarinen于1988年首创的一种网络聊天协议。
	经过十年的发展，目前世界上有超过60个国家提供了IRC的服务。IRC的工作原理非常简单，
	您只要在自己的PC上运行客户端软件，然后通过因特网以IRC协议连接到一台IRC服务器上即可。
	它的特点是速度非常之快，聊天时几乎没有延迟的现象，并且只占用很小的带宽资源。
	所有用户可以在一个被称为\"Channel\"（频道）的地方就某一话题进行交谈或密谈。
	每个IRC的使用者都有一个Nickname（昵称）。
	IRC用户使用特定的用户端聊天软件连接到IRC服务器，
	通过服务器中继与其他连接到这一服务器上的用户交流，
	所以IRC的中文名为“因特网中继聊天”。
	*/
    pthread_t pid = 0;
#if 0
    // Get addresses from IRC and advertise ours
    if (pthread_create(&pid, NULL, ThreadIRCSeed, NULL) == -1)
        printf("Error: pthread_create(ThreadIRCSeed) failed\n");
    gThreadList.push_back(pid);
#endif
	// 启动线程
    //
    // Start threads
    //
    
    if (pthread_create(&pid, NULL, ThreadSocketHandler, (void*)new SOCKET(hListenSocket)) == -1)
    {
        strError = "Error: pthread_create(ThreadSocketHandler) failed";
        printf("%s\n", strError.c_str());
        return false;
    }
    gThreadList.push_back(pid);
    
    if (pthread_create(&pid, NULL, ThreadMessageHandler, NULL) == -1)
    {
        strError = "Error: pthread_create(ThreadMessageHandler) failed";
        printf("%s\n", strError.c_str());
        return false;
    } 
    gThreadList.push_back(pid);
    
    if (pthread_create(&pid, NULL, ThreadOpenConnections, NULL) == -1)
    {
        strError = "Error: pthread_create(ThreadOpenConnections) failed";
        printf("%s\n", strError.c_str());
        return false;
    }
    gThreadList.push_back(pid);
    return true;
}

bool StopNode()
{
    printf("StopNode()\n");
    fShutdown = true;
    BlockEngine::getInstance()->nTransactionsUpdated++;
    int64 nStart = GetTime();
    while (vfThreadRunning[0] || vfThreadRunning[2] || vfThreadRunning[3])
    {
        if (GetTime() - nStart > 15)
            break;
        sleep(20);
    }
    if (vfThreadRunning[0]) printf("ThreadSocketHandler still running\n");
    if (vfThreadRunning[1]) printf("ThreadOpenConnections still running\n");
    if (vfThreadRunning[2]) printf("ThreadMessageHandler still running\n");
    if (vfThreadRunning[3]) printf("ThreadBitcoinMiner still running\n");
    while (vfThreadRunning[2])
        sleep(20);
    sleep(50);

    // Sockets shutdown
 //   WSACleanup();
    return true;
}

void CheckForShutdown(int n)
{
    if (fShutdown)
    {
        if (n != -1)
            vfThreadRunning[n] = false;
        if (n == 0)
            foreach(CNode* pnode, vNodes)
                close(pnode->hSocket);
//        _endthread();
    }
}

/* EOF */
