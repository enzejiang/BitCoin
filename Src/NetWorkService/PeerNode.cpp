/*
 * =====================================================================================
 *
 *       Filename:  PeerNode.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2018 03:36:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <map>
#include "CommonBase/ProtocSerialize.h"
#include "CommonBase/market.h"
#include "BlockEngine/CBlockLocator.h"
#include "BlockEngine/BlockEngine.h"
#include "DAO/DaoServ.h"
#include "WalletService/WalletServ.h"
#include "PeerNode.h"
#include "NetWorkServ.h"
#include "SocketWraper.h"
using namespace std;
using namespace Enze;

PeerNode::PeerNode(const CAddress& addrIn, int socketFd, bool fInboundIn)
: m_bInbound(fInboundIn),
  m_cAddr(addrIn),
  m_peerFd(socketFd),
  m_SendLst(),
  m_RecvLst(),
  m_cSndMtx(),
  m_cRcvMtx()
{

}


PeerNode::~PeerNode()
{
    foreach(PB_MessageData* pRecv, m_RecvLst)
    {
        delete pRecv;
    }
    m_RecvLst.clear();
    foreach(PB_MessageData*pSend, m_SendLst)
    {
        delete pSend;
    }
    m_SendLst.clear();
}

bool PeerNode::pingNode()
{
    int ret = sock_sendto(m_peerFd, "ping", m_cAddr);// ping the server 
    if (0 > ret) {
        printf("%s---%d, Error[%m]\n", __FILE__, __LINE__);
        return false;
    }    
    return true;
}

bool PeerNode::repPong()
{
    int ret = sock_sendto(m_peerFd, "pong", m_cAddr);// ping the server 
    if (0 > ret) {
        printf("%s---%d, Error[%m]\n", __FILE__, __LINE__);
        return false;
    }    
    return true;
}

void PeerNode::Disconnect()
{
    printf("disconnecting node %s\n",  m_cAddr.GetKey().c_str());

    map<string, CAddress>& mapAddresses = Enze::NetWorkServ::getInstance()->getMapAddr();
    // If outbound and never got version message, mark address as failed
    if (!m_bInbound)
        mapAddresses[m_cAddr.GetKey()].nLastFailed = GetTime();

    m_bDisconnect = true;
    // All of a nodes broadcasts and subscriptions are automatically torn down
    // when it goes down, so a node has to stay up to keep its broadcast going.

  /*  for (std::map<uint256, CProduct>::iterator mi = mapProducts.begin(); mi != mapProducts.end();)
        Enze::NetWorkServ::getInstance()->AdvertRemoveSource(this, MSG_PRODUCT, 0, (*(mi++)).second);
    */
}
// 准备释放链接
bool PeerNode::ReadyToDisconnect()
{
    return m_bDisconnect || GetRefCount() <= 0;
}

// 获取对应的应用计数
int PeerNode::GetRefCount()
{
    return max(m_nRefCount, 0) + (GetTime() < m_nReleaseTime ? 1 : 0);
}

// 增加对应的应用计数
void PeerNode::AddRef(int64 nTimeout)
{
    if (nTimeout != 0)
        m_nReleaseTime = max(m_nReleaseTime, GetTime() + nTimeout); // 推迟节点对应的释放时间
    else
        m_nRefCount++;

}

// 节点释放对应，则对应的应用计数减1
void PeerNode::Release()
{
    --m_nRefCount;
}

void PeerNode::SendVersion()
{
    printf("PeerNode::SendVersion\n");
    int64 nTime = (m_bInbound ? GetAdjustedTime() : GetTime());
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_Version);
    Version* pVersion = cProtoc->mutable_cversion();
    pVersion->set_utime(nTime);
    pVersion->set_nversion(VERSION);
    pVersion->set_nservices(NODE_NETWORK);
    ::SeriaAddress(m_cAddr, *pVersion->mutable_addrme());
    
    string strData;
    cProtoc->SerializePartialToString(&strData);
    sock_sendto(m_peerFd, "data", m_cAddr);
    sock_sendto(m_peerFd, strData.c_str(), m_cAddr);
    delete cProtoc;

}

void PeerNode::SendGetBlocks(const CBlockLocator& local, const uint256& hash)
{
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_GetBlocks);
    GetBlocks* pGetBlock = cProtoc->mutable_getblocks();
    pGetBlock->set_hashstop(hash.ToString());
    GetBlocks::BlockLocator* pLoc = pGetBlock->mutable_locator();
    foreach(auto it, local.vHave)
    {
        pLoc->add_vhave(it.ToString());
    }
    
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    m_SendLst.push_back(cProtoc);
}

