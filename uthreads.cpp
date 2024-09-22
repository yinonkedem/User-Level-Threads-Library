#include <iostream>
#include <csignal>
#include <set>
#include <queue>
#include <sys/time.h>
#include <algorithm>
#include <vector>

#include "uthreads.h"
#include "Thread.h"
#include "tools.h"
#include "deque"

#define STE_ERROR "system error: timer error."
#define MASK_ERROR cerr << "system error: mask error" << endl
#define INVALID_INPUT "thread library error: invalid input"
#define OUT_OF_RANGE  "thread library error: invalid input thread id invalid"
#define SLEEP_ERROR "thread library error: try to sleep main thread"
#define BLOCK_ERROR "thread library error: try to block main thread"
#define SIZE_ERROR "thread library error: no space"
#define SIG_ERROR "system error: sigaction error."
#define ALLOC_ERROR "system error: allocation error"
using namespace std;

std::deque<Thread*> blockedThreads;
std::deque<Thread*> readiesThreads;
Thread* currentThread;
int quantumMicroUsecs;
int total_quan;
Thread* threads[MAX_THREAD_NUM]{nullptr};

void setTimer (void (*handler)(int));
void Maincase ();
void terminateItself ();
void readyCase (int tid);
void blockedCase (int tid);
void release ();
void blockedItself ();
void threadConditionHandeler ();
void spawnWhenTerminate (int i);

void resumeBlocked (int tid);
void blockWhenReady (int tid);
void scheduler(int sig){
  threadConditionHandeler ();
  if(currentThread->StateThread!=TERMINATE){
    if(sigsetjmp(currentThread->env,1)!=0){
      return;
    }
  }
  if(currentThread->StateThread==RUNNING){
    currentThread->StateThread=READY;
    readiesThreads.push_back (currentThread);
  }
  if(readiesThreads.empty()==0){
    total_quan++;
    currentThread=readiesThreads.front();
    currentThread->StateThread=RUNNING;
    currentThread->QuantumsCounter+=1;
    readiesThreads.pop_front();
    setTimer (&scheduler);
    siglongjmp(currentThread->env,1);
  }
}

void threadConditionHandeler ()
{
  for(int i=0; i < MAX_THREAD_NUM; i++){
    Thread* current=threads[i];
    if(current== nullptr){
      continue;
    }
    if(current->StateThread==SLEEP||current->StateThread==SLEEP_WITH_BLOCKED){
      current->Wait-=1;
      if(current->Wait==0){
       current->StateThread= (current->StateThread==SLEEP)?READY:BLOCKED;
        (current->StateThread==READY)?readiesThreads.push_back (current)
        :blockedThreads.push_back (current);
      }
    }
    if(current!=currentThread&&current->StateThread==TERMINATE){
      delete current->Stack;
      delete current;
      threads[i]= nullptr;
    }
  }
}

int uthread_init(int quantum_usecs) {
  if (quantum_usecs <= 0){
    cerr << INVALID_INPUT << " quantum can't be negative or zero" << endl;
    return -1;
  }
  quantumMicroUsecs=quantum_usecs;
  total_quan=1;
  try{
    threads[0]=new Thread(0,nullptr);
  }
  catch (const std::exception& e){
    cerr<< ALLOC_ERROR << endl;
  }
  Thread* first_thread=threads[0];
  first_thread->StateThread= RUNNING;
  first_thread->QuantumsCounter+=1;
  currentThread=first_thread;
  sigsetjmp(first_thread->env,1);
  setTimer(&scheduler);
  return 0;
}

void setTimer (void (*handler)(int))
{
  struct sigaction sa = {0};
  struct itimerval timer{};

  sa.sa_handler = handler;
  if (sigaction(SIGVTALRM, &sa, NULL) < 0)
  {
    release();
    cerr << SIG_ERROR << endl;
    exit(1);
  }
  timer.it_value.tv_sec = quantumMicroUsecs / 1000000;
  timer.it_value.tv_usec = quantumMicroUsecs % 1000000;

  timer.it_interval.tv_sec = quantumMicroUsecs / 1000000;
  timer.it_interval.tv_usec = quantumMicroUsecs % 1000000;

  if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
  {
    release();
    cerr << STE_ERROR << endl;
    exit(1);
  }
}

void release ()
{
  for(int i=MAX_THREAD_NUM-1;i>=0;i--){
    if(threads[i]!= nullptr){
      delete threads[i]->Stack;
      delete threads[i];
    }
  }
  readiesThreads.clear();
  blockedThreads.clear();
}

void setupThread(Thread* thread, thread_entry_point entry_point){
  address_t sp = (address_t) thread->Stack + STACK_SIZE - sizeof(address_t);
  address_t pc = (address_t) entry_point;
  sigsetjmp(thread->env, 1);
  ((thread->env)->__jmpbuf)[JB_SP] = translate_address(sp);
  ((thread->env)->__jmpbuf)[JB_PC] = translate_address(pc);
  if(sigemptyset(&(thread->env->__saved_mask)) != 0){
    MASK_ERROR;
    release();
    exit(1);
  }
}

void handleSigmaskError() {
  MASK_ERROR;
  release();
  exit(1);
}

void  unPreventSignal(){
  sigset_t set;
  sigemptyset(&set);

  if (sigaddset(&set, SIGVTALRM) != 0) {
    handleSigmaskError();
  }

  if (sigprocmask(SIG_UNBLOCK, &set, nullptr) != 0) {
    handleSigmaskError();
  }
}

