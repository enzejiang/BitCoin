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
#include <poll.h>
#include<unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "BlockEngine/BlockEngine.h"
#include "DAO/CWalletDB.h"
#include "WalletService/CWalletTx.h"
#include "WalletService/WalletServ.h"
#include "CommonBase/ProtocSerialize.h"
#include "CommonBase/market.h"
#include "ProtocSrc/Message.pb.h"
#include "ProtocSrc/ServerMessage.pb.h"
#include "NetWorkServ.h"
#include "PeerNode.h"
#include "zhelpers.h"
#include "SocketWraper.h"
#include <thread>
using namespace Enze;

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
    
    CAddress& addrIncoming = WalletServ::getInstance()->addrIncoming;
    // Get our external IP address for incoming connections
    if (addrIncoming.ip)
        m_cAddrLocalHost.ip = addrIncoming.ip;
    if (GetMyExternalIP(m_cAddrLocalHost.ip))
    {
        addrIncoming = m_cAddrLocalHost;
        //CWalletDB().WriteSetting("addrIncoming", addrIncoming);
    }
    printf("m_cAddrLocalHost = %s\n", m_cAddrLocalHost.ToString().c_str());

    m_strIdentity = "tcp://"+m_cAddrLocalHost.ToString();
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
     printf("start Node\n");

     std::thread t4(&NetWorkServ::AddrManagerThread, this);
     t4.detach();

   // std::thread t1(ThreadSocketHandler);
    std::thread t1(&NetWorkServ::SocketHandler, this);
    t1.detach();
    
   // std::thread t2(ThreadMessageHandler);
    std::thread t2(&NetWorkServ::MessageHandler, this);
    t2.detach();
    
    std::thread t3(&NetWorkServ::OpenConnections, this);
    t3.detach();
    
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
 //   printf("%s---%d\n", __FILE__, __LINE__);
#if 0
    if (fShutdown)
    {
        if (n != -1)
            vfThreadRunning[n] = false;
        if (n == 0)
            foreach(PeerNode* pnode, m_cPeerNodeLst)
                close(pnode->hSocket);
    }
#endif 
}


void NetWorkServ::AddrManagerThread()
{
#if 0
    struct hostent* phostent = gethostbyname("www.pocbitcoin.com");
    if (!phostent)
    {
        string strError = strprintf("Error: Unable to get IP address of www.pocbitcoin.com (gethostbyname returned error %d)", errno);
        printf("%s\n", strError.c_str());
        return ;
    }

    char* servIp = phostent->h_addr_list[0];
#endif
    char* servIp = "127.0.0.1";
    string servEndPoint = string("tcp://")+string(servIp)+string(":6667");

    
    struct sockaddr_in servAddr;
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(servIp);
    servAddr.sin_port = htons(6666);

    struct sockaddr_in udpServAddr;
    bzero(&udpServAddr, sizeof(udpServAddr));
    udpServAddr.sin_family = AF_INET;
    udpServAddr.sin_addr.s_addr = inet_addr(servIp);
    udpServAddr.sin_port = htons(6666);
    CAddress updServ(udpServAddr);
    m_iSocketFd = udp_socket(m_cAddrLocalHost, updServ, false);

    void* server = zmq_socket(m_cZmqCtx, ZMQ_REQ);
    zmq_setsockopt(server, ZMQ_IDENTITY, m_strIdentity.c_str(), m_strIdentity.length());
    if (0 != zmq_connect(server, servEndPoint.c_str())) {
        printf("%s---%d, zmq_connect error %m\n", __FILE__, __LINE__);
        return;
    } 
    
//    s_sendmore(server, "");
    s_send(server, "getAddr");
    printf("star recv from[%s]\n", servEndPoint.c_str());

#if 1
    CAddress addr(servAddr);
    sock_sendto(m_iSocketFd, "ping", addr);
    zmq_pollitem_t items [] = { {server, 0, ZMQ_POLLIN, 0},
                                {NULL, m_iSocketFd, ZMQ_POLLIN, 0}    
                        };//new zmq_pollitem_t[m_cPeerNodeLst.size()];

    long timeout = 1000; //1s frequency to poll pnode->vSend 咨询节点是否有数据要发送的频率,
    
    while(1) {
        int retCnt = zmq_poll(items, 2, timeout * 5);
        if ( -1 == retCnt) { 
            printf("%s---%d, error %m\n", __FILE__, __LINE__);
            break;
        }
    
        if (items[0].revents & ZMQ_POLLIN) {
            printf("start recv from\n");

            char* pData = s_recv(server);
            //printf("recv from[%s]\n",pData);
           // pData = s_recv(server);
            //printf("recv from[%s]\n",pData);

            Enze::ServMsg cMsg;
            cMsg.ParsePartialFromString(pData);
            AddAddress(cMsg);
        }
        
        if (items[1].revents & ZMQ_POLLIN) {
        
        }
     
    }
#endif
}

