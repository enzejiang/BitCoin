/*
 * =====================================================================================
 *
 *       Filename:  ThreadGuard.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/2018 09:01:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_THREADGUARD_H
#define EZ_BT_THREADGUARD_H
#include <thread>

using namespace std;
namespace Enze {
    
    class ThreadGuard
    {
        public:
            ThreadGuard(thread* pThread)
                :m_pThread(pThread)        
            {
            
            }
            
            ~ThreadGuard()
            {
                if (m_pThread && m_pThread->joinable()) {
                    m_pThread->join();
                }
                
                delete m_pThread;
            }
            
            inline thread* getThread() 
            {
                return m_pThread;
            }
            
            ThreadGuard()=delete;
            ThreadGuard(const ThreadGuard&)=delete;
            ThreadGuard& operator=(const ThreadGuard&)=delete;
        private:
            thread* m_pThread;
    };


} /*End Namespace Enze*/

#endif
/* EOF */

