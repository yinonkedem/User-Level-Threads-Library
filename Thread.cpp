//
// Created by yinon on 06/06/2024.
//

#include "Thread.h"
Thread::Thread(int id, char* stack) {
  Id = id;
  StateThread = READY;
  QuantumsCounter = 0;
  Wait=0;
  Stack = stack;
}