void NetWorkServ::NodeSyncThread()
{
#if 0
    printf("%s---%d\n", __FILE__, __LINE__);
    printf("NetWorkServ::NodeSyncThread--start\n");
    void* worker = zmq_socket(m_cZmqCtx, ZMQ_DEALER);
    zmq_connect(worker, "inproc://backend.ipc");
    while(1) 
    {
        char* brokerId = s_recv(worker);
        printf("brokerid [%s]\n", brokerId);
        char* repId  = s_recv(worker);
        printf("repId [%s]\n", repId);
        char*ReqType = s_recv(worker);
        printf("Recv Data, brokerId[%s], repId[%s]\n", brokerId, repId);
        printf("Recv Data, ReqType[%s]\n", ReqType);
        if (0 == strcmp("ping", ReqType)) {;
            s_sendmore(worker, brokerId);
            s_sendmore(worker, repId);
            s_send(worker, (char*)m_strIdentity.c_str()); 
            printf("Recv Data, ReqType[%s]--rep\n", ReqType);
            if (!FindNode(brokerId)) {
                printf("Recv Data, ReqType not Find[%s]\n", brokerId);
                AddNewAddrByEndPoint(repId);
            }

            free(brokerId);
            free(repId);
            free(ReqType);
        }
        else {
            char* pData = s_recv(worker);
            PeerNode *pNode = FindNode(brokerId);
            if (!pNode) {
                pNode = ConnectNode(brokerId);
            }
            free(brokerId);
            free(repId);
            free(ReqType);
            if (!pNode) {
                printf("NetWorkServ::NodeSyncThread--Node[%s] is not alive\n", repId);
                free(pData);
                continue;
            }
            pNode->AddRef();
            string req = pData;
            PB_MessageData *cProtoc = new PB_MessageData;
            cProtoc->ParsePartialFromString(req);
            pNode->AddRecvMessage(cProtoc);
            pNode->Release();
            free(pData);
#if 0
            if (PB_MessageData_Mesg_Kind_MK_Version == cProtoc->emsgkind()) {
                printf("NetWorkServ::MessageRecv--recv Version[%d]\n", cProtoc->cversion().nversion());
            }
            cProtoc->Clear();
            cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_Reply);
            cProtoc->set_hashreply("jyb");
           // SeriaAddress(m_cAddrLocalHost, cProtoc.mu);
            string strData;
            cProtoc->SerializePartialToString(&strData);
            s_sendmore(worker, brokerId);
            s_sendmore(worker, repId);
            s_sendmore(worker, (char*)m_strIdentity.c_str());
            s_send(worker, (char*)strData.c_str());
#endif
        }

        sleep(1);
    }
#endif 

}


void NetWorkServ::MessageRecv()
{
    
    void* backend = zmq_socket(m_cZmqCtx, ZMQ_DEALER);
    zmq_setsockopt(backend, ZMQ_IDENTITY, "backend", m_strIdentity.length());
    zmq_bind(backend, "inproc://backend.ipc");
    printf("NetWorkServ::MessageRecv--start\n");
    void * frontend = zmq_socket(m_cZmqCtx, ZMQ_ROUTER);
   // string ipAddr = "tcp://"+m_cAddrLocalHost.ToString();
    printf("m_strIdentity [%s]\n", m_strIdentity.c_str());
    zmq_setsockopt(frontend, ZMQ_IDENTITY, m_strIdentity.c_str(), m_strIdentity.length());
    zmq_bind(frontend, m_strIdentity.c_str());
   
    std::thread t0(&NetWorkServ::NodeSyncThread, this);
    t0.detach();
    
    std::thread t1(&NetWorkServ::NodeSyncThread, this);
    t1.detach();

    zmq_proxy(frontend, backend, NULL);
}

