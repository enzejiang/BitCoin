/*
 * =====================================================================================
 *
 *       Filename:  NetWorkThread.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/06/2018 03:22:03 PM
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
#include "BlockEngine/BlockEngine.h"
#include "DAO/CWalletDB.h"
#include "WalletService/CWalletTx.h"
#include "WalletService/WalletServ.h"
#include "NetWorkServ.h"
using namespace Enze;

void* ThreadMessageHandler(void* parg);
void* ThreadSocketHandler(void* parg);
void* ThreadOpenConnections(void* parg);
extern vector<pthread_t> gThreadList;

//
// Global state variables
//
bool fShutdown = false; // 标记是否关闭
boost::array<bool, 10> vfThreadRunning; // 线程运行状态标记，0索引表示sockethandler，1索引表示openConnection，2索引表示处理消息

// 启动节点
bool NetWorkServ::StartNode()
{
    string strError = "";
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
    m_cAddrLocalHost = CAddress(*(long*)(phostent->h_addr_list[0]),
                             DEFAULT_PORT,
                             m_nLocalServices);

    printf("m_cAddrLocalHost = %s\n", m_cAddrLocalHost.ToString().c_str());

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
    struct sockaddr_in sockaddr = m_cAddrLocalHost.GetSockAddr();
    if (bind(hListenSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        int nErr = errno;
        if (nErr == EADDRINUSE)
            strError = strprintf("Error: Unable to bind to port %s on this computer. The program is probably already running.", m_cAddrLocalHost.ToString().c_str());
        else
            strError = strprintf("Error: Unable to bind to port %s on this computer (bind returned error %d)", m_cAddrLocalHost.ToString().c_str(), nErr);
        printf("%s\n", strError.c_str());
        return false;
    }
    printf("bound to m_cAddrLocalHost = %s\n\n", m_cAddrLocalHost.ToString().c_str());

    // Listen for incoming connections
    if (listen(hListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        strError = strprintf("Error: Listening for incoming connections failed (listen returned error %d)", errno);
        printf("%s\n", strError.c_str());
        return false;
    }
    CAddress& addrIncoming = WalletServ::getInstance()->addrIncoming;
    // Get our external IP address for incoming connections
    if (addrIncoming.ip)
        m_cAddrLocalHost.ip = addrIncoming.ip;
    if (GetMyExternalIP(m_cAddrLocalHost.ip))
    {
        addrIncoming = m_cAddrLocalHost;
        //CWalletDB().WriteSetting("addrIncoming", addrIncoming);
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

bool NetWorkServ::StopNode()
{
    printf("StopNode()\n");
    fShutdown = true;
    WalletServ::getInstance()->nTransactionsUpdated++;
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

void NetWorkServ::CheckForShutdown(int n)
{
    if (fShutdown)
    {
        if (n != -1)
            vfThreadRunning[n] = false;
        if (n == 0)
            foreach(CNode* pnode, m_cNodeLst)
                close(pnode->hSocket);
    }
}

// socket 处理，parag对应的是本地节点开启的监听socket
void* ThreadSocketHandler(void* parg)
{
//  IMPLEMENT_RANDOMIZE_STACK(ThreadSocketHandler(parg));

    loop
    {
        vfThreadRunning[0] = true;
        //CheckForShutdown(0);
        try
        {
            NetWorkServ::getInstance()->SocketHandler(parg);
        }
        CATCH_PRINT_EXCEPTION("ThreadSocketHandler()")
        vfThreadRunning[0] = false;
        sleep(5000);
    }
}
// socket 处理，parag对应的是本地节点开启的监听socket
void NetWorkServ::SocketHandler(void* parg)
{
    printf("ThreadSocketHandler started\n");
    SOCKET hListenSocket = *(SOCKET*)parg; // 获得监听socket
    list<CNode*> m_cNodeLstDisconnected;
    int nPrevNodeCount = 0;

    loop
    {
        // Disconnect nodes
        {
            // Disconnect duplicate connections 释放同一个ip重复链接对应的节点，可能是不同端口
            map<unsigned int, CNode*> mapFirst;
            foreach(CNode* pnode, m_cNodeLst)
            {
                if (pnode->fDisconnect)
                    continue;
                unsigned int ip = pnode->addr.ip;
				// 本地主机ip地址对应的是0，所以所有的ip地址都应该大于这个ip
                if (mapFirst.count(ip) && m_cAddrLocalHost.ip < ip)
                {
                    // In case two nodes connect to each other at once,
                    // the lower ip disconnects its outbound connection
                    CNode* pnodeExtra = mapFirst[ip];

                    if (pnodeExtra->GetRefCount() > (pnodeExtra->fNetworkNode ? 1 : 0))
                        printf("error %s_%d\n", __FILE__, __LINE__);
                        //swap(pnodeExtra, pnode);

                    if (pnodeExtra->GetRefCount() <= (pnodeExtra->fNetworkNode ? 1 : 0))
                    {
                        printf("(%d nodes) disconnecting duplicate: %s\n", m_cNodeLst.size(), pnodeExtra->addr.ToString().c_str());
                        if (pnodeExtra->fNetworkNode && !pnode->fNetworkNode)
                        {
                            pnode->AddRef();
                            printf("error %s_%d\n", __FILE__, __LINE__);
                            //swap(pnodeExtra->fNetworkNode, pnode->fNetworkNode);
                            pnodeExtra->Release();
                        }
                        pnodeExtra->fDisconnect = true;
                    }
                }
                mapFirst[ip] = pnode;
            }
			// 断开不使用的节点
            // Disconnect unused nodes
            vector<CNode*> m_cNodeLstCopy = m_cNodeLst;
            foreach(CNode* pnode, m_cNodeLstCopy)
            {
				// 节点准备释放链接，并且对应的接收和发送缓存区都是空
                if (pnode->ReadyToDisconnect() && pnode->vRecv.empty() && pnode->vSend.empty())
                {
					// 从节点列表中移除
                    // remove from m_cNodeLst
                    m_cNodeLst.erase(remove(m_cNodeLst.begin(), m_cNodeLst.end(), pnode), m_cNodeLst.end());
                    pnode->Disconnect();

					// 将对应准备释放的节点放在对应的节点释放链接池中，等待对应节点的所有引用释放
                    // hold in disconnected pool until all refs are released
                    pnode->nReleaseTime = max(pnode->nReleaseTime, GetTime() + 5 * 60); // 向后推迟5分钟
                    if (pnode->fNetworkNode)
                        pnode->Release();
                    m_cNodeLstDisconnected.push_back(pnode);
                }
            }

			// 删除端口的链接的节点：删除的条件是对应节点的引用小于等于0
            // Delete disconnected nodes
            list<CNode*> m_cNodeLstDisconnectedCopy = m_cNodeLstDisconnected;
            foreach(CNode* pnode, m_cNodeLstDisconnectedCopy)
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
                        m_cNodeLstDisconnected.remove(pnode);
                        delete pnode;
                    }
                }
            }
        }
        if (m_cNodeLst.size() != nPrevNodeCount)
        {
            nPrevNodeCount = m_cNodeLst.size(); // 记录前一次节点对应的数量
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
        //CRITICAL_BLOCK(cs_m_cNodeLst)
        {
            foreach(CNode* pnode, m_cNodeLst)
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
        foreach(CNode* pnode, m_cNodeLst)
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
                m_cNodeLst.push_back(pnode);
            }
        }


        // 对每一个socket进行服务处理
        // Service each socket
        //
        vector<CNode*> m_cNodeLstCopy;
        m_cNodeLstCopy = m_cNodeLst;
        foreach(CNode* pnode, m_cNodeLstCopy)
        {
            CheckForShutdown(0);
            SOCKET hSocket = pnode->hSocket; // 获取每一个节点对应的socket

            // 从节点对应的socket中读取对应的数据，将数据放入节点的接收缓冲区中
            // Receive
            //
            if (FD_ISSET(hSocket, &fdsetRecv))
            {
                printf("error %s_%d\n", __FILE__, __LINE__);
#if 0
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
#endif
            }

            // 将节点对应的发送缓冲中的内容发送出去
            // Send
            //
            if (FD_ISSET(hSocket, &fdsetSend))
            {
                printf("error %s_%d\n", __FILE__, __LINE__);
#if 0
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
#endif
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
        //CheckForShutdown(1);
        try
        {
            NetWorkServ::getInstance()->OpenConnections(parg);
        }
        CATCH_PRINT_EXCEPTION("ThreadOpenConnections()")
        vfThreadRunning[1] = false;
        sleep(5000);
    }
}
// 对于每一个打开节点的链接，进行节点之间信息通信，获得节点对应的最新信息，比如节点对应的知道地址进行交换等
void NetWorkServ::OpenConnections(void* parg)
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
        while (m_cNodeLst.size() >= nMaxConnections || m_cNodeLst.size() >= m_cMapAddresses.size())
        {
            CheckForShutdown(1);
            sleep(50);
        }
        vfThreadRunning[1] = true;
        CheckForShutdown(1);


		// Ip对应的C类地址，相同的C类地址放在一起
        fIRCOnly = !fIRCOnly;
        fIRCOnly = false;
#if 0
        if (mapIRCAddresses.empty())
            fIRCOnly = false;
        else if (nTry++ < 30 && m_cNodeLst.size() < nMaxConnections/2)
            fIRCOnly = true;
#endif
        // Make a list of unique class C's
        unsigned char pchIPCMask[4] = { 0xff, 0xff, 0xff, 0x00 };
        unsigned int nIPCMask = *(unsigned int*)pchIPCMask;
        vector<unsigned int> vIPC;
        //CRITICAL_BLOCK(cs_mapIRCAddresses)
        //CRITICAL_BLOCK(cs_m_cMapAddresses)
        {
            vIPC.reserve(m_cMapAddresses.size());
            unsigned int nPrev = 0;
			// mapAddress已经进行排序了，默认是生效排序
            foreach(const PAIRTYPE(string, CAddress)& item, m_cMapAddresses)
            {
                const CAddress& addr = item.second;
                if (!addr.IsIPv4())
                    continue;
#if 0
                if (fIRCOnly && !mapIRCAddresses.count(item.first))
                    continue;
#endif
                // Taking advantage of m_cMapAddresses being in sorted order,
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
            int64 nDelay = ((30 * 60) << m_cNodeLst.size());
            if (!fIRCOnly)
            {
                nDelay *= 2;
                if (m_cNodeLst.size() >= 3)
                    nDelay *= 4;
                
                //if (!mapIRCAddresses.empty())
                //    nDelay *= 100;  
            }
					
            /*
            map::lower_bound(key):返回map中第一个大于或等于key的迭代器指针
            map::upper_bound(key):返回map中第一个大于key的迭代器指针
            */
            for (map<string, CAddress>::iterator mi = m_cMapAddresses.lower_bound(CAddress(ipC, 0).GetKey());
                 mi != m_cMapAddresses.upper_bound(CAddress(ipC | ~nIPCMask, 0xffff).GetKey());
                 ++mi)
            {
                const CAddress& addr = (*mi).second;
                //if (fIRCOnly && !mapIRCAddresses.count((*mi).first))
                //    continue;

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
               // printf("OpenConnection,LocalIP[%s], addrConnect_ip[%s]\n", m_cAddrLocalHost.ToStringIP().c_str(), addrConnect.ToStringIP().c_str());
				// ip不能是本地ip，且不能是非ipV4地址，对应的ip地址不在本地的节点列表中
                CheckForShutdown(1);
                if (addrConnect.ip == m_cAddrLocalHost.ip || !addrConnect.IsIPv4() || FindNode(addrConnect.ip))
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
                if (m_cAddrLocalHost.IsRoutable())
                {
                    // Advertise our address
                    vector<CAddress> vAddrToSend;
                    vAddrToSend.push_back(m_cAddrLocalHost);
                    pnode->PushMessage("addr", vAddrToSend); // 将消息推送出去放入vsend中，在消息处理线程中进行处理
                }

				// 从创建的节点获得尽可能多的地址信息，发送消息，在消息处理线程中进行处理
                // Get as many addresses as we can
                pnode->PushMessage("getaddr");

                ////// should the one on the receiving end do this too?
                // Subscribe our local subscription list
				// 新建的节点要订阅我们本地主机订阅的对应通断
                const unsigned int nHops = 0;
                for (unsigned int nChannel = 0; nChannel < m_pcNodeLocalHost->vfSubscribe.size(); nChannel++)
                    if (m_pcNodeLocalHost->vfSubscribe[nChannel])
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
        //vfThreadRunning[2] = true;
        //CheckForShutdown(2);
        try
        {
            NetWorkServ::getInstance()->MessageHandler(parg);
        }
        CATCH_PRINT_EXCEPTION("ThreadMessageHandler()")
        //vfThreadRunning[2] = false;
        sleep(5000);
    }
}
// 消息处理线程
void NetWorkServ::MessageHandler(void* parg)
{
    printf("ThreadMessageHandler started\n");
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    loop
    {
		// 轮询链接的节点用于消息处理
        // Poll the connected nodes for messages
        vector<CNode*> m_cNodeLstCopy;
        m_cNodeLstCopy = m_cNodeLst;
		// 对每一个节点进行消息处理：发送消息和接收消息
        foreach(CNode* pnode, m_cNodeLstCopy)
        {

            pnode->AddRef();

            // Receive messages
            ProcessMessages(pnode);

            // Send messages
            SendMessages(pnode);

            pnode->Release();
        }

        // Wait and allow messages to bunch up
        vfThreadRunning[2] = false;
        sleep(10);
        vfThreadRunning[2] = true;
        CheckForShutdown(2);
    }
}



/* EOF */