void  preventSignal(){
  sigset_t set;
  sigemptyset(&set);

  if (sigaddset(&set, SIGVTALRM) != 0) {
    handleSigmaskError();
  }

  if (sigprocmask(SIG_BLOCK, &set, nullptr) != 0) {
    handleSigmaskError();
  }
}

int uthread_spawn(thread_entry_point entry_point) {
  if (entry_point == nullptr) {
    cerr << INVALID_INPUT << " entry point can't be null" << endl;
    unPreventSignal();
    return -1;
  }
  for (int i = 1; i < MAX_THREAD_NUM; i++) {
    spawnWhenTerminate (i);
    if (threads[i] == nullptr) {
      try {
        threads[i] = new Thread(i, new char[STACK_SIZE]);
      }
      catch(const std::exception& e) {
        cerr<< ALLOC_ERROR <<endl;
      }
      readiesThreads.push_back(threads[i]);
      setupThread(threads[i], entry_point);
      return threads[i]->Id;
    }
  }
  cerr << SIZE_ERROR << endl;
  return -1;
}

void spawnWhenTerminate (int i)
{
  if(threads[i] != nullptr && threads[i]->StateThread == TERMINATE){
    delete threads[i]->Stack;
    delete threads[i];
    threads[i] = nullptr;
  }
}

int uthread_terminate(int tid) {
  preventSignal();
  if(tid < 0 || tid >= MAX_THREAD_NUM){
    cerr << OUT_OF_RANGE << endl;
    unPreventSignal();
    return -1;
  }
  else if (tid == 0) {
    Maincase ();
  } else if (tid == uthread_get_tid()) {
    terminateItself ();
  } else if(threads[tid] == nullptr || threads[tid]->StateThread == TERMINATE){
    unPreventSignal();
    return -1;
  } else if(threads[tid]->StateThread == READY){
    readyCase (tid);
  }else if(threads[tid]->StateThread == BLOCKED){
    blockedCase (tid);
  }
  delete threads[tid]->Stack;
  delete threads[tid];
  threads[tid] = nullptr;
  unPreventSignal();
  return 0;
}

void blockedCase (int tid)
{
  auto it = find(blockedThreads.begin(), blockedThreads.end(), threads[tid]);
  if (it != readiesThreads.end()) {
    blockedThreads.erase(it);
  }
}

void readyCase (int tid)
{
  auto it = find(readiesThreads.begin(), readiesThreads.end(), threads[tid]);
  if (it != readiesThreads.end()) {
    readiesThreads.erase(it);
  }
}

void terminateItself ()
{
  currentThread->StateThread = TERMINATE;
  unPreventSignal();
  scheduler(SIGALRM);
}

void Maincase ()
{
  release();
  unPreventSignal();
  exit(0);
}

int uthread_block(int tid) {
  preventSignal();
  if(tid < 0 || tid >= MAX_THREAD_NUM){
    cerr << OUT_OF_RANGE << endl;
    unPreventSignal();
    return -1;
  }
  else if (threads[tid]==nullptr || threads[tid]->StateThread==TERMINATE)
  {
    cerr << INVALID_INPUT << endl;
    unPreventSignal();
    return -1;
  } else if (tid == 0){
    cerr << BLOCK_ERROR << endl;
    unPreventSignal();
    return -1;
  }
  else if (tid == uthread_get_tid()) {
    blockedItself ();

  } else if(threads[tid]->StateThread == READY){
    blockWhenReady (tid);
  }else if(threads[tid]->StateThread == SLEEP){
    threads[tid]->StateThread=SLEEP_WITH_BLOCKED;
    blockedThreads.push_back (threads[tid]);
  }
  unPreventSignal();
  return 0;
}
void blockWhenReady (int tid)
{
  readyCase (tid);
  threads[tid]->StateThread=BLOCKED;
  blockedThreads.push_back (threads[tid]);
}

void blockedItself ()
{
  currentThread->StateThread=BLOCKED;
  blockedThreads.push_back (currentThread);
  unPreventSignal();
  scheduler (SIGALRM);
}

int uthread_resume(int tid) {
  preventSignal();
  if(tid < 0 || tid >= MAX_THREAD_NUM||threads[tid]==
  nullptr||threads[tid]->StateThread==TERMINATE){
    cerr << OUT_OF_RANGE << endl;
    unPreventSignal();
    return -1;
  }
  if(threads[tid]->StateThread==BLOCKED){
    resumeBlocked (tid);
  }
  else if(threads[tid]->StateThread==SLEEP_WITH_BLOCKED){
    blockedCase (tid);
    threads[tid]->StateThread=SLEEP;
  }
  unPreventSignal();
  return 0;
}
void resumeBlocked (int tid)
{
  threads[tid]->StateThread=READY;
  blockedCase (tid);
  readiesThreads.push_back (threads[tid]);
}

int uthread_sleep(int num_quantums) {
  if(uthread_get_tid()!=0){
    preventSignal();
    currentThread->StateThread=SLEEP;
    currentThread->Wait=num_quantums+1;
    unPreventSignal();
    scheduler(SIGALRM);
    return 0;
  }
  cerr << SLEEP_ERROR << endl;
  return -1;
}

int uthread_get_tid() {
  return currentThread->Id;
}

int uthread_get_total_quantums() {
  return total_quan;
}

int uthread_get_quantums(int tid){
  if(tid < 0 || tid >= MAX_THREAD_NUM){
    cerr << OUT_OF_RANGE << endl;
    return -1;
  }
  else if(threads[tid] == nullptr || threads[tid]->StateThread == TERMINATE){
    cerr << INVALID_INPUT << endl;
    return -1;
  }
  return threads[tid]->QuantumsCounter;
}
