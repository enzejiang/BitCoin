/*
 * =====================================================================================
 *
 *       Filename:  CMessageHeader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 08:13:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CMESSAGE_HEADER_H
#define EZ_BT_CMESSAGE_HEADER_H

#include "Net_DataDef.h"

namespace Enze{

//
// Message header
//  (4) message start
//  (12) command
//  (4) size

// The message start string is designed to be unlikely to occur in normal data.
// The characters are rarely used upper ascii, not valid as UTF-8, and produce
// a large 4-byte int at any alignment.
// 所有的消息都共有的消息头
static const char pchMessageStart[4] = { 0xf9, 0xbe, 0xb4, 0xd9 };

// 消息头
class CMessageHeader
{
public:
    enum { COMMAND_SIZE=12 };
    char m_pchMessageStart[sizeof(::pchMessageStart)];
    char m_pchCommand[COMMAND_SIZE]; // 命令
    unsigned int m_nMessageSize; // 消息内容的大小

    CMessageHeader()
    {
        memcpy(m_pchMessageStart, ::pchMessageStart, sizeof(m_pchMessageStart));
        memset(m_pchCommand, 0, sizeof(m_pchCommand));
        m_pchCommand[1] = 1;
        m_nMessageSize = -1;
    }

    CMessageHeader(const char* pszCommand, unsigned int nMessageSizeIn)
    {
        memcpy(m_pchMessageStart, ::pchMessageStart, sizeof(m_pchMessageStart));
        strncpy(m_pchCommand, pszCommand, COMMAND_SIZE);
        m_nMessageSize = nMessageSizeIn;
    }


    string GetCommand()
    {
        if (m_pchCommand[COMMAND_SIZE-1] == 0)
            return string(m_pchCommand, m_pchCommand + strlen(m_pchCommand));
        else
            return string(m_pchCommand, m_pchCommand + COMMAND_SIZE);
    }

    // 判断对应的消息头是否有效
    bool IsValid()
    {
        // Check start string
        if (memcmp(m_pchMessageStart, ::pchMessageStart, sizeof(m_pchMessageStart)) != 0)
            return false;

        // Check the command string for errors
        for (char* p1 = m_pchCommand; p1 < m_pchCommand + COMMAND_SIZE; p1++)
        {
            // 遇到一个为0后，其对应之后都应该为0
            if (*p1 == 0)
            {
                // Must be all zeros after the first zero
                for (; p1 < m_pchCommand + COMMAND_SIZE; p1++)
                    if (*p1 != 0)
                        return false;
            }
            else if (*p1 < ' ' || *p1 > 0x7E)
                return false;
        }

        // Message size
        if (m_nMessageSize > 0x10000000)
        {
            printf("CMessageHeader::IsValid() : nMessageSize too large %u\n", m_nMessageSize);
            return false;
        }

        return true;
    }
};

}// end namespace
#endif /* EZ_BT_CMESSAGE_HEADER_h */
/* EOF */

