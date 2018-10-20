/*
 * =====================================================================================
 *
 *       Filename:  NetWorkServ.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/06/2018 10:35:31 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EZ_BT_NWS_H
#define EZ_BT_NWS_H
#include <pthread.h>
#include "CAddress.h"
#include "CNode.h"

namespace Enze
{

    class CAddrDB;
    
    class NetWorkServ
    {
        public:
            static NetWorkServ* getInstance();
            static void Destory();
        public:
            void initiation();
            bool ConnectSocket(const CAddress& addrConnect, SOCKET& hSocketRet);
            CNode* FindNode(unsigned int ip);
            CNode* FindNode(const CAddress& addr);
            CNode* ConnectNode(const CAddress& addrConnect, int64 nTimeout=0);
            bool StartNode();
            bool StopNode();
    
            template<typename Stream>
            bool ScanMessageStart(Stream& s);
            bool ProcessMessages(CNode* pfrom);
            bool ProcessMessage(CNode* pfrom, string strCommand, CDataStream& vRecv);
            bool SendMessages(CNode* pto);
            
            void CheckForShutdown(int n);
            void AbandonRequests(void (*fn)(void*, CDataStream&), void* param1);
            bool AnySubscribed(unsigned int nChannel);
            map<string, CAddress>& getMapAddr()
            {
                return m_cMapAddresses; 
            }
            vector<CNode*>& getNodeList()
            {
                return m_cNodeLst;
            }
            
            const CAddress& getLocakAddr()
            {
                return m_cAddrLocalHost;
            }
            
            map<CInv, int64>& getMapAlreadyAskedFor()
            {
                return mapAlreadyAskedFor;
            }
            
            void MessageHandler(void* parg);
            void SocketHandler(void* parg);
            void OpenConnections(void* parg);
            
        private:
            NetWorkServ();
            ~NetWorkServ();
        
            bool LoadAddresses();
            bool AddUserProviedAddress();
            bool GetMyExternalIP(unsigned int& ipRet);
            bool GetMyExternalIP2(const CAddress& addrConnect, 
                                    const char* pszGet, const char* pszKeyword, unsigned int& ipRet);
        private:
            
            int nDropMessagesTest = 0; // 消息采集的频率，即是多个少消息采集一次进行处理
            bool m_bClient = false;
            uint64 m_nLocalServices = (m_bClient ? 0 : NODE_NETWORK);
            CAddress m_cAddrLocalHost;// = new CAddress(0, DEFAULT_PORT, m_nLocalServices);// 本地主机地址
            CNode* m_pcNodeLocalHost = new CNode(INVALID_SOCKET, CAddress("127.0.0.1", m_nLocalServices)); // 本地节点
            
            vector<CNode*> m_cNodeLst;
            map<string, CAddress> m_cMapAddresses;
            CAddress addrProxy;

            map<CInv, CDataStream> mapRelay;
            deque<pair<int64, CInv> > vRelayExpiration;
            map<CInv, int64> mapAlreadyAskedFor;
            
            static NetWorkServ* m_pInstance;
            static pthread_mutex_t m_NWSLock;
        public:
            // 转播库存
            inline void RelayInventory(const CInv& inv)
            {
                // 将此节点相连的所有节点进行转播此信息
                // Put on lists to offer to the other nodes
              //  CRITICAL_BLOCK(cs_m_cNodeLst)
                    foreach(CNode* pnode, m_cNodeLst)
                        pnode->PushInventory(inv);
            }

            template<typename T>
            void RelayMessage(const CInv& inv, const T& a)
            {
                printf("error %s--%d\n", __FILE__, __LINE__);
#if 0
                CDataStream ss(SER_NETWORK);
                ss.reserve(10000);
                ss << a;
                RelayMessage(inv, ss);
#endif
            }
#if 0
            inline void RelayMessage(const CInv& inv, const CDataStream& ss)
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
#endif
            


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
               // CRITICAL_BLOCK(cs_m_cNodeLst)
                    foreach(CNode* pnode, m_cNodeLst)
                        if (pnode != pfrom && (nHops < PUBLISH_HOPS || pnode->IsSubscribed(nChannel)))
                            pnode->PushMessage("publish", nChannel, nHops, obj);
            }

            template<typename T>
            void AdvertStopPublish(CNode* pfrom, unsigned int nChannel, unsigned int nHops, T& obj)
            {
                uint256 hash = obj.GetHash();

               // CRITICAL_BLOCK(cs_m_cNodeLst)
                    foreach(CNode* pnode, m_cNodeLst)
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

        
    };

}


#endif /* EZ_BT_NWS_H */
/* EOF */