void NetWorkServ::SocketHandler()
{
    printf("ThreadSocketHandler started\n");
    list<PeerNode*> cPeerNodeLstDisconnected;
    loop
    {
        // Disconnect duplicate connections 释放同一个ip重复链接对应的节点，可能是不同端口
        map<unsigned int, PeerNode*> mapFirst;
        foreach(auto it, m_cPeerNodeLst)
        {
            PeerNode* pnode= it;
            if (pnode->isDistconnect())
                continue;
            unsigned int ip = pnode->getAddr().ip;
            // 本地主机ip地址对应的是0，所以所有的ip地址都应该大于这个ip
            if (mapFirst.count(ip) && m_cAddrLocalHost.ip < ip)
            {
                // In case two nodes connect to each other at once,
                // the lower ip disconnects its outbound connection
                PeerNode* pnodeExtra = mapFirst[ip];

                if (pnodeExtra->GetRefCount() > (pnodeExtra->isNetworkNode() ? 1 : 0))
                    std::swap(pnodeExtra, pnode);

                if (pnodeExtra->GetRefCount() <= (pnodeExtra->isNetworkNode() ? 1 : 0))
                {
                    printf("(%d nodes) disconnecting duplicate: %s\n", m_cPeerNodeLst.size(), pnodeExtra->getAddr().ToString().c_str());
                    if (pnodeExtra->isNetworkNode() && !pnode->isNetworkNode())
                    {
                        pnode->AddRef();
                        //printf("error %s_%d\n", __FILE__, __LINE__);
                        bool bNode = pnode->isNetworkNode();
                        bool bExtNode = pnodeExtra->isNetworkNode();
                        pnode->setNetworkState(bExtNode);
                        pnodeExtra->setNetworkState(bNode);
                        pnodeExtra->Release();
                    }
                    pnodeExtra->setNetworkState(true);
                }
            }
            mapFirst[ip] = pnode;
        }
        // 断开不使用的节点
        // Disconnect unused nodes
        vector<PeerNode*> cPeerNodeLstCopy = m_cPeerNodeLst;
        foreach(auto it, cPeerNodeLstCopy)
        {
            PeerNode* pnode= it;
            // 节点准备释放链接，并且对应的接收和发送缓存区都是空
            if (pnode->ReadyToDisconnect() && pnode->isEmptyRcvLst() && pnode->isEmptySndLst())
            {
                // 从节点列表中移除
                // remove from m_cPeerNodeLst
                m_cPeerNodeLst.erase(remove(m_cPeerNodeLst.begin(), m_cPeerNodeLst.end(), it), m_cPeerNodeLst.end());
                //printf("Remove Node[%s] From List\n",it.first.c_str());
                //m_cPeerNodeLst.erase(it.first);
                pnode->Disconnect();

                // 将对应准备释放的节点放在对应的节点释放链接池中，等待对应节点的所有引用释放
                // hold in disconnected pool until all refs are released
                pnode->setReleaseTime(max(pnode->getReleaseTime(), GetTime() + 5 * 60)); // 向后推迟5分钟
                if (pnode->isNetworkNode())
                    pnode->Release();
                cPeerNodeLstDisconnected.push_back(pnode);
            }
        }

        // 删除端口的链接的节点：删除的条件是对应节点的引用小于等于0
        // Delete disconnected nodes
        // map<string, PeerNode*> cPeerNodeLstDisconnectedCopy = cPeerNodeLstDisconnected;
        foreach(PeerNode* pnode, cPeerNodeLstDisconnected)
        {
            // wait until threads are done using it
            if (pnode->GetRefCount() <= 0)
            {
                cPeerNodeLstDisconnected.remove(pnode);
                delete pnode;
            }
        }

        if (m_cPeerNodeLst.size() == 0)
        {
           sleep(10);
           continue;
        }

        //zmq_pollitem_t items[] = {{0, m_iSocketFd, ZMQ_POLLIN, 0}};//new zmq_pollitem_t[m_cPeerNodeLst.size()];
        int item_num  = m_cPeerNodeLst.size() + 1;
        struct pollfd *items = new pollfd[item_num];
        items[0].fd = m_iSocketFd;
        items[0].events = POLLIN | POLLRDHUP;
        items[0].revents = 0;
        for (int i =1; i< item_num; ++i) {
            items[i].fd = m_cPeerNodeLst[i-1]->getPeerSock();
            items[i].events = POLLIN | POLLRDHUP;
            items[i].revents = 0;
        }

        // 找出哪一个socket有数据要发送
        // Find which sockets have data to receive
        //
        long timeout = 1000; //1s frequency to poll pnode->vSend 咨询节点是否有数据要发送的频率,
        printf("NetWorkServ::SocketHandler start listen[%d]\n", item_num);

        int retCnt = poll(items, item_num, timeout);
        if ( -1 == retCnt) { 
            printf("%s---%d\n", __FILE__, __LINE__);
            break;
        }
        
		// 随机增加种子：性能计数
        RandAddSeed();
        // 对每一个socket进行服务处理
        // Service each socket
        //
        if (0 != retCnt) {
            if (items[0].revents & POLLIN) {
                CAddress cAddr;
                char* buf = sock_recvfrom(m_iSocketFd, cAddr);
                if (NULL == buf) {
                    continue;
                }
                if (0 == strcmp("ping", buf)) {
                    sock_sendto(m_iSocketFd, "pong", cAddr);
                    printf("New Node Come in\n");
                }
                else {
                    printf("Recv Data [%s]\n", buf);
                }
                free(buf);
            }
            for (int index = 1; index < item_num; ++index)
            {
                if (items[index].revents & POLLIN)
                {
                    char* buf = sock_recv(items[index].fd);
                    if (NULL == buf) {
                        continue;
                    }
                    if (0 == strcmp("ping", buf)) {
                        sock_send(items[index].fd, "pong");
                    }
                    else if (0 == strcmp("data", buf)){
                        printf("Recv Data\n");
                        m_cPeerNodeLst[index-1]->Recv();
                    }
                    free(buf);
                }
            }
        }
        else {
            // No Data Come in ,we will send all data to peer node
            // 将节点对应的发送缓冲中的内容发送出去
            // Send
            // deal send data
            printf("NetWorkServ::SocketHandler start send\n");

            foreach (auto it, m_cPeerNodeLst) {
                it->Send();
            }
        }
        delete[] items;
        items = NULL;
        sleep(10);
    }
}


