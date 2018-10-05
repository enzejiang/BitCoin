// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef CXX_BT_NET_H
#define CXX_BT_NET_H
#include <arpa/inet.h>
#include "main.h"
#include "Network/CNode.h"
#include "Network/irc.h"
class CAddrDB;


bool ConnectSocket(const CAddress& addrConnect, SOCKET& hSocketRet);
bool GetMyExternalIP(unsigned int& ipRet);
bool AddAddress(CAddrDB& addrdb, const CAddress& addr);
CNode* FindNode(unsigned int ip);
CNode* ConnectNode(CAddress addrConnect, int64 nTimeout=0);
void AbandonRequests(void (*fn)(void*, CDataStream&), void* param1);
bool AnySubscribed(unsigned int nChannel);
void* ThreadBitcoinMiner(void* parg);
bool StartNode(string& strError=REF(string()));
bool StopNode();
void CheckForShutdown(int n);


extern CAddress addrLocalHost;
extern CNode* pnodeLocalHost;
extern vector<CNode*> vNodes;
extern map<vector<unsigned char>, CAddress> mapAddresses;
extern map<CInv, CDataStream> mapRelay;
extern deque<pair<int64, CInv> > vRelayExpiration;
extern map<CInv, int64> mapAlreadyAskedFor;
extern CAddress addrProxy;



// 转播库存
inline void RelayInventory(const CInv& inv)
{
	// 将此节点相连的所有节点进行转播此信息
    // Put on lists to offer to the other nodes
  //  CRITICAL_BLOCK(cs_vNodes)
        foreach(CNode* pnode, vNodes)
            pnode->PushInventory(inv);
}

template<typename T>
void RelayMessage(const CInv& inv, const T& a)
{
    CDataStream ss(SER_NETWORK);
    ss.reserve(10000);
    ss << a;
    RelayMessage(inv, ss);
}

template<>
inline void RelayMessage<>(const CInv& inv, const CDataStream& ss)
{
    //CRITICAL_BLOCK(cs_mapRelay)
    {
        // Expire old relay messages
        while (!vRelayExpiration.empty() && vRelayExpiration.front().first < GetTime())
        {
            mapRelay.erase(vRelayExpiration.front().second);
            vRelayExpiration.pop_front();
        }

        // Save original serialized message so newer versions are preserved
        mapRelay[inv] = ss;
        vRelayExpiration.push_back(make_pair(GetTime() + 15 * 60, inv));
    }
	// 节点进行库存转播
    RelayInventory(inv);
}




//
// Templates for the publish and subscription system.
// The object being published as T& obj needs to have:
//   a set<unsigned int> setSources member
//   specializations of AdvertInsert and AdvertErase
// Currently implemented for CTable and CProduct.
//

template<typename T>
void AdvertStartPublish(CNode* pfrom, unsigned int nChannel, unsigned int nHops, T& obj)
{
    // Add to sources
    obj.setSources.insert(pfrom->addr.ip);

    if (!AdvertInsert(obj))
        return;

    // Relay
   // CRITICAL_BLOCK(cs_vNodes)
        foreach(CNode* pnode, vNodes)
            if (pnode != pfrom && (nHops < PUBLISH_HOPS || pnode->IsSubscribed(nChannel)))
                pnode->PushMessage("publish", nChannel, nHops, obj);
}

template<typename T>
void AdvertStopPublish(CNode* pfrom, unsigned int nChannel, unsigned int nHops, T& obj)
{
    uint256 hash = obj.GetHash();

   // CRITICAL_BLOCK(cs_vNodes)
        foreach(CNode* pnode, vNodes)
            if (pnode != pfrom && (nHops < PUBLISH_HOPS || pnode->IsSubscribed(nChannel)))
                pnode->PushMessage("pub-cancel", nChannel, nHops, hash);

    AdvertErase(obj);
}

template<typename T>
void AdvertRemoveSource(CNode* pfrom, unsigned int nChannel, unsigned int nHops, T& obj)
{
    // Remove a source
    obj.setSources.erase(pfrom->addr.ip);

    // If no longer supported by any sources, cancel it
    if (obj.setSources.empty())
        AdvertStopPublish(pfrom, nChannel, nHops, obj);
}

#endif /* CXX_BT_NET_H */
/* EOF */
