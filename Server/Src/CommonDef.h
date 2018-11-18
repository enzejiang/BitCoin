/*
 * =====================================================================================
 *
 *       Filename:  CommonDef.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/18/2018 03:13:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_COMMON_DEF_H
#define EZ_BT_COMMON_DEF_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <errno.h>
#include <arpa/inet.h>
const short UDP_PORT  = 6666;
const short SYNC_PORT = 6667;

int udp_socket();
#endif
/* EOF */

