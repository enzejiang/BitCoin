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
#include "ProtocSrc/ServerMessage.pb.h"
#include "NetWorkServ.h"
#include "SocketWraper.h"
#include "CInv.h"
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
        printf("GetMyExternalIP() received %s\n", Buf);
        char BufIP[32] = {0};
        sscanf(strstr(Buf,"utf-8")+9,"%*[^\n]\n%[^\n]",BufIP);
        printf("GetMyExternalIP() received IP %s\n", BufIP);
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

// 根据ip在本地存储的节点列表m_cPeerNodeLst中查找对应的节点
PeerNode* NetWorkServ::FindNode(unsigned int ip)
{
    foreach(auto it, m_cPeerNodeLst)
    {
        if (it->getAddr().ip == ip)
            return it;
    }


    return NULL;
}

PeerNode* NetWorkServ::FindNode(const CAddress& addr)
{

    foreach(auto it, m_cPeerNodeLst)
    {
        if (it->getAddr() == addr)
            return (it);
    }
    return NULL;
}

// 链接对应地址的节点
PeerNode* NetWorkServ::ConnectNode(const CAddress& addrConnect, int64 nTimeout)
{
    printf("NetWorkServ::ConnectNode, addrConnect_ip[%s]--start\n", addrConnect.ToStringIPPort().c_str());

    if (addrConnect.ip == m_cAddrLocalHost.ip)
        return NULL;

	// 使用ip地址在本地对应的节点列表中查找对应的节点，如果存在则直接返回对应的节点
    // Look for an existing connection
    PeerNode* pnode = FindNode(addrConnect);
    if (pnode)
    {
        if (nTimeout != 0)
            pnode->AddRef(nTimeout); // 推迟节点对应的释放时间
        else
            pnode->AddRef(); // 增加节点对应的引用
        return pnode;
    }

    /// debug print
    printf("NetWorkServ::ConnectNode, addrConnect_ip[%s], new Node\n", addrConnect.ToStringIPPort().c_str());

	// 对请求的地址进行连接
    // Connect
    pnode = new PeerNode(addrConnect, m_iSocketFd,false);
    if (pnode->pingNode())
    {
        if (nTimeout != 0)
            pnode->AddRef(nTimeout);
        else
            pnode->AddRef();

        pnode->SendVersion();
        pnode->setLastActiveTime(GetTime());
        m_cPeerNodeLst.push_back(pnode);

        m_cMapAddresses[addrConnect.GetKey()].nLastFailed = 0;
        return pnode;
    }

    delete pnode;
    m_cMapAddresses[addrConnect.GetKey()].nLastFailed = GetTime();
    return NULL;

}

// 处理单个节点对应的消息：单个节点接收到的消息进行处理
bool NetWorkServ::ProcessMessages(PeerNode* pfrom)
{

    if (!pfrom) true;
    // 同一个的消息格式
    // Message format
    //  (4) message start
    //  (12) command
    //  (4) size
    //  (x) data
    //
	// 消息头包含：message start;command;size;
    bool bFinish = false;
    while(!bFinish)
    {
        bFinish = pfrom->ProcessMsg();
    }
    return true;
}



// 处理节点对应的消息发送
bool NetWorkServ::SendMessages(PeerNode* pto)
{

    printf("%s---%d\n", __FILE__, __LINE__);
    // Don't send anything until we get their version message
    if (pto->getVesiong() == 0)
        return true;

    pto->SendSelfAddr();

    pto->SendSelfInv();
    
    pto->SendGetDataReq();

    return true;
}


void NetWorkServ::AbandonRequests(/*void (*fn)(void*, CDataStream&), void* param1*/)
{
    printf("%s---%d\n", __FILE__, __LINE__);
#if 0
    // If the dialog might get closed before the reply comes back,
    // call this in the destructor so it doesn't get called after it's deleted.
    foreach(PeerNode* pnode, m_cPeerNodeLst)
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
#endif 
}

bool NetWorkServ::AnySubscribed(unsigned int nChannel)
{
    printf("%s---%d\n", __FILE__, __LINE__);
#if 0
    if (m_pcNodeLocalHost->IsSubscribed(nChannel))
        return true;
    foreach(PeerNode* pnode, m_cPeerNodeLst)
        if (pnode->IsSubscribed(nChannel))
            return true;
    return false;
#endif 
}