void PeerNode::SendAddr(const vector<CAddress>& addr)
{
    if (0 == addr.size()) return; 
    
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_Addr);
    foreach(auto it, addr) 
    {
        ::SeriaAddress(it, *cProtoc->add_vaddr()); 
    }
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    m_SendLst.push_back(cProtoc);
}

void PeerNode::SendTx(const CTransaction& tx)
{
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_Tx);
    ::SeriaTransaction(tx, *cProtoc->mutable_tx());
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    m_SendLst.push_back(cProtoc);
}
    
void PeerNode::SendBlock(const CBlock& block)
{
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_Block);
    ::SeriaBlock(block, *cProtoc->mutable_cblock());
    
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    m_SendLst.push_back(cProtoc);
}

void PeerNode::SendGetAddrRequest()
{
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_GetAddr);
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    m_SendLst.push_back(cProtoc);
}
    
void PeerNode::SendInv(const vector<CInv>& vInv, bool bReqInv)
{
    PB_MessageData *cProtoc = new PB_MessageData();
    if (!bReqInv)
        cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_Inv);
    else 
        cProtoc->set_emsgkind(PB_MessageData_Mesg_Kind_MK_GetData);
    foreach(auto it, vInv) {
        SeriaInv(it, *(cProtoc->add_vinv()));
    }

    printf("PeerNode::SendInv,DataSize[%d],%s---%d\n", cProtoc->ByteSizeLong(),__FILE__, __LINE__);
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    m_SendLst.push_back(cProtoc);
}

// 增加库存
void PeerNode::AddInventoryKnown(const CInv& inv)
{
    m_setInventoryKnown.insert(inv);
}

// 推送库存
void PeerNode::PushInventory(const CInv& inv)
{
    if (!m_setInventoryKnown.count(inv))
        m_vInventoryToSend.push_back(inv);
}

void PeerNode::AskFor(const CInv& inv)
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
    m_mapAskFor.insert(make_pair(nRequestTime, inv));
}




void PeerNode::AddRecvMessage(PB_MessageData* pRecvData)
{
    std::lock_guard<std::mutex> guard(m_cRcvMtx);
    m_RecvLst.push_back(pRecvData);
}

void PeerNode::Recv(const char* pData)
{
    if (NULL == pData) return;
    std::lock_guard<std::mutex> guard(m_cRcvMtx);
    PB_MessageData *cProtoc = new PB_MessageData();
    cProtoc->ParsePartialFromString(pData);
    m_RecvLst.push_back(cProtoc);
}

void PeerNode::Send()
{
    if (!pingNode()) {
        printf("PeerNode::Send--Serv[%s] is offline\n", m_cAddr.GetKey().c_str());
        m_bDisconnect = true;
        return;
    }

    if (0 == m_nVersion) {
        SendVersion();
        printf("Do not Get peer Node Version Data ,will not send data to it\n");
        return ;
    }
    std::lock_guard<std::mutex> guard(m_cSndMtx);
    while(!m_SendLst.empty()) {
        PB_MessageData* pData = m_SendLst.front();
        string strData;
        pData->SerializePartialToString(&strData);
        //s_sendmore(socket, m_EndPoint.c_str());
        sock_sendto(m_peerFd, "data", m_cAddr);
        sock_sendto(m_peerFd, strData.c_str(), m_cAddr);
        m_SendLst.pop_front();
        delete pData;
    }
}

