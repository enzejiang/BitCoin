/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/03/2018 03:26:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "Hello.pb.h"
#include <iostream>
using namespace std;
int main()
{
    Person cTest;
    cTest.set_m_strname("Enze");
    cout<< cTest.m_strname()<<endl;

    char buf[1024] = {0};
    int Len = cTest.ByteSize();
    cout << "Size--"<<Len<<endl;
    cTest.SerializeToArray(buf, Len);
    Person cTest2;
    cTest2.ParseFromArray(buf, Len);
    cout<< cTest2.m_strname()<<endl;
}
/* EOF */