bool NetWorkServ::LoadAddresses()
{
    std::lock_guard<std::mutex> guard(m_cAddrMutex);
    DaoServ::getInstance()->LoadAddresses(m_cMapAddresses);
    AddUserProviedAddress();
}

void NetWorkServ::AddAddress(const ServMsg& cMsg)
{
     std::lock_guard<std::mutex> guard(m_cAddrMutex);
     foreach(auto ep, cMsg.eplist()) {
        CAddress addr(ep.ip(), ep.port());
        // 根据地址的ip+port来查询对应的内存对象m_cMapAddresses，
        map<string, CAddress>::iterator it = m_cMapAddresses.find(addr.GetKey());
        if (it == m_cMapAddresses.end())
        {
            printf("NetWorkServ::AddAddress(const ServMsg&)--add-into db\n");
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
                  printf("NetWorkServ::AddAddress(const ServMsg&)--update-into db\n");
                // 增加地址对应的服务类型，并将其写入数据库
                // Services have been added
                addrFound.m_nServices |= addr.m_nServices;
                DaoServ::getInstance()->WriteAddress(addrFound);
            }
            continue;
        }
     }
}
    
vector<unsigned int> NetWorkServ:: getIPCList()
{
    std::lock_guard<std::mutex> guard(m_cAddrMutex);
    // Ip对应的C类地址，相同的C类地址放在一起
    // Make a list of unique class C's
    unsigned char pchIPCMask[4] = { 0xff, 0xff, 0xff, 0x00 };
    unsigned int nIPCMask = *(unsigned int*)pchIPCMask;
    vector<unsigned int> vIPC;
    vIPC.reserve(m_cMapAddresses.size());
    unsigned int nPrev = 0;
    // mapAddress已经进行排序了，默认是生效排序
    foreach(const PAIRTYPE(string, CAddress)& item, m_cMapAddresses)
    {
        const CAddress& addr = item.second;
        if (!addr.IsIPv4())
            continue;
        // Taking advantage of m_cMapAddresses being in sorted order,
        // with IPs of the same class C grouped together.
        unsigned int ipC = addr.ip & nIPCMask;
        if (ipC != nPrev)
            vIPC.push_back(nPrev = ipC);
    }

    return vIPC;
}

map<unsigned int, vector<CAddress> > NetWorkServ::selectIp(unsigned int ipC)
{

    // IP选择的过程
    // The IP selection process is designed to limit vulnerability致命性 to address flooding.
    // Any class C (a.b.c.?) has an equal chance of being chosen, then an IP is
    // chosen within the class C.  An attacker may be able to allocate many IPs, but
    // they would normally be concentrated in blocks of class C's.  They can hog独占 the
    // attention within their class C, but not the whole IP address space overall.
    // A lone node in a class C will get as much attention as someone holding all 255
    // IPs in another class C.
    /*
    map::lower_bound(key):返回map中第一个大于或等于key的迭代器指针
    map::upper_bound(key):返回map中第一个大于key的迭代器指针
    */
    std::lock_guard<std::mutex> guard(m_cAddrMutex);
    map<unsigned int, vector<CAddress> > mapIP;
    unsigned char pchIPCMask[4] = { 0xff, 0xff, 0xff, 0x00 };
    unsigned int nIPCMask = *(unsigned int*)pchIPCMask;
    for (map<string, CAddress>::const_iterator mi = m_cMapAddresses.lower_bound(CAddress(ipC, 0).GetKey());
         mi != m_cMapAddresses.upper_bound(CAddress(ipC | ~nIPCMask, 0xffff).GetKey());
         ++mi)
    {
        
//           printf("NetWorkServ::OpenConnections----6\n");
        const CAddress& addr = (*mi).second;
        //if (fIRCOnly && !mapIRCAddresses.count((*mi).first))
        //    continue;

        // 当前时间 - 地址连接最新失败的时间 要大于对应节点重连的间隔时间
        if (GetTime() - addr.nLastFailed > 12)
            mapIP[addr.ip].push_back(addr); //同一个地址区段不同地址： 同一个地址的不同端口，所有对应同一个ip会有多个地址
    }

    return mapIP;
}

