/*
 * =====================================================================================
 *
 *       Filename:  rtdealer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/18/2018 09:49:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */


#include "zhelpers.h"
#include <pthread.h>

#define NBR_WORKERS 3

static void* woker_task(void *ctx)
{
    //void* ctx = zmq_ctx_new();
    void* worker = zmq_socket(ctx, ZMQ_DEALER);
    s_set_id(worker);
    zmq_connect(worker, "tcp://localhost:5671");
    int total = 0;
    while(1){
        s_sendmore(worker, "");
        s_send(worker, "Hi Boss");
        free(s_recv(worker));
        char* workload = s_recv(worker);
        if(0 == strcmp(workload, "Fired!")) {
            printf(" Completed: %d tasks\n", total);
            free(workload);
            break;
        }
        else {
         //   printf("%s\n", workload);
        }
        
        free(workload);
        ++total; 
        s_sleep(randof(500)+1);
    }
    
    zmq_close(worker);
    return NULL;
}

int main()
{
    void* ctx = zmq_ctx_new();
    void* broker = zmq_socket(ctx, ZMQ_ROUTER);
    zmq_bind(broker, "tcp://*:5671");
    srandom((unsigned)time(NULL));
    
    int iCnt = 0;
    for (iCnt = 0; iCnt < NBR_WORKERS; ++iCnt) {
        pthread_t pid;
        pthread_create(&pid, NULL, woker_task, ctx);
    }
    
    int64_t end_time = s_clock() + 5000;
    int wokers_fired = 0;
    while(1) {
        char* id = s_recv(broker);
        s_sendmore(broker, id);
        //printf("%s ", id);
        free(id); 
        id = s_recv(broker);
        //printf("%s ", id);
        free(id);
        id = s_recv(broker);
       // printf("%s\n", id);
        free(id);
        
        s_sendmore(broker, "");
        if (s_clock() < end_time)
            s_send(broker, "Work harder");
        else {
            s_send(broker, "Fired!");
            ++wokers_fired;
            if (NBR_WORKERS == wokers_fired)
                break;
        }
    }
   
    sleep(3);
    zmq_close(broker);
    zmq_ctx_destroy(ctx);
    return 0;
}
/* EOF */

