//
// Created by yinon on 06/06/2024.
//
#include <csetjmp>
#include "tools.h"
#ifndef _THREAD_H_
#define _THREAD_H_

class Thread {
 public:
  int Id;
  States StateThread;
  int QuantumsCounter;
  char* Stack;
  sigjmp_buf env;
  int Wait;
  Thread(int id, char* stack);
};


#endif //_THREAD_H_
