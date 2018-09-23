/*
 * =====================================================================================
 *
 *       Filename:  ui.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/22/2018 10:30:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CXX_BT_UI_H
#define CXX_BT_UI_H
#include<map>
extern map<string, string> mapAddressBook; // 地址和名称的映射，其中key为地址，value为名称


void* ThreadRequestProductDetails(void* parg);
void* ThreadRandSendTest(void* parg);
extern bool fRandSendTest;
void RandSend();
map<string, string> ParseParameters(int argc, char* argv[]);
bool OnInit(int argc, char* argv[]);
extern vector<pthread_t> gThreadList;

#endif
/* EOF */
