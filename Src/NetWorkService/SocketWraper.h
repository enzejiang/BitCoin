/*
 * =====================================================================================
 *
 *       Filename:  SocketWraper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2018 03:49:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef EZ_BT_SOCKET_WRAPPER_H
#define EZ_BT_SOCKET_WRAPPER_H

#include <unistd.h>
namespace Enze {
    #define SOCKET int 
    class CAddress;
    SOCKET udp_socket(const CAddress& cSelfAddr, const CAddress& cServAddr, bool bConnct = true);
    SOCKET tcp_socket(const CAddress& cServAddr, short selfPort = 8334);
    int sock_send(SOCKET fd, const char* buf);
    char* sock_recv(SOCKET fd);
    int sock_sendto(SOCKET fd, const char* buf, const CAddress&cAddr);
    char* sock_recvfrom(SOCKET fd, CAddress& cAddr);
}


#endif /* EZ_BT_SOCKET_WRAPPER_H */
/* EOF */