bool PeerNode::ProcessMsg()
{
    printf("PeerNode::ProcessMsg()---start\n");
    PB_MessageData* pData = popRecvMsg();
   
    bool bFinish = (!pData);
    if (!bFinish) {
        printf("PeerNode::ProcessMsg()---recv Data\n");
        PB_MessageData_Mesg_Kind eMsgKind = pData->emsgkind();
        static map<unsigned int, vector<unsigned char> > mapReuseKey;
        printf("PeerNode::ProcessMsg()---recv Data Kind[%d]\n", eMsgKind);
#if 0
        // 消息采集频率进行处理
        if (nDropMessagesTest > 0 && GetRand(nDropMessagesTest) == 0)
        {
            printf("dropmessages DROPPING RECV MESSAGE\n");
            return true;
        }
#endif 
        switch(eMsgKind) {
            // 如果命令是版本：节点对应的版本
            case PB_MessageData_Mesg_Kind_MK_Version:
                    ProcessVerMsg(pData->cversion());
                    break;

            case PB_MessageData_Mesg_Kind_MK_Addr:
                    foreach(auto it, pData->vaddr()) {
                        ProcessAddrMsg(it); 
                    }
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_Inv:
                    foreach(auto it, pData->vinv())
                        ProcessInvMsg(it); 
                    break;

            case PB_MessageData_Mesg_Kind_MK_GetData:
                    foreach(auto it, pData->vinv())
                        ProcessGetDataMsg(it);  
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_GetBlocks:
                    ProcessGetBlockMsg(pData->getblocks());
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_Tx:
                    ProcessTxMsg(pData->tx());
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_Review:
                    ProcessReviewMsg(pData->creview());
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_Block:
                    ProcessBlockMsg(pData->cblock());
                    break;

            case PB_MessageData_Mesg_Kind_MK_GetAddr:
                    ProcessGetAddrMsg(); 
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_CheckOrder:
                    ProcessCheckOrderMsg(pData->corder(), mapReuseKey);
                    break;
                    
            case PB_MessageData_Mesg_Kind_MK_SubmitOrder:
                    ProcessSubmitOrderMsg(pData->corder(), mapReuseKey);
                    break;
            
            case PB_MessageData_Mesg_Kind_MK_Reply:
                    ProcessHashReplyMsg(pData->hashreply());
                    break;
        
        }

    }   
    printf("PeerNode::ProcessMsg()---end\n");
    delete pData;
    pData = NULL;
    return bFinish;
}

void PeerNode::ProcessVerMsg(const Version& cVer)
{
    // 节点对应的版本只能更新一次，初始为0，后面进行更新
    // Can only do this once
    if (m_nVersion != 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("ProcessVerMsg Can only do this once,: %s---%d\n", __FILE__, __LINE__);
        return ;
    }

    int64 uTime;
    CAddress addrMe; // 读取消息对应的内容
    m_nVersion = cVer.nversion();
    m_nServices = cVer.nservices();
    uTime = cVer.utime();
    ::UnSeriaAddress(cVer.addrme(), addrMe);
    if (m_nVersion == 0) {
        printf("PeerNode::ProcessVerMsg Recv Version[%d]\n", m_nVersion);
        return ;
    }
    // 更新发送和接收缓冲区中的对应的版本
    //pfrom->vSend.SetVersion(min(pfrom->nVersion, VERSION));
    //pfrom->vRecv.SetVersion(min(pfrom->nVersion, VERSION));

    // 如果节点对应的服务类型是节点网络，则对应节点的客户端标记就是false
    m_bClient = !(m_nServices & NODE_NETWORK);
    if (m_bClient)
    {
        printf("error %s_%d\n", __FILE__, __LINE__);
        // 如果不是节点网络，可能仅仅是一些节点不要保存对应的完整区块信息，仅仅需要区块的头部进行校验就可以了
        //pfrom->vSend.nType |= SER_BLOCKHEADERONLY;
        //pfrom->vRecv.nType |= SER_BLOCKHEADERONLY;
    }
    // 增加时间样本数据：没有什么用处，仅仅用于输出
    AddTimeData(m_cAddr.ip, uTime);

    // 对第一个进来的节点请求block信息
    // Ask the first connected node for block updates
    static bool fAskedForBlocks;
    if (!fAskedForBlocks && m_bClient)
    {
        fAskedForBlocks = true;
        SendGetBlocks(CBlockLocator(BlockEngine::getInstance()->getBestIndex()), uint256(0));
    }

    printf("version message: %s has version %d, addrMe=%s\n", m_cAddr.ToString().c_str(), m_nVersion, addrMe.ToString().c_str());

}

void PeerNode::ProcessAddrMsg(const Address& pbAddr)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    
    CAddress addr;
    ::UnSeriaAddress(pbAddr, addr);
    // Store the new addresses
    // 将地址增加到数据库中
    printf("error %s---%d\n", __FILE__, __LINE__);

    if (DaoServ::getInstance()->WriteAddress(addr))
    {
        // Put on lists to send to other nodes
        m_setAddrKnown.insert(addr); // 将对应的地址插入到已知地址集合中
        vector<PeerNode*>& cPeerNodeLst = NetWorkServ::getInstance()->getNodeList();
        foreach(auto it, cPeerNodeLst) 
        {
            if (!it->isExsitAddr(addr))
                it->AddAddr2Send(addr);// 地址的广播
        }
    }

}

