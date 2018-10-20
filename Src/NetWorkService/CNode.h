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

#ifndef EZ_BT_CNODE_H
#define EZ_BT_CNODE_H
#include "NetWorkService/Net_DataDef.h"
#include "NetWorkService/CAddress.h"
#include "NetWorkService/CInv.h"
#include "NetWorkService/CMessageHeader.h"
#include "NetWorkService/CRequestTracker.h"
#include "ProtocSrc/Message.pb.h"
namespace Enze
{
class CBlockLocator;
class CTransaction;
class CBlock;
// 节点定义
class CNode
{

public:
    CNode(SOCKET hSocketIn, const CAddress& addrIn, bool fInboundIn=false);
    ~CNode();

    bool IsSubscribed(unsigned int nChannel);
    void Subscribe(unsigned int nChannel, unsigned int nHops=0);
    void CancelSubscribe(unsigned int nChannel);
    void Disconnect();
    void AskFor(const CInv& inv);

    // 准备释放链接
    bool ReadyToDisconnect();
    // 获取对应的应用计数
    int GetRefCount();
    // 增加对应的应用计数
    void AddRef(int64 nTimeout=0);
    // 节点释放对应，则对应的应用计数减1
    void Release();

    // 增加库存
    void AddInventoryKnown(const CInv& inv);

    // 推送库存
    void PushInventory(const CInv& inv);


    void BeginMessage(const char* pszCommand);

    void AbortMessage();
	// 修改消息头中对应的消息大小字段
    void EndMessage();

    void EndMessageAbortIfEmpty();

    const char* GetMessageCommand() const;

    void PushMessage(const char* pszCommand);

    void SendVersion(int64 nTime);
	
    void SendGetBlocks(const CBlockLocator& local, const uint256& hash);
   
    void SendAddr(const vector<CAddress>& addr);
    void SendTx(const CTransaction& tx);
    void SendBlock(const CBlock& block);
    
    // 将消息发送对应节点的vsend属性中
    template<typename T1>
    void PushMessage(const char* pszCommand, const T1& a1)
    {
        printf("error %s--%d\n", __FILE__, __LINE__);
        try
        {
            BeginMessage(pszCommand);
        //    vSend << a1;
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
        printf("error %s--%d\n", __FILE__, __LINE__);
        try
        {
            BeginMessage(pszCommand);
        //    vSend << a1 << a2;
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
        printf("error %s--%d\n", __FILE__, __LINE__);
        try
        {
            BeginMessage(pszCommand);
        //    vSend << a1 << a2 << a3;
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
        printf("error %s--%d\n", __FILE__, __LINE__);
        try
        {
            BeginMessage(pszCommand);
        //    vSend << a1 << a2 << a3 << a4;
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



protected:
    int nRefCount; // 使用技术器

public:
    // socket
    uint64 m_nServices;
    SOCKET hSocket;
    list<PB_MessageData> vSend; // 发送缓存区
    list<PB_MessageData> vRecv; // 接收缓冲区
    unsigned int nPushPos;// 指定发送区已经发送的位置
    CAddress addr;
    int nVersion; // 节点对应的版本，如果节点版本为0，则消息发送不出去
    bool fClient;// 比较是否是客户端，如果是客户端则需要区块的头部进行校验就可以了,不需要保存整个区块的内容
    bool fInbound;
    bool fNetworkNode; // 设置对应的节点为网络节点，是因为从对应的本地节点列表中没有查询到
    bool fDisconnect; // 端口链接的标记
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
private:
    CNode(const CNode&);
    void operator=(const CNode&);
};

} //end namespace

#endif /* EZ_BT_CNODE_H */

/* EOF */

