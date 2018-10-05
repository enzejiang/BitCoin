/*
 * =====================================================================================
 *
 *       Filename:  CNode.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 08:19:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_BT_CNODE_H
#define CXX_BT_CNODE_H
#include "BlockEngine.h"
#include "Network/Net_DataDef.h"
#include "Network/CAddress.h"
#include "Network/CInv.h"
#include "Network/CMessageHeader.h"
#include "Network/CRequestTracker.h"
extern map<CInv, int64> mapAlreadyAskedFor;

// 节点定义
class CNode
{
public:
    // socket
    uint64 m_nServices;
    SOCKET hSocket;
    CDataStream vSend; // 发送缓存区
    CDataStream vRecv; // 接收缓冲区
    unsigned int nPushPos;// 指定发送区已经发送的位置
    CAddress addr;
    int nVersion; // 节点对应的版本，如果节点版本为0，则消息发送不出去
    bool fClient;// 比较是否是客户端，如果是客户端则需要区块的头部进行校验就可以了,不需要保存整个区块的内容
    bool fInbound;
    bool fNetworkNode; // 设置对应的节点为网络节点，是因为从对应的本地节点列表中没有查询到
    bool fDisconnect; // 端口链接的标记
protected:
    int nRefCount; // 使用技术器
public:
    int64 nReleaseTime; // 节点释放的时间
    map<uint256, CRequestTracker> mapRequests;
   // CCriticalSection cs_mapRequests;

    // flood 洪泛：地址消息的命令为addr
    vector<CAddress> vAddrToSend; // 消息需要发送对应的地址，对需要发送的地址进行已知地址的集合过滤之后再发送
    set<CAddress> setAddrKnown; // 已知地址的集合

    // inventory based relay  基于转播的库存：库存消息的命令为inv
    set<CInv> setInventoryKnown; // 已知库存的集合
    set<CInv> setInventoryKnown2;
    vector<CInv> vInventoryToSend; //库存准备发送的集合，对库存准备发送的集合根据已知库存的集合进行过滤之后在发送
   // CCriticalSection cs_inventory;
    multimap<int64, CInv> mapAskFor; // 咨询请求映射，key为时间（单位到微秒）

    // publish and subscription
    vector<char> vfSubscribe;


    CNode(SOCKET hSocketIn, const CAddress& addrIn, bool fInboundIn=false)
    {
        m_nServices = 0;
        hSocket = hSocketIn;
        vSend.SetType(SER_NETWORK);
        vRecv.SetType(SER_NETWORK);
        nPushPos = -1;
        addr = addrIn;
        nVersion = 0;
        fClient = false; // set by version message
        fInbound = fInboundIn;
        fNetworkNode = false;
        fDisconnect = false;
        nRefCount = 0;
        nReleaseTime = 0;
        vfSubscribe.assign(256, false);

        // Push a version message
        /// when NTP implemented, change to just nTime = GetAdjustedTime()
        int64 nTime = (fInbound ? GetAdjustedTime() : GetTime());
		// 创建节点的时候会发送节点版本的消息：消息命令为version,后面是消息发送的内容
        PushMessage("version", VERSION, nLocalServices, nTime, addr);
    }

    ~CNode()
    {
        if (hSocket != INVALID_SOCKET)
            close(hSocket);
    }

private:
    CNode(const CNode&);
    void operator=(const CNode&);
public:

    // 准备释放链接
    bool ReadyToDisconnect()
    {
        return fDisconnect || GetRefCount() <= 0;
    }
    // 获取对应的应用计数
    int GetRefCount()
    {
        return max(nRefCount, 0) + (GetTime() < nReleaseTime ? 1 : 0);
    }
    // 增加对应的应用计数
    void AddRef(int64 nTimeout=0)
    {
        if (nTimeout != 0)
            nReleaseTime = max(nReleaseTime, GetTime() + nTimeout); // 推迟节点对应的释放时间
        else
            nRefCount++;
    }
    // 节点释放对应，则对应的应用计数减1
    void Release()
    {
        nRefCount--;
    }


    // 增加库存
    void AddInventoryKnown(const CInv& inv)
    {
            setInventoryKnown.insert(inv);
    }

    // 推送库存
    void PushInventory(const CInv& inv)
    {
            if (!setInventoryKnown.count(inv))
                vInventoryToSend.push_back(inv);
    }

    void AskFor(const CInv& inv)
    {
        // We're using mapAskFor as a priority queue, 优先级队列
        // the key is the earliest time the request can be sent （key对应的是请求最早被发送的时间）
        int64& nRequestTime = mapAlreadyAskedFor[inv];
        printf("askfor %s  %I64d\n", inv.ToString().c_str(), nRequestTime);

		// 确保不要时间索引让事情在同一个顺序
        // Make sure not to reuse time indexes to keep things in the same order
        int64 nNow = (GetTime() - 1) * 1000000; // 单位到微秒
        static int64 nLastTime;
        nLastTime = nNow = max(nNow, ++nLastTime);//如果调用很快的话，可以保证对应的nlastTime++是的对应的时间不一样

        // Each retry is 2 minutes after the last，没有到2分钟，则对应的nRequesttime对应的值都一样
        nRequestTime = max(nRequestTime + 2 * 60 * 1000000, nNow);
        mapAskFor.insert(make_pair(nRequestTime, inv));
    }



    void BeginMessage(const char* pszCommand)
    {
        //EnterCriticalSection(&cs_vSend);
        if (nPushPos != -1)
            AbortMessage();
        nPushPos = vSend.size();
        vSend << CMessageHeader(pszCommand, 0);
        printf("sending: %-12s ", pszCommand);
    }

    void AbortMessage()
    {
        if (nPushPos == -1)
            return;
        vSend.resize(nPushPos);
        nPushPos = -1;
       // LeaveCriticalSection(&cs_vSend);
        printf("(aborted)\n");
    }
	// 修改消息头中对应的消息大小字段
    void EndMessage()
    {
        int nDropMessagesTest = BlockEngine::getInstance()->nDropMessagesTest;
        if (nDropMessagesTest > 0 && GetRand(nDropMessagesTest) == 0)
        {
            printf("dropmessages DROPPING SEND MESSAGE\n");
            AbortMessage();
            return;
        }

        if (nPushPos == -1)
            return;

		// 修改消息头中对应的消息大小
        // Patch in the size
        unsigned int nSize = vSend.size() - nPushPos - sizeof(CMessageHeader);
        memcpy((char*)&vSend[nPushPos] + offsetof(CMessageHeader, m_nMessageSize), &nSize, sizeof(nSize));

        printf("(%d bytes)  ", nSize);
        //for (int i = nPushPos+sizeof(CMessageHeader); i < min(vSend.size(), nPushPos+sizeof(CMessageHeader)+20U); i++)
        //    printf("%02x ", vSend[i] & 0xff);
        printf("\n");

        nPushPos = -1;
    }

    void EndMessageAbortIfEmpty()
    {
        if (nPushPos == -1)
            return;
        int nSize = vSend.size() - nPushPos - sizeof(CMessageHeader);
        if (nSize > 0)
            EndMessage();
        else
            AbortMessage();
    }

    const char* GetMessageCommand() const
    {
        if (nPushPos == -1)
            return "";
        return &vSend[nPushPos] + offsetof(CMessageHeader, m_pchCommand);
    }




    void PushMessage(const char* pszCommand)
    {
        try
        {
            BeginMessage(pszCommand);
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

	// 将消息发送对应节点的vsend属性中
    template<typename T1>
    void PushMessage(const char* pszCommand, const T1& a1)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template<typename T1, typename T2>
    void PushMessage(const char* pszCommand, const T1& a1, const T2& a2)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template<typename T1, typename T2, typename T3>
    void PushMessage(const char* pszCommand, const T1& a1, const T2& a2, const T3& a3)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template<typename T1, typename T2, typename T3, typename T4>
    void PushMessage(const char* pszCommand, const T1& a1, const T2& a2, const T3& a3, const T4& a4)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }


    void PushRequest(const char* pszCommand,
                     void (*fn)(void*, CDataStream&), void* param1)
    {
        uint256 hashReply;
        RAND_bytes((unsigned char*)&hashReply, sizeof(hashReply));

       // CRITICAL_BLOCK(cs_mapRequests)
            mapRequests[hashReply] = CRequestTracker(fn, param1);

        PushMessage(pszCommand, hashReply);
    }

    template<typename T1>
    void PushRequest(const char* pszCommand, const T1& a1,
                     void (*fn)(void*, CDataStream&), void* param1)
    {
        uint256 hashReply;
        RAND_bytes((unsigned char*)&hashReply, sizeof(hashReply));

       // CRITICAL_BLOCK(cs_mapRequests)
            mapRequests[hashReply] = CRequestTracker(fn, param1);

        PushMessage(pszCommand, hashReply, a1);
    }

    template<typename T1, typename T2>
    void PushRequest(const char* pszCommand, const T1& a1, const T2& a2,
                     void (*fn)(void*, CDataStream&), void* param1)
    {
        uint256 hashReply;
        RAND_bytes((unsigned char*)&hashReply, sizeof(hashReply));

       // CRITICAL_BLOCK(cs_mapRequests)
            mapRequests[hashReply] = CRequestTracker(fn, param1);

        PushMessage(pszCommand, hashReply, a1, a2);
    }



    bool IsSubscribed(unsigned int nChannel);
    void Subscribe(unsigned int nChannel, unsigned int nHops=0);
    void CancelSubscribe(unsigned int nChannel);
    void Disconnect();
};


#endif /* CXX_BT_CNODE_H */

/* EOF */