void PeerNode::ProcessInvMsg(const Inventory& pbInv)
{
    printf("PeerNode::ProcessInvMsg()---start\n");
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    
    CInv inv;
    UnSeriaInv(pbInv, inv);
    AddInventoryKnown(inv); // 将对应的库存发送消息增加到库存发送已知中

    bool fAlreadyHave = WalletServ::getInstance()->AlreadyHave(inv);
    printf("  got inventory: %s  %s\n", inv.ToString().c_str(), fAlreadyHave ? "have" : "new");

    if (!fAlreadyHave)
        AskFor(inv);// 如果不存在，则请求咨询，这里会在线程中发送getdata消息
    else if (inv.type == MSG_BLOCK && BlockEngine::getInstance()->mapOrphanBlocks.count(inv.hash))
        SendGetBlocks(CBlockLocator(BlockEngine::getInstance()->getBestIndex()), BlockEngine::getInstance()->GetOrphanRoot(BlockEngine::getInstance()->mapOrphanBlocks[inv.hash]));

}

void PeerNode::ProcessGetDataMsg(const Inventory& pbInv)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    CInv inv;
    UnSeriaInv(pbInv, inv);

    printf("received getdata for: %s\n", inv.ToString().c_str());

    if (inv.type == MSG_BLOCK)
    {
        // Send block from disk
        map<uint256, CBlockIndex*>::const_iterator mi = BlockEngine::getInstance()->getMapBlockIndex().find(inv.hash);
        if (mi != BlockEngine::getInstance()->getMapBlockIndex().end())
        {
            //// could optimize this to send header straight from blockindex for client
            CBlock block;
            block.ReadFromDisk((*mi).second, !m_bClient);
            SendBlock(block);// 获取数据对应的类型是block，则发送对应的块信息
        }
    }
    else if (inv.IsKnownType())
    {
        printf("error %s---%d\n", __FILE__, __LINE__);
        // Send stream from relay memory
#if 0
        map<CInv, CDataStream>::iterator mi = mapRelay.find(inv); // 重新转播的内容
        if (mi != mapRelay.end())
            pfrom->PushMessage(inv.GetCommand(), (*mi).second);
#endif
    }

}

void PeerNode::ProcessGetBlockMsg(const GetBlocks& cBks)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    
    CBlockLocator locator;
    UnSeriaLoctor(cBks.locator(), locator);
    uint256 hashStop(cBks.hashstop());

    //找到本地有的且在主链上的
    // Find the first block the caller has in the main chain
    CBlockIndex* pindex = const_cast<CBlockIndex*>(locator.GetBlockIndex());

    // 将匹配得到的块索引之后的所有在主链上的块发送出去
    // Send the rest of the chain
    if (pindex)
        pindex = pindex->m_pNextBlkIndex;
    printf("getblocks %d to %s\n", (pindex ? pindex->m_nCurHeight : -1), hashStop.ToString().substr(0,14).c_str());
    for (; pindex; pindex = pindex->m_pNextBlkIndex)
    {
        if (pindex->GetBlockHash() == hashStop)
        {
            printf("  getblocks stopping at %d %s\n", pindex->m_nCurHeight, pindex->GetBlockHash().ToString().substr(0,14).c_str());
            break;
        }

        // Bypass setInventoryKnown in case an inventory message got lost
        CInv inv(MSG_BLOCK, pindex->GetBlockHash());
        // 判断在已知库存2中是否存在
        // returns true if wasn't already contained in the set
        if (m_setInventoryKnown2.insert(inv).second)
        {
            m_setInventoryKnown.erase(inv);
            m_vInventoryToSend.push_back(inv);// 插入对应的库存发送集合中准备发送，在另一个线程中进行发送，发送的消息为inv
        }
    }

}

