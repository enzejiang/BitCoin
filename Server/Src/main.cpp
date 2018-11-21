/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/18/2018 03:08:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <thread>
#include <mutex>
#include <list>
#include <iostream>
#include "zhelpers.h"
#include "ServerMessage.pb.h"
#include "CommonDef.h"
using namespace std;

std::mutex g_cMutex;            
ServMsg g_cMsg;


void RouterThread()
{
    printf("RouterThread --start\n");
    char buf[30] = {0};
    sprintf(buf, "tcp://*:%d",SYNC_PORT);
    printf("RouterThread, Endpoint[%s]\n", buf);
    void* ctx = zmq_ctx_new();
    void* Router = zmq_socket(ctx, ZMQ_ROUTER);
    int rc = zmq_bind(Router, buf);
    if (0 != rc) {
        printf("zmq_bind(%s) error %m\n", buf);
        return;
    }
    
    zmq_pollitem_t items [] = { {Router, 0, ZMQ_POLLIN, 0}};
    long timeout = 1000;
    
    while(1) {
        int ret = zmq_poll(items, 1, timeout * 5);
        if (-1 ==ret) {
            printf("zmq_poll error %m\n");
            break;
        }
        if (items[0].revents & ZMQ_POLLIN) {
            char* id = s_recv(Router);
            printf("RouterThread, Recv id[%s]\n", id);
            char* emp = s_recv(Router);
            printf("RouterThread, Recv emp[%s]\n", emp);
            char* data = s_recv(Router);
            printf("RouterThread, Recv 2 data[%s]\n", data);
            if (0 == strcmp("getaddr", data) || 0 == strcmp("getAddr", data)) {
                printf("Send Add List to [%s], Size[%d]\n",id, g_cMsg.eplist_size());
                s_sendmore(Router, id);
                s_sendmore(Router, "");
                string strData;
                g_cMutex.try_lock();    
                g_cMsg.SerializePartialToString(&strData);
                g_cMutex.unlock();    
                s_send(Router, (char*)strData.c_str());
            }
            free(id);
            free(emp);
            free(data);
        } 
    }
    
    zmq_close(Router);
    zmq_ctx_destroy(ctx);
}

int main()
{
    int udpFd = udp_socket();
    if (-1 == udpFd) {
        printf("socket error %m\n");
        return -1;
    }
    
    thread t1(RouterThread);
    while(1)
    {
        struct {
            unsigned int ip;
            short port;
        }ep;
        
        sockaddr_in    addrUser1 = {0};

        socklen_t nLen1 = sizeof(addrUser1);
        //服务器接收来自客户端的消息，并且用addrUser1保存地址和端口
        if (-1 == recvfrom(udpFd, &ep, sizeof(ep), 0, (sockaddr*)&addrUser1, &nLen1))
        {
            cout << "dfdfda"<<endl;
            printf ("recv user 1 failed.errno=[%d]", errno);
            break;
        }
        else 
        {    
            char buf[100] = {0};
            printf ("connect user ip=[%s] port=[%d]\n", inet_ntoa(addrUser1.sin_addr), htons(addrUser1.sin_port));
            g_cMutex.try_lock();    
            endPoint* pEndPoint = g_cMsg.add_eplist();
            pEndPoint->set_ip(addrUser1.sin_addr.s_addr);
            pEndPoint->set_port(addrUser1.sin_port);
            
            pEndPoint->set_localip(ep.ip);
            pEndPoint->set_localport(ep.port);
            g_cMutex.unlock();    
        }        
    }

    t1.join();
    return 0;
}

/* EOF */

