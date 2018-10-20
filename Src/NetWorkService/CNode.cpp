/*
 * =====================================================================================
 *
 *       Filename:  CNode.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/06/2018 11:44:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "CommonBase/market.h"
#include "CommonBase/ProtocSerialize.h"
#include "BlockEngine/CBlockLocator.h"
#include "BlockEngine/CBlock.h"
#include "WalletService/CTransaction.h"
#include "CNode.h"
#include "NetWorkServ.h"
using namespace Enze;

CNode::CNode(SOCKET hSocketIn, const CAddress& addrIn, bool fInboundIn)
{
    m_nServices = 0;
    hSocket = hSocketIn;
    vSend.clear();
    vRecv.clear();
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
    //PushMessage("version", VERSION, NODE_NETWORK, nTime, addr);
    SendVersion(nTime);
}

CNode::~CNode()
{
    if (hSocket != INVALID_SOCKET)
        close(hSocket);
}

void CNode::SendVersion(int64 nTime)
{
    PB_MessageData cProtoc;
    cProtoc.set_emsgkind(PB_MessageData_Mesg_Kind_MK_Version);
    Version* pVersion = cProtoc.mutable_cversion();
    pVersion->set_utime(nTime);
    pVersion->set_nversion(VERSION);
    pVersion->set_nservices(NODE_NETWORK);
    ::SeriaAddress(addr, *pVersion->mutable_addrme());
    printf("%s---%d\n", __FILE__, __LINE__);

}

void CNode::SendGetBlocks(const CBlockLocator& local, const uint256& hash)
{
    PB_MessageData cProtoc;
    cProtoc.set_emsgkind(PB_MessageData_Mesg_Kind_MK_GetBlocks);
    GetBlocks* pGetBlock = cProtoc.mutable_getblocks();
    pGetBlock->set_hashstop(hash.ToString());
    GetBlocks::BlockLocator* pLoc = pGetBlock->mutable_locator();
    foreach(auto it, local.vHave)
    {
        pLoc->add_vhave(it.ToString());
    }
    printf("%s---%d\n", __FILE__, __LINE__);
}

void CNode::SendAddr(const vector<CAddress>& addr)
{
    if (0 == addr.size()); 
    PB_MessageData cProtoc;
    cProtoc.set_emsgkind(PB_MessageData_Mesg_Kind_MK_Addr);
    foreach(auto it, addr) 
    {
        ::SeriaAddress(it, *cProtoc.add_vaddr()); 
    }
    printf("%s---%d\n", __FILE__, __LINE__);
}

void CNode::SendTx(const CTransaction& tx)
{
    PB_MessageData cProtoc;
    cProtoc.set_emsgkind(PB_MessageData_Mesg_Kind_MK_Tx);
    printf("%s---%d\n", __FILE__, __LINE__);
    ::SeriaTransaction(tx, *cProtoc.mutable_tx());
}
    
void CNode::SendBlock(const CBlock& block)
{
    PB_MessageData cProtoc;
    cProtoc.set_emsgkind(PB_MessageData_Mesg_Kind_MK_Block);
    ::SeriaBlock(block, *cProtoc.mutable_cblock());
    printf("%s---%d\n", __FILE__, __LINE__);
}

// 准备释放链接
bool CNode::ReadyToDisconnect()
{
    return fDisconnect || GetRefCount() <= 0;
}
// 获取对应的应用计数
int CNode::GetRefCount()
{
    return max(nRefCount, 0) + (GetTime() < nReleaseTime ? 1 : 0);
}
// 增加对应的应用计数
void CNode::AddRef(int64 nTimeout)
{
    if (nTimeout != 0)
        nReleaseTime = max(nReleaseTime, GetTime() + nTimeout); // 推迟节点对应的释放时间
    else
        nRefCount++;
}
// 节点释放对应，则对应的应用计数减1
void CNode::Release()
{
    nRefCount--;
}


// 增加库存
void CNode::AddInventoryKnown(const CInv& inv)
{
        setInventoryKnown.insert(inv);
}

// 推送库存
void CNode::PushInventory(const CInv& inv)
{
        if (!setInventoryKnown.count(inv))
            vInventoryToSend.push_back(inv);
}


void CNode::BeginMessage(const char* pszCommand)
{
    //EnterCriticalSection(&cs_vSend);
    if (nPushPos != -1)
        AbortMessage();
    nPushPos = vSend.size();
    printf("error %s--%d\n", __FILE__, __LINE__);
    //vSend << CMessageHeader(pszCommand, 0);
    printf("sending: %-12s ", pszCommand);
}

void CNode::AbortMessage()
{
    if (nPushPos == -1)
        return;
    vSend.resize(nPushPos);
    nPushPos = -1;
   // LeaveCriticalSection(&cs_vSend);
    printf("(aborted)\n");
}
// 修改消息头中对应的消息大小字段
void CNode::EndMessage()
{
    printf("error %s--%d\n", __FILE__, __LINE__);
    int nDropMessagesTest = 0;// BlockEngine::getInstance()->nDropMessagesTest;
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
    printf("error %s--%d\n", __FILE__, __LINE__);
    //memcpy((char*)&vSend[nPushPos] + offsetof(CMessageHeader, m_nMessageSize), &nSize, sizeof(nSize));

    printf("(%d bytes)  ", nSize);
    //for (int i = nPushPos+sizeof(CMessageHeader); i < min(vSend.size(), nPushPos+sizeof(CMessageHeader)+20U); i++)
    //    printf("%02x ", vSend[i] & 0xff);
    printf("\n");

    nPushPos = -1;
}

void CNode::EndMessageAbortIfEmpty()
{
    printf("error %s--%d\n", __FILE__, __LINE__);
    if (nPushPos == -1)
        return;
    int nSize = vSend.size() - nPushPos - sizeof(CMessageHeader);
    if (nSize > 0)
        EndMessage();
    else
        AbortMessage();
}

const char* CNode::GetMessageCommand() const
{
    printf("error %s--%d\n", __FILE__, __LINE__);
    if (nPushPos == -1)
        return "";
    return 0;
   // return &vSend[nPushPos] + offsetof(CMessageHeader, m_pchCommand);
}




void CNode::PushMessage(const char* pszCommand)
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


//
// Subscription methods for the broadcast and subscription system.
// Channel numbers are message numbers, i.e. MSG_TABLE and MSG_PRODUCT.
//
// The subscription system uses a meet-in-the-middle strategy.
// With 100,000 nodes, if senders broadcast to 1000 random nodes and receivers
// subscribe to 1000 random nodes, 99.995% (1 - 0.99^1000) of messages will get through.
//


bool CNode::IsSubscribed(unsigned int nChannel)
{
    if (nChannel >= vfSubscribe.size())
        return false;
    return vfSubscribe[nChannel];
}
#if 0
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
#endif 

void CNode::Disconnect()
{
    printf("disconnecting node %s\n", addr.ToString().c_str());

    close(hSocket);
    map<string, CAddress>& mapAddresses = Enze::NetWorkServ::getInstance()->getMapAddr();
    // If outbound and never got version message, mark address as failed
    if (!fInbound && nVersion == 0)
        mapAddresses[addr.GetKey()].nLastFailed = GetTime();

    // All of a nodes broadcasts and subscriptions are automatically torn down
    // when it goes down, so a node has to stay up to keep its broadcast going.

    for (map<uint256, CProduct>::iterator mi = mapProducts.begin(); mi != mapProducts.end();)
        Enze::NetWorkServ::getInstance()->AdvertRemoveSource(this, MSG_PRODUCT, 0, (*(mi++)).second);
#if 0
    // Cancel subscriptions
    for (unsigned int nChannel = 0; nChannel < vfSubscribe.size(); nChannel++)
        if (vfSubscribe[nChannel])
            CancelSubscribe(nChannel);
#endif 
}

void CNode::AskFor(const CInv& inv)
{
    // We're using mapAskFor as a priority queue, 优先级队列
    // the key is the earliest time the request can be sent （key对应的是请求最早被发送的时间）
    int64& nRequestTime = Enze::NetWorkServ::getInstance()->getMapAlreadyAskedFor()[inv];
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

/* EOF */

