/*
 * =====================================================================================
 *
 *       Filename:  SocketWraper.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2018 03:55:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include "CAddress.h"
#include "SocketWraper.h"
namespace Enze
{

SOCKET udp_socket(const CAddress& cSelfAddr, const CAddress& cServAddr, bool bConnct)
{
    SOCKET udpFd = socket(AF_INET, SOCK_DGRAM, 0);
    int on = 1;
    setsockopt(udpFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    setsockopt(udpFd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    
    struct sockaddr_in servAddr = cServAddr.GetSockAddr();
    
    struct sockaddr_in selfAddr = cSelfAddr.GetSockAddr();
    printf("udp_socket, ip[%s] port[%d]\n", inet_ntoa(selfAddr.sin_addr), htons(selfAddr.sin_port));
    if (0 != bind(udpFd, (struct sockaddr*)&selfAddr, sizeof(selfAddr))) {
        printf("%s---%d, Bind error %m\n", __FILE__, __LINE__);
        return -1;
    }
    if (bConnct) {
        if (0 != connect(udpFd, (struct sockaddr*)&servAddr, sizeof(servAddr))) {
            printf("%s---%d, connect error %m\n", __FILE__, __LINE__);
            return -1; 
        }
    
    }
    
    return udpFd;
}

SOCKET tcp_socket(const CAddress& cServAddr, short selfPort)
{
    SOCKET tcpFd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(tcpFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    
    struct sockaddr_in servAddr = cServAddr.GetSockAddr();
    
    struct sockaddr_in selfAddr;
    bzero(&selfAddr, sizeof(servAddr));
    selfAddr.sin_family = AF_INET;
    selfAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    selfAddr.sin_port = htons(selfPort);
    
    if (0 != bind(tcpFd, (struct sockaddr*)&selfAddr, sizeof(selfAddr))) {
        printf("%s---%d, Bind error %m\n", __FILE__, __LINE__);
        return -1;
    }
    
    if (0 != connect(tcpFd, (struct sockaddr*)&cServAddr, sizeof(cServAddr))) {
        printf("%s---%d, connect error %m\n", __FILE__, __LINE__);
        return -1; 
    }
    
    
    
    return tcpFd;

}

int sock_send(SOCKET fd, const char* buf)
{
    if (-1 ==fd || NULL == buf) {
        return -1;
    }
    
    int len = strlen(buf);
    return send(fd, buf, len, 0);
}

char* sock_recv(SOCKET fd) 
{
    if (0 > fd) {
        printf("%s---%d, error [fd = %d]\n", __FILE__, __LINE__, fd);
        return NULL;
    }
    const int MAX_LEN = 1024;
    char* buf = (char* )malloc(sizeof(char)*MAX_LEN);
    int ret = recv(fd, buf, MAX_LEN-1, 0);
    if (ret < 0) {
        printf("%s---%d, error [%m]\n", __FILE__, __LINE__);
        free(buf);
        return NULL;
    }
    buf[ret] = '\0';
    return buf;
}
    
int sock_sendto(SOCKET fd, const char* buf, const CAddress&cAddr)
{
    if (-1 ==fd) {
        printf("%s---%d, invalid param\n", __FILE__, __LINE__);
        return -1;
    }
    struct sockaddr_in addr = cAddr.GetSockAddr();
    int len = strlen(buf);
    printf("sock_sendto, ip[%s] port[%d]\n", inet_ntoa(addr.sin_addr), htons(addr.sin_port));

    return sendto(fd, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr));
}

char* sock_recvfrom(SOCKET fd, CAddress& cAddr)
{
    if (-1 ==fd) {
        printf("%s---%d, invalid param\n", __FILE__, __LINE__);
        return NULL;
    }
    
    const int MAX_LEN = 1024;
    char* buf = (char* )malloc(sizeof(char)*MAX_LEN);
    
    struct sockaddr_in addr;
    socklen_t sockLen = sizeof(addr); 
    bzero(&addr, sockLen);
    
    int ret = recvfrom(fd, buf, MAX_LEN, 0, (struct sockaddr*)&addr, &sockLen);
    if (ret < 0) {
        printf("%s---%d, error[%m]\n", __FILE__, __LINE__);
        free(buf);
        return 0;
    }
    
    buf[ret] = '\0';
    cAddr = CAddress(addr);
    return buf;
}

}
/* EOF */

