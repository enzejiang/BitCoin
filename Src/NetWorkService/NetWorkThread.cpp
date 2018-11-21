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
	// 启动线程
    //
    // Start threads
    //
     printf("start Node\n");

    std::thread t1(&NetWorkServ::AddrManagerThread, this);
    t1.detach();
    
    std::thread t2(&NetWorkServ::PeerNodeManagerThread, this);
    t2.detach();

    std::thread t3(&NetWorkServ::MessageHandler, this);
    t3.detach();
    
    std::thread t4(&NetWorkServ::SocketHandler, this);
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
    char* servIp = "192.168.2.206";
    string servEndPoint = string("tcp://")+string(servIp)+string(":6667");

    
    struct sockaddr_in servAddr;
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(servIp);
    servAddr.sin_port = htons(6666);

    CAddress addr(servAddr);
    m_iSocketFd = udp_socket(m_cAddrLocalHost, addr, false);

    void* server = zmq_socket(m_cZmqCtx, ZMQ_DEALER);
    zmq_setsockopt(server, ZMQ_IDENTITY, m_strIdentity.c_str(), m_strIdentity.length());
    if (0 != zmq_connect(server, servEndPoint.c_str())) {
        printf("%s---%d, zmq_connect error %m\n", __FILE__, __LINE__);
        return;
    } 
    
    s_sendmore(server, "");
    s_send(server, "getAddr");
    printf("star recv from, EndPoint[%s]\n", servEndPoint.c_str());

    struct sockaddr_in selfAddr;
    socklen_t len = sizeof(selfAddr);
    bzero(&selfAddr, len);
    getsockname(m_iSocketFd, (struct sockaddr*)&selfAddr, &len);
    struct {
        unsigned int ip;
        short port;
    } ep;
    ep.ip = selfAddr.sin_addr.s_addr;
    ep.port = selfAddr.sin_port;

    printf("AddrManager self ip [%s] port[%d]\n", inet_ntoa(selfAddr.sin_addr), ntohs(ep.port));
   // sock_sendto(m_iSocketFd,"ping", addr);
    sendto(m_iSocketFd, &ep, sizeof(ep), 0, (struct sockaddr*)&servAddr, len);
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
            printf("start recv from emp\n");
            char* emp = s_recv(server);
            free(emp);
            printf("Recv Data start\n");
            char* pData = s_recv(server);
            printf("Recv Data End\n");
            

            Enze::ServMsg cMsg;
            cMsg.ParsePartialFromString(pData);
            AddAddress(cMsg);
            free(pData);
        }
        
        if (items[1].revents & ZMQ_POLLIN) {
        
        }
     
    }

}


void NetWorkServ::SocketHandler()
{
    printf("ThreadSocketHandler started\n");
    void* server = zmq_socket(m_cZmqCtx, ZMQ_DEALER);
    zmq_setsockopt(server, ZMQ_IDENTITY, "SocketHandler", strlen("SocketHandler"));
    if (0 != zmq_connect(server, "inproc://PeerNodeManagerThread.ipc")) {
        printf("%s---%d, SocketHandler,zmq_connect error %m\n", __FILE__, __LINE__);
        return;
    }
    loop
    {
        zmq_pollitem_t items[] = {{0, m_iSocketFd, ZMQ_POLLIN, 0}};

        // Find which sockets have data to receive
        //
        long timeout = 1000;
        int retCnt = zmq_poll(items, 1, timeout);
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
                char* dataType = sock_recvfrom(m_iSocketFd, cAddr);
                if (NULL == dataType) {
                    continue;
                }
                s_sendmore(server, "");
                s_sendmore(server, dataType);
                if (0 == strcmp("ping", dataType) || 0 == strcmp("pong", dataType)) {
                    s_send(server,(char*)cAddr.ToString().c_str());
                }
                else if (0 == strcmp("data", dataType)){
                    printf("Recv Data\n");
                    s_sendmore(server,(char*)cAddr.ToString().c_str());
                    char* buf = sock_recvfrom(m_iSocketFd, cAddr);
                    s_send(server, buf);
                    free(buf);
                }
                else {
                    printf("unknown Recv Data [%s]\n", dataType);
                }
                free(dataType);
            }
        }
        sleep(10);
    }
}


void NetWorkServ::PeerNodeManagerThread()
{
    const int nMaxConnections = 15;
    void* PeerNodeManagerSock = zmq_socket(m_cZmqCtx, ZMQ_ROUTER);
    zmq_setsockopt(PeerNodeManagerSock, ZMQ_IDENTITY, "PeerNodeManagerThread", strlen("PeerNodeManagerThread"));
    zmq_bind(PeerNodeManagerSock, "inproc://PeerNodeManagerThread.ipc");
    loop 
    {
        if (m_cPeerNodeLst.size() < nMaxConnections) {
            AddrConvertPeerNode();
        }
        
        DealPeerNodeMsg(PeerNodeManagerSock);
        
        UpdatePeerNodeStatu();
        
        sleep(10);

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