void PeerNode::ProcessTxMsg(const Transaction& pbTx)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    vector<uint256> vWorkQueue;
    CTransaction tx;
    UnSeriaTransaction(pbTx, tx);

    CInv inv(MSG_TX, tx.GetHash());
    AddInventoryKnown(inv);// 将交易消息放入到对应的已知库存中

    bool fMissingInputs = false;
    // 如果交易能够被接受
    if (tx.AcceptTransaction(true, &fMissingInputs))
    {
        WalletServ::getInstance()->AddToWalletIfMine(tx, NULL);
        // RelayMessage(inv, vMsg);// 转播消息 need define the function
        NetWorkServ::getInstance()->removeAlreadyAskedFor(inv);
        vWorkQueue.push_back(inv.hash);

        // 递归处理所有依赖这个交易对应的孤儿交易
        // Recursively process any orphan transactions that depended on this one
        for (int i = 0; i < vWorkQueue.size(); i++)
        {
            printf("error:%s---%d\n", __FILE__, __LINE__);
            uint256 hashPrev = vWorkQueue[i];
            for (multimap<uint256, CTransaction*>::iterator mi = WalletServ::getInstance()->mapOrphanTransactionsByPrev.lower_bound(hashPrev);
                 mi != WalletServ::getInstance()->mapOrphanTransactionsByPrev.upper_bound(hashPrev);
                 ++mi)
            {
                CTransaction tx = *((*mi).second);
                CInv inv(MSG_TX, tx.GetHash());

                if (tx.AcceptTransaction(true))
                {
                    printf("   accepted orphan tx %s\n", inv.hash.ToString().substr(0,6).c_str());
                    WalletServ::getInstance()->AddToWalletIfMine(tx, NULL);
                //    RelayMessage(inv, vMsg);
                    NetWorkServ::getInstance()->removeAlreadyAskedFor(inv);
                    vWorkQueue.push_back(inv.hash);
                }
            }
        }

        foreach(uint256 hash, vWorkQueue)
            WalletServ::getInstance()->EraseOrphanTx(hash);
    }
    else if (fMissingInputs)
    {
        printf("storing orphan tx %s\n", inv.hash.ToString().substr(0,6).c_str());
        WalletServ::getInstance()->AddOrphanTx(pbTx); // 如果交易当前不被接受则对应的孤儿交易
    }

}

void PeerNode::ProcessReviewMsg(const Review& cRev)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    CReview review;
    UnSeriaReview(cRev, review);

    CInv inv(MSG_REVIEW, review.GetHash());
    AddInventoryKnown(inv);

    if (review.AcceptReview())
    {
        // Relay the original message as-is in case it's a higher version than we know how to parse
        printf("%s---%d\n", __FILE__, __LINE__);
        //RelayMessage(inv, vMsg);
        NetWorkServ::getInstance()->removeAlreadyAskedFor(inv);
    }

}


void PeerNode::ProcessBlockMsg(const Block& cbk)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    
    auto_ptr<CBlock> pblock(new CBlock);
    UnSeriaBlock(cbk, *pblock);

    //// debug print
    printf("received block:\n"); pblock->print();

    CInv inv(MSG_BLOCK, pblock->GetHash());
    AddInventoryKnown(inv);// 增加库存

    printf("error %s---%d\n", __FILE__, __LINE__);

    if (BlockEngine::getInstance()->ProcessBlock(this, pblock.release()))
        NetWorkServ::getInstance()->removeAlreadyAskedFor(inv);

}

void PeerNode::ProcessHashReplyMsg(const string& strReply)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    
    uint256 hashReply(strReply);

    CRequestTracker tracker;
    map<uint256, CRequestTracker>::iterator mi = m_mapRequests.find(hashReply);
    if (mi != m_mapRequests.end())
    {
        tracker = (*mi).second;
        m_mapRequests.erase(mi);
    }
    if (!tracker.IsNull()) {
        printf("%s---%d\n", __FILE__, __LINE__);
        //tracker.fn(tracker.param1, vRecv);
    
    }

}

void PeerNode::ProcessGetAddrMsg()
{

    m_vAddrToSend.clear();
    int64 nSince = GetAdjustedTime() - 5 * 24 * 60 * 60; // in the last 5 days
    map<string, CAddress>& cMapAddr = NetWorkServ::getInstance()->getMapAddr();
    unsigned int nSize = cMapAddr.size();
    foreach(const PAIRTYPE(string, CAddress)& item, cMapAddr)
    {
        const CAddress& addr = item.second;
        //// will need this if we lose IRC
        //if (addr.nTime > nSince || (rand() % nSize) < 500)
        if (addr.nTime > nSince)
            m_vAddrToSend.push_back(addr);
    }
}


