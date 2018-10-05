/*
 * =====================================================================================
 *
 *       Filename:  Net_DataDef.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 08:27:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_BT_NET_DATA_DEF_H
#define CXX_BT_NET_DATA_DEF_H

#include <arpa/inet.h>
#include "serialize.h"
#include "util.h"

typedef int SOCKET;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
// 默认端口号
const unsigned short DEFAULT_PORT = htons(8333);
const unsigned int PUBLISH_HOPS = 5;

const unsigned char pchIPv4[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };

extern bool fClient;
extern uint64 nLocalServices;
extern bool fShutdown;
extern boost::array<bool, 10> vfThreadRunning;

enum
{
    NODE_NETWORK = (1 << 0),
};
// 消息类型
enum
{
    MSG_TX = 1, // 交易消息
    MSG_BLOCK, // 块信息
    MSG_REVIEW, //
    MSG_PRODUCT, // 产品消息
    MSG_TABLE,// 表
};

static const char* ppszTypeName[] =
{
    "ERROR",
    "tx",
    "block",
    "review",
    "product",
    "table",
};



#endif /* CXX_BT_NET_DATA_DEF_H */
/* EOF */

