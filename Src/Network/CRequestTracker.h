/*
 * =====================================================================================
 *
 *       Filename:  CRequestTracker.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 08:52:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_BT_CREQUEST_TRACKER_H
#define CXX_BT_CREQUEST_TRACKER_H

#include "Net_DataDef.h"

class CRequestTracker
{
public:
    void (*fn)(void*, CDataStream&);
    void* param1;

    explicit CRequestTracker(void (*fnIn)(void*, CDataStream&)=NULL, void* param1In=NULL)
    {
        fn = fnIn;
        param1 = param1In;
    }

    bool IsNull()
    {
        return fn == NULL;
    }
};

#endif /* CXX_BT_CREQUEST_TRACKER_H */
/* EOF */