void NetWorkServ::AddrConvertPeerNode()
{
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
            if (addrConnect.ip == m_cAddrLocalHost.ip || !addrConnect.IsIPv4() || FindNode(addrConnect))
                continue;
            // 链接对应地址信息的节点
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

void NetWorkServ::DealPeerNodeMsg(void* zmqSock)
{
    if (NULL == zmqSock) return;
    long timeout = 1000 * 5; // 5s
    zmq_pollitem_t items[] = {{zmqSock, 0, ZMQ_POLLIN, 0}};
    int retCnt = zmq_poll(items, 1, timeout);
    if ( -1 == retCnt) {
        printf("%s---%d\n", __FILE__, __LINE__);
        return;
    }

    if (0 != retCnt) {
        if (items[0].revents & ZMQ_POLLIN) {
            char* buf = s_recv(zmqSock);
            printf("DealPeerNode Msg ---1[%s]\n", buf);
            char* Straddr = s_recv(zmqSock);
            printf("DealPeerNode Msg ---2[%s]\n", Straddr);
            CAddress cAddr(Straddr);
            if (NULL == buf) {
                return;
            }
            PeerNode* pNode = FindNode(cAddr);
            if (!pNode) {
                pNode=ConnectNode(cAddr);
                printf("New Node Come in addr[%s], Straddr[%s]\n", cAddr.ToString().c_str(), Straddr);
            }
            pNode->AddRef();
            pNode->setLastActiveTime(GetTime());
            if (0 == strcmp("ping", buf)) {
                pNode->repPong();
            }
            else if (0 == strcmp("data", buf)){
                printf("DealPeerNode[%s]Recv Data\n", Straddr);
                char* pData = s_recv(zmqSock);
                pNode->Recv(pData);
                free(pData);
            }
            else {
                printf("Recv Data [%s]\n", buf);
            }
            pNode->Release();
            free(buf);
            free(Straddr);
        }
    }

}

void NetWorkServ::UpdatePeerNodeStatu()
{
    // Disconnect duplicate connections 释放同一个ip重复链接对应的节点，可能是不同端口
    list<PeerNode*> cPeerNodeLstDisconnected;
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
                    pnode->Release();
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
        PeerNode* pnode = it;
        int64 curTime = GetTime();
        // 节点准备释放链接，并且对应的接收和发送缓存区都是空
        const uint64 heartBeatTime = 2 *60; // every 2 Minutes Send ping
        const uint64 OffLineTime = 10*60; // if 10 Minutes has no data come in, we think the node is offline, ready to disconnect this Node;
        const uint64 DeleteNodeTime = 15*60; // if 15 Minutes  has no data come in, to delete this Node
        const uint64 LastActiveTime = pnode->getLastActiveTime();
        const uint64 DiffTime = LastActiveTime - curTime;
        if (DiffTime > DeleteNodeTime) {
            if (pnode->ReadyToDisconnect())
            {
                // remove from m_cPeerNodeLst
                m_cPeerNodeLst.erase(remove(m_cPeerNodeLst.begin(), m_cPeerNodeLst.end(), it), m_cPeerNodeLst.end());
                // hold in disconnected pool until all refs are released
                cPeerNodeLstDisconnected.push_back(pnode);
            }
            else {
                pnode->Disconnect();
            }
        }
        else if (DiffTime > OffLineTime) {
             pnode->Disconnect();
             pnode->setReleaseTime(curTime + 5 * 60); // Delay by five minutes, just for deal the msg in recvbuf
             if (pnode->isNetworkNode())
                 pnode->Release();
        }
        else if (DiffTime > heartBeatTime){
            pnode->pingNode();
        }
    }
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
    

}

NetWorkServ::NetWorkServ()
: m_cAddrLocalHost(0, DEFAULT_PORT, m_nLocalServices)
{
    m_cZmqCtx = zmq_ctx_new();
    //PeerNode* m_pcNodeLocalHost = new PeerNode(" ", m_cZmqCtx, CAddress("127.0.0.1", m_nLocalServices)); // 本地节点

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

