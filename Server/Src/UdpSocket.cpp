/*
 * =====================================================================================
 *
 *       Filename:  UdpSocket.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/18/2018 09:07:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "CommonDef.h"

int udp_socket()
{
    int udpFd = socket(AF_INET, SOCK_DGRAM, 0);
    
    int on = 1;
    setsockopt(udpFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    setsockopt(udpFd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    struct sockaddr_in cAddr;
    int len = sizeof(cAddr);
    bzero(&cAddr, len);
    cAddr.sin_family = AF_INET;
    cAddr.sin_addr.s_addr = INADDR_ANY;
    cAddr.sin_port = htons(UDP_PORT);
    
    if (0 < bind(udpFd, (struct sockaddr*)&cAddr, len)) {
        printf("Bind Error\n");
        close(udpFd);
        return -1;
    }
    
    return udpFd;
}


/* EOF */

