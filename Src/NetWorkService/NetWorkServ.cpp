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


void NetWorkServ::AddNewAddrByEndPoint(const char* endPoint)
{
    if (NULL == endPoint) return;
    
    // endPoint format is [tcp://ip:port]
    int headLen = strlen("tcp://");
    if (0 != strncmp(endPoint, "tcp://", headLen)) 
        return;
    
    CAddress addr(endPoint+headLen, NODE_NETWORK);
    // 根据地址的ip+port来查询对应的内存对象m_cMapAddresses，
    map<string, CAddress>::iterator it = m_cMapAddresses.find(addr.GetKey());
    if (it == m_cMapAddresses.end())
    {
        printf("NetWorkServ::AddNewAddrByEndPoint--add-into db\n");
        // New address
        m_cMapAddresses.insert(make_pair(addr.GetKey(), addr));
        DaoServ::getInstance()->WriteAddress(addr);
    }
    else
    {
        CAddress& addrFound = (*it).second;
        if ((addrFound.m_nServices | addr.m_nServices) != addrFound.m_nServices)
        {
              printf("NetWorkServ::AddNewAddrByEndPoint--update-into db\n");
            // 增加地址对应的服务类型，并将其写入数据库
            // Services have been added
            addrFound.m_nServices |= addr.m_nServices;
            DaoServ::getInstance()->WriteAddress(addrFound);
        }
    }
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

// 根据ip在本地存储的节点列表m_cZNodeLst中查找对应的节点
ZNode* NetWorkServ::FindNode(unsigned int ip)
{
    foreach(auto it, m_cZNodeLst)
    {
        ZNode* pnode = it.second;
        if (pnode->getAddr().ip == ip)
            return (pnode);
    }


    return NULL;
}

ZNode* NetWorkServ::FindNode(const CAddress& addr)
{

    foreach(auto it, m_cZNodeLst)
    {
        ZNode* pnode = it.second;
        if (pnode->getAddr() == addr)
            return (pnode);
    }


    return NULL;
}

ZNode* NetWorkServ::FindNode(const char* endPoint)
{
    ZNode* pNode = NULL;
    if (endPoint)
    {
        auto it = m_cZNodeLst.find(endPoint);
        if (it != m_cZNodeLst.end()) {
            pNode = it->second;
        }
    }
    
    return pNode;
}

ZNode* NetWorkServ::ConnectNode(const char* endPoint)
{
    ZNode* pNode = NULL;
    if (endPoint) {
        CAddress cAddr(endPoint+4);
        pNode = FindNode(endPoint);
        if (!pNode) {
            pNode = new ZNode(m_strIdentity, m_cZmqCtx, endPoint);
            if (pNode->pingNode())
            {
                pNode->AddRef();
                pNode->SendVersion();
                m_cZNodeLst.insert(pair<string, ZNode*>(pNode->getServId(),pNode));

                m_cMapAddresses[cAddr.GetKey()].nLastFailed = 0;
            } 
            else {
                delete pNode;
                m_cMapAddresses[cAddr.GetKey()].nLastFailed = GetTime();
            
            }
        }
        else {
            if (!pNode->pingNode()) {
                pNode->Disconnect();
                m_cMapAddresses[cAddr.GetKey()].nLastFailed = GetTime();
                return NULL;
            }
        }
        
    } 
    
    return pNode;

}

// 链接对应地址的节点
ZNode* NetWorkServ::ConnectNode(const CAddress& addrConnect, int64 nTimeout)
{
    if (addrConnect.ip == m_cAddrLocalHost.ip)
        return NULL;

	// 使用ip地址在本地对应的节点列表中查找对应的节点，如果存在则直接返回对应的节点
    // Look for an existing connection
    ZNode* pnode = FindNode(addrConnect.ip);
    if (pnode)
    {
        if (pnode->pingNode()) {
            if (nTimeout != 0)
                pnode->AddRef(nTimeout); // 推迟节点对应的释放时间
            else
                pnode->AddRef(); // 增加节点对应的引用
            return pnode; 
        }
        else {
            pnode->Disconnect();
            m_cMapAddresses[addrConnect.GetKey()].nLastFailed = GetTime(); 
            return NULL;
        }
    }

    /// debug print

	// 对请求的地址进行连接
    // Connect
    pnode = new ZNode(m_strIdentity, m_cZmqCtx, addrConnect, false);
    if (pnode->pingNode())
    {
        if (nTimeout != 0)
            pnode->AddRef(nTimeout);
        else
            pnode->AddRef();

        pnode->SendVersion();
        m_cZNodeLst.insert(pair<string, ZNode*>(pnode->getServId(),pnode));

        m_cMapAddresses[addrConnect.GetKey()].nLastFailed = 0;
        return pnode;
    }

    delete pnode;
    m_cMapAddresses[addrConnect.GetKey()].nLastFailed = GetTime();
    return NULL;

}

// 处理单个节点对应的消息：单个节点接收到的消息进行处理
bool NetWorkServ::ProcessMessages(ZNode* pfrom)
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
bool NetWorkServ::SendMessages(ZNode* pto)
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
    foreach(ZNode* pnode, m_cZNodeLst)
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
    foreach(ZNode* pnode, m_cZNodeLst)
        if (pnode->IsSubscribed(nChannel))
            return true;
    return false;
#endif 
}

bool NetWorkServ::LoadAddresses()
{
    DaoServ::getInstance()->LoadAddresses(m_cMapAddresses);
    AddUserProviedAddress();
}

NetWorkServ::NetWorkServ()
: m_cAddrLocalHost(0, DEFAULT_PORT, m_nLocalServices)
{
    m_cZmqCtx = zmq_ctx_new();
    ZNode* m_pcNodeLocalHost = new ZNode(" ", m_cZmqCtx, CAddress("127.0.0.1", m_nLocalServices)); // 本地节点

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