// 对于每一个打开节点的链接，进行节点之间信息通信，获得节点对应的最新信息，比如节点对应的知道地址进行交换等
void NetWorkServ::OpenConnections()
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
        sleep(5);
        //printf("NetWorkServ::OpenConnections----1\n");

        vector<unsigned int> vIPC = getIPCList();     
        bool fSuccess = false;
        int nLimit = vIPC.size();
        while (!fSuccess && nLimit-- > 0)
        {
            // Choose a random class C
            unsigned int ipC = vIPC[GetRand(vIPC.size())];
            // Organize all addresses in the class C by IP
            map<unsigned int, vector<CAddress> > mapIP = selectIp(ipC);
            if (mapIP.empty())
                break;
            // Choose a random IP in the class C
            map<unsigned int, vector<CAddress> >::iterator mi = mapIP.begin();
            boost::iterators::advance_adl_barrier::advance(mi, GetRand(mapIP.size())); // 将指针定位到随机位置
            //advance(mi, GetRand(mapIP.size())); // 将指针定位到随机位置

            // Once we've chosen an IP, we'll try every given port before moving on
            foreach(const CAddress& addrConnect, (*mi).second)
            {
                printf("OpenConnection,LocalIP[%s], addrConnect_ip[%s]\n", m_cAddrLocalHost.ToStringIP().c_str(), addrConnect.ToStringIP().c_str());
				// ip不能是本地ip，且不能是非ipV4地址，对应的ip地址不在本地的节点列表中
                CheckForShutdown(1);
                if (addrConnect.ip == m_cAddrLocalHost.ip || !addrConnect.IsIPv4() || FindNode(addrConnect))
                    continue;
				// 链接对应地址信息的节点
                vfThreadRunning[1] = false;
                PeerNode* pnode = ConnectNode(addrConnect);
                if (!pnode)
                    continue;
                pnode->setNetworkState(true);

				// 如果本地主机地址能够进行路由，则需要广播我们的地址
                if (m_cAddrLocalHost.IsRoutable())
                {
                    // Advertise our address
                    vector<CAddress> vAddrToSend;
                    vAddrToSend.push_back(m_cAddrLocalHost);
                    pnode->SendAddr(vAddrToSend);// 将消息推送出去放入vsend中，在消息处理线程中进行处理
                }

				// 从创建的节点获得尽可能多的地址信息，发送消息，在消息处理线程中进行处理
                // Get as many addresses as we can
                pnode->SendGetAddrRequest();
                fSuccess = true;
                break;
            }
        }
    }
}


// 消息处理线程
void NetWorkServ::MessageHandler()
{
    printf("ThreadMessageHandler started\n");
    loop
    {
        // Poll the connected nodes for messages
        vector<PeerNode*>cNodeLstCopy = m_cPeerNodeLst;
		// 对每一个节点进行消息处理：发送消息和接收消息
        foreach(auto it, cNodeLstCopy)
        {
            it->AddRef();

            // Receive messages
            ProcessMessages(it);

            // Send messages
            SendMessages(it);

            it->Release();
        }

        // Wait and allow messages to bunch up
        sleep(10);
    }
}



/* EOF */

