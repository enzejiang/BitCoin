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
#include "CommonBase/ProtocSerialize.h"
#include "CommonBase/market.h"
#include "ProtocSrc/Message.pb.h"
#include "NetWorkServ.h"
#include "ZMQNode.h"
#include "zhelpers.h"
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
    
   // std::thread t1(ThreadSocketHandler);
    std::thread t1(&NetWorkServ::SocketHandler, this);
    t1.detach();
    
   // std::thread t2(ThreadMessageHandler);
    std::thread t2(&NetWorkServ::MessageHandler, this);
    t2.detach();
    
    std::thread t3(&NetWorkServ::OpenConnections, this);
    t3.detach();
    
    std::thread t4(&NetWorkServ::MessageRecv, this);
    t4.detach();
    
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
            foreach(ZNode* pnode, m_cZNodeLst)
                close(pnode->hSocket);
    }
#endif 
}


void NetWorkServ::NodeSyncThread()
{
    printf("%s---%d\n", __FILE__, __LINE__);
    printf("NetWorkServ::NodeSyncThread--start\n");
    void* worker = zmq_socket(m_cZmqCtx, ZMQ_ROUTER);
    zmq_connect(worker, "inproc://backend.ipc");
    while(1) 
    {
        char* brokerId = s_recv(worker);
        char* repId  = s_recv(worker);
        char*emp = s_recv(worker);
        free(emp);
        char*ReqType = s_recv(worker);
        printf("Recv Data, brokerId[%s], repId[%s]\n", brokerId, repId);
        printf("Recv Data, ReqType[%s]\n", ReqType);
        if (0 == strcmp("ping", ReqType)) {;
            s_sendmore(worker, brokerId);
            s_sendmore(worker, repId);
            s_send(worker, (char*)m_strIdentity.c_str()); 
            printf("Recv Data, ReqType[%s]--rep\n", ReqType);
#if 0
            if (!FindNode(repId)) {
                printf("Recv Data, ReqType not Find[%s]\n", repId);
                ConnectNode(repId);
            }
#endif
        }
        else {
            char* pData = s_recv(worker);
            ZNode *pNode = FindNode(repId);
            if (!pNode) {
                pNode = ConnectNode(repId);
            }
            if (!pNode) {
                printf("NetWorkServ::NodeSyncThread--Node[%s] is not alive\n", repId);
                free(pData);
                continue;
            }
            string req = pData;
            PB_MessageData *cProtoc = new PB_MessageData;
            cProtoc->ParsePartialFromString(req);
            pNode->AddRecvMessage(cProtoc);
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
        free(brokerId);
        free(repId);
        //free(ReqType);
        free(ReqType);
        sleep(1);
    }
    

}


void NetWorkServ::MessageRecv()
{
    
    void* backend = zmq_socket(m_cZmqCtx, ZMQ_DEALER);
    zmq_setsockopt(backend, ZMQ_IDENTITY, m_strIdentity.c_str(), m_strIdentity.length());
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

// socket 处理，parag对应的是本地节点开启的监听socket
void NetWorkServ::SocketHandler()
{
    printf("ThreadSocketHandler started\n");
    list<ZNode*> cZNodeLstDisconnected;
    loop
    {
        // Disconnect duplicate connections 释放同一个ip重复链接对应的节点，可能是不同端口
        map<unsigned int, ZNode*> mapFirst;
        foreach(auto it, m_cZNodeLst)
        {
            ZNode* pnode= it.second;
            if (pnode->isDistconnect())
                continue;
            unsigned int ip = pnode->getAddr().ip;
            // 本地主机ip地址对应的是0，所以所有的ip地址都应该大于这个ip
            if (mapFirst.count(ip) && m_cAddrLocalHost.ip < ip)
            {
                // In case two nodes connect to each other at once,
                // the lower ip disconnects its outbound connection
                ZNode* pnodeExtra = mapFirst[ip];

                if (pnodeExtra->GetRefCount() > (pnodeExtra->isNetworkNode() ? 1 : 0))
                    std::swap(pnodeExtra, pnode);

                if (pnodeExtra->GetRefCount() <= (pnodeExtra->isNetworkNode() ? 1 : 0))
                {
                    printf("(%d nodes) disconnecting duplicate: %s\n", m_cZNodeLst.size(), pnodeExtra->getAddr().ToString().c_str());
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
        map<string, ZNode*> cZNodeLstCopy = m_cZNodeLst;
        foreach(auto it, cZNodeLstCopy)
        {
            ZNode* pnode= it.second;
            // 节点准备释放链接，并且对应的接收和发送缓存区都是空
            if (pnode->ReadyToDisconnect() /*&& pnode->vRecv.empty() && pnode->vSend.empty()*/)
            {
                // 从节点列表中移除
                // remove from m_cZNodeLst
                //m_cZNodeLst.erase(remove(m_cZNodeLst.begin(), m_cZNodeLst.end(), it), m_cZNodeLst.end());
                printf("Remove Node[%s] From List\n",it.first.c_str());
                m_cZNodeLst.erase(it.first);
                pnode->Disconnect();

                // 将对应准备释放的节点放在对应的节点释放链接池中，等待对应节点的所有引用释放
                // hold in disconnected pool until all refs are released
                pnode->setReleaseTime(max(pnode->getReleaseTime(), GetTime() + 5 * 60)); // 向后推迟5分钟
                if (pnode->isNetworkNode())
                    pnode->Release();
                cZNodeLstDisconnected.push_back(pnode);
            }
        }

        // 删除端口的链接的节点：删除的条件是对应节点的引用小于等于0
        // Delete disconnected nodes
        // map<string, ZNode*> cZNodeLstDisconnectedCopy = cZNodeLstDisconnected;
        foreach(ZNode* pnode, cZNodeLstDisconnected)
        {
            // wait until threads are done using it
            if (pnode->GetRefCount() <= 0)
            {
                cZNodeLstDisconnected.remove(pnode);
                delete pnode;
            }
        }

        if (m_cZNodeLst.size() == 0)
        {
           sleep(10);
           continue;
        }

        zmq_pollitem_t *items = new zmq_pollitem_t[m_cZNodeLst.size()];
        int iCnt = 0;
        foreach(auto it, m_cZNodeLst) {
            items[iCnt].socket = it.second->getPeerSock();
            items[iCnt].fd = 0;
            items[iCnt].events = ZMQ_POLLIN;
            items[iCnt].revents = 0;
        }

        // 找出哪一个socket有数据要发送
        // Find which sockets have data to receive
        //
        long timeout = 1000; //1s frequency to poll pnode->vSend 咨询节点是否有数据要发送的频率,
	    
        int retCnt = zmq_poll(items, m_cZNodeLst.size(), timeout);
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
            for (int i = 0; i < m_cZNodeLst.size(); ++i) {
                if (items[i].revents & ZMQ_POLLIN) {
                    //zmq::socket_t* pSock = items[i].socket;
                  //  string emp = s_recv(items[i].socket);
                    string identity = s_recv(items[i].socket);
                    auto it = m_cZNodeLst.find(identity);
                    if (it != m_cZNodeLst.end()) {
                        printf("[%s] Start recv\n", identity.c_str());
                        it->second->Recv();
                    }else {
                       printf("%s---%d---%s,error[Node %s was not find]\n", __FILE__, __LINE__, __func__, identity.c_str());
                       foreach (auto it, m_cZNodeLst) {
                           printf("Node id [%s]\n", it.first.c_str());
                       }
                    }
                }
            }
        }
        else {
            // No Data Come in ,we will send all data to peer node
            // 将节点对应的发送缓冲中的内容发送出去
            // Send
            // deal send data
            printf("NetWorkServ::SocketHandler start send\n");

            foreach (auto it, m_cZNodeLst) {
                it.second->Send();
            }
        }
        delete[] items;
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
        vfThreadRunning[1] = false;
        sleep(5);
        while (m_cZNodeLst.size() >= nMaxConnections || m_cZNodeLst.size() >= m_cMapAddresses.size())
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
        else if (nTry++ < 30 && m_cZNodeLst.size() < nMaxConnections/2)
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
            int64 nDelay = ((30 * 60) << m_cZNodeLst.size());
            if (!fIRCOnly)
            {
                nDelay *= 2;
                if (m_cZNodeLst.size() >= 3)
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
            boost::iterators::advance_adl_barrier::advance(mi, GetRand(mapIP.size())); // 将指针定位到随机位置
            //advance(mi, GetRand(mapIP.size())); // 将指针定位到随机位置

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
                ZNode* pnode = ConnectNode(addrConnect);
                vfThreadRunning[1] = true;
                CheckForShutdown(1);
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

                ////// should the one on the receiving end do this too?
                // Subscribe our local subscription list
				// 新建的节点要订阅我们本地主机订阅的对应通断
#if 0
                const unsigned int nHops = 0;
                for (unsigned int nChannel = 0; nChannel < m_pcNodeLocalHost->vfSubscribe.size(); nChannel++)
                    if (m_pcNodeLocalHost->vfSubscribe[nChannel])
                        pnode->PushMessage("subscribe", nChannel, nHops);
#endif 
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
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    loop
    {
		// 轮询链接的节点用于消息处理
        // Poll the connected nodes for messages
        map<string, ZNode*>cNodeLstCopy = m_cZNodeLst;
		// 对每一个节点进行消息处理：发送消息和接收消息
        foreach(auto it, cNodeLstCopy)
        {
            ZNode* pnode = it.second;
            pnode->AddRef();

            // Receive messages
            ProcessMessages(pnode);

            // Send messages
            SendMessages(pnode);

            pnode->Release();
        }

        // Wait and allow messages to bunch up
        sleep(10);
    }
}



/* EOF */

