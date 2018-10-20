// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef EZ_BT_IRC_H
#define EZ_BT_IRC_H
namespace Enze
{
    bool RecvLine(SOCKET hSocket, string& strLine);
    void* ThreadIRCSeed(void* parg);

}

//extern CCriticalSection cs_mapIRCAddresses;
#endif
/* EOF */
