/*
 * =====================================================================================
 *
 *       Filename:  ZMQNode.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/20/2018 07:38:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EN_BT_ZMQ_NODE_H
#define EN_BT_ZMQ_NODE_H
#include <mutex>
#include "ProtocSrc/Message.pb.h"
#include "CRequestTracker.h"
#include "zhelpers.h"
#include "CAddress.h"
#include "CInv.h"

namespace Enze {
class CBlockLocator;
class CTransaction;
class CBlock;

class ZNode 
{
public:
    ZNode(const string& id, void* ctx, const CAddress& addrIn, bool fInboundIn=false);
    ZNode(const string& id, void* ctx, const char* endPoint);
    ~ZNode();
      
    void Disconnect();
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
    
    void AskFor(const CInv& inv);
  
    void SendSelfAddr();
    void SendSelfInv();
    void SendGetDataReq();
    
    void SendVersion();
    void SendAddr(const vector<CAddress>& addr);
    void SendTx(const CTransaction& tx);
    void SendBlock(const CBlock& block);
    void SendGetBlocks(const CBlockLocator& local, const uint256& hash);
    void SendGetAddrRequest();
    void SendInv(const vector<CInv>& vInv, bool bReqInv); 
    bool pingNode();
    const char* getServId()const;
    
    void AddRecvMessage(PB_MessageData* pRecvData);
    bool ProcessMsg();
    void Recv();
    void Send();
   
    inline void setReleaseTime(int64 nTime)
    {
        m_nReleaseTime = nTime;
    }
    
    inline int64 getReleaseTime()const
    {
        return m_nReleaseTime;
    }
   
    inline void setNetworkState(bool bNetworkNode)
    {
        m_bNetworkNode = bNetworkNode;
    }
    
    inline void setConnecState(bool bConnect)
    {
        m_bDisconnect = bConnect;
    }
    
    inline bool isNetworkNode() const
    {
        return m_bNetworkNode;
    }
    
    inline bool isDistconnect()const 
    {
        return m_bDisconnect;
    }
    
    inline const CAddress& getAddr()const 
    {
        return m_cAddr;
    }
    
    inline void* getPeerSock() 
    {
        return m_cP2PSocket;
    
    }
    
    inline int getVesiong()const
    {
        return m_nVersion;
    }
    
    inline bool isExsitAddr(const CAddress& addr)
    {
        return m_setAddrKnown.count(addr);
    }
    
    inline void AddAddr2Send(const CAddress& addr)
    {
        m_vAddrToSend.push_back(addr); 
    }
private:
    void ProcessVerMsg(const Version& cVer);
    void ProcessAddrMsg(const Address& vAddr);
    void ProcessInvMsg(const Inventory& vInv);
    void ProcessGetDataMsg(const Inventory& vInv);
    void ProcessGetBlockMsg(const GetBlocks& cBks);
    void ProcessTxMsg(const Transaction& cTx);
    void ProcessReviewMsg(const Review& cRev);
    void ProcessSubmitOrderMsg(const Order& cOrd, map<unsigned int, vector<unsigned char> >& mapReuseKey);
    void ProcessCheckOrderMsg(const Order& cOrd, map<unsigned int, vector<unsigned char> >& mapReuseKey);
    void ProcessBlockMsg(const Block& cbk);
    void ProcessHashReplyMsg(const string& strReply);
    void ProcessGetAddrMsg(); 
    PB_MessageData* popRecvMsg();
    
private:
        bool m_bInbound;
        bool m_bNetworkNode; // 设置对应的节点为网络节点，是因为从对应的本地节点列表中没有查询到
        bool m_bDisconnect; // 端口链接的标记
        bool m_bClient;// 比较是否是客户端，如果是客户端则需要区块的头部进行校验就可以了,不需要保存整个区块的内容
        int m_nRefCount; // 使用技术器
        int m_nVersion; // 节点对应的版本，如果节点版本为0，则消息发送不出去
        int64 m_nReleaseTime; // 节点释放的时间
        uint64 m_nServices;
        CAddress m_cAddr;
        string m_Id;
        string m_EndPoint;
        string m_ServId;
        void*  m_cP2PSocket;
        list<PB_MessageData*> m_SendLst; // 发送缓存区
        list<PB_MessageData*> m_RecvLst; // 接收缓冲区
        std::mutex m_cSndMtx;
        std::mutex m_cRcvMtx;

        map<uint256, CRequestTracker> m_mapRequests;

        // flood 洪泛：地址消息的命令为addr
        vector<CAddress> m_vAddrToSend; // 消息需要发送对应的地址，对需要发送的地址进行已知地址的集合过滤之后再发送
        set<CAddress> m_setAddrKnown; // 已知地址的集合

        // inventory based relay  基于转播的库存：库存消息的命令为inv
        set<CInv> m_setInventoryKnown; // 已知库存的集合
        set<CInv> m_setInventoryKnown2;
        vector<CInv> m_vInventoryToSend; //库存准备发送的集合，对库存准备发送的集合根据已知库存的集合进行过滤之后在发送
        multimap<int64, CInv> m_mapAskFor; // 咨询请求映射，key为时间（单位到微秒）
private:
        ZNode(const ZNode&)= delete;
        ZNode() = delete;
        ZNode operator =(const ZNode&) = delete;
};




} /* End namespace */



#endif /* EN_BT_ZMQ_NODE_H */
/* EOF */