void PeerNode::ProcessSubmitOrderMsg(const Order& cOrd, map<unsigned int, vector<unsigned char> >& mapReuseKey)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    uint256 hashReply(cOrd.hashreply());
    CWalletTx wtxNew;
    UnSeriaWalletTx(cOrd.txorder(), wtxNew);
   // vRecv >> hashReply >> wtxNew;

    // Broadcast
    if (!wtxNew.AcceptWalletTransaction())
    {
        printf("%s---%d\n", __FILE__, __LINE__);
        //PushMessage("reply", hashReply, (int)1);
        error("submitorder AcceptWalletTransaction() failed, returning error 1");
        return ;
    }
    wtxNew.m_bTimeReceivedIsTxTime = true;
    WalletServ::getInstance()->AddToWallet(wtxNew);
    wtxNew.RelayWalletTransaction();
    mapReuseKey.erase(m_cAddr.ip);

    // Send back confirmation
    printf("%s---%d\n", __FILE__, __LINE__);
    //PushMessage("reply", hashReply, (int)0);

}

void PeerNode::ProcessCheckOrderMsg(const Order& cOrd, map<unsigned int, vector<unsigned char> >& mapReuseKey)
{
    if (m_nVersion == 0)
    {
        // 节点在处理任何消息之前一定有一个版本消息
        // Must have a version message before anything else
        printf("Must have a version message before anything: %s---%d\n", __FILE__, __LINE__);
        return ;
    }
    uint256 hashReply(cOrd.hashreply());
    CWalletTx order;
    UnSeriaWalletTx(cOrd.txorder(), order);

    /// we have a chance to check the order here

    // Keep giving the same key to the same ip until they use it
    if (!mapReuseKey.count(m_cAddr.ip))
        mapReuseKey[m_cAddr.ip] = WalletServ::getInstance()->GenerateNewKey();

    // Send back approval of order and pubkey to use
    CScript scriptPubKey;
    scriptPubKey << mapReuseKey[m_cAddr.ip] << OP_CHECKSIG;
    printf("%s---%d\n", __FILE__, __LINE__);
    //PushMessage("reply", hashReply, (int)0, scriptPubKey);

}

PB_MessageData* PeerNode::popRecvMsg()
{
    PB_MessageData* pData = NULL;
    std::lock_guard<std::mutex> guard(m_cRcvMtx);
    if (!m_RecvLst.empty()) {
        pData = m_RecvLst.front();
        m_RecvLst.pop_front();
    }
    
    return pData;
}

void PeerNode::SendSelfAddr()
{
    // 消息发送的地址
    // Message: addr
    //
    vector<CAddress> vAddrToSend;
    vAddrToSend.reserve(m_vAddrToSend.size());
    // 如果发送的地址不在已知地址的集合中，则将其放入临时地址发送数组中
    foreach(const CAddress& addr, m_vAddrToSend)
        if (!m_setAddrKnown.count(addr))
            vAddrToSend.push_back(addr);
    // 清空地址发送的数组
    m_vAddrToSend.clear();
    // 如果临时地址发送数组不为空，则进行地址的消息的发送
    if (!vAddrToSend.empty())
        SendAddr(vAddrToSend);


}

void PeerNode::SendSelfInv()
{
    // 库存消息处理
    // Message: inventory
    //
    vector<CInv> vInventoryToSend;
    vInventoryToSend.reserve(m_vInventoryToSend.size());
    foreach(const CInv& inv, m_vInventoryToSend)
    {
        // returns true if wasn't already contained in the set
        if (m_setInventoryKnown.insert(inv).second)
            vInventoryToSend.push_back(inv);
    }
    m_vInventoryToSend.clear();
    m_setInventoryKnown2.clear();
    // 库存消息发送
    if (!vInventoryToSend.empty())
        SendInv(vInventoryToSend, false);

}

void PeerNode::SendGetDataReq()
{
    // getdata消息发送
    // Message: getdata
    //
    vector<CInv> vAskFor;
    int64 nNow = GetTime() * 1000000;
    // 判断节点对应的请求消息map是否为空，且对应的请求map中的消息对应的最早请求时间是否小于当前时间
    while (!m_mapAskFor.empty() && (*m_mapAskFor.begin()).first <= nNow)
    {
        const CInv& inv = (*m_mapAskFor.begin()).second;
        printf("sending getdata: %s\n", inv.ToString().c_str());
        if (!WalletServ::getInstance()->AlreadyHave(inv))
            vAskFor.push_back(inv);// 不存在才需要进行消息发送
        m_mapAskFor.erase(m_mapAskFor.begin());// 请求消息处理完一条就删除一条
    }
    if (!vAskFor.empty())
        SendInv(vAskFor, true); //send get Data

}

/* EOF */

