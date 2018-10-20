/*
 * =====================================================================================
 *
 *       Filename:  CommDataDef.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 03:52:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef EZ_BT_COMMON_DATA_DEFINE_H
#define EZ_BT_COMMON_DATA_DEFINE_H

namespace Enze
{

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64  int64;
typedef unsigned __int64  uint64;
#else
typedef long long  int64;
typedef unsigned long long  uint64;
#endif
#if defined(_MSC_VER) && _MSC_VER < 1300
#define for  if (false) ; else for
#endif

const unsigned int MAX_SIZE = 0x02000000;
// COIN 表示的是一个比特币，而且100000000就是表示一个比特币，比特币最小单位为小数点后8位
const int64 COIN = 100000000;
const int64 CENT = 1000000;
const int COINBASE_MATURITY = 2;// 币基成熟度
// 工作量证明的难度
const int VERSION = 105;
enum
{
    // primary actions
    SER_NETWORK         = (1 << 0),
    SER_DISK            = (1 << 1),
    SER_GETHASH         = (1 << 2),

    // modifiers
    SER_SKIPSIG         = (1 << 16),
    SER_BLOCKHEADERONLY = (1 << 17),
};
} //end namespace

#endif /* EZ_BT_COMMON_DATA_DEFINE_H */
/* EOF */

