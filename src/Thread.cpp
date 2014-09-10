#include "Thread.h"
#include<iostream>
using namespace std;

int Thread::next_thread_Id = 0;

Thread::Thread(void (*function)(void)) {
  threadId = next_thread_Id++;
  state = CREATED;
  functionPointer=function;
  thread_stat = new statistics;
  thread_stat->noOfBursts = -1;
  thread_stat->totalExecutionTime = 0;
  thread_stat->totalSleepingTime = 0;
  thread_stat->avgExecutionTimeQuantum = 0;
  thread_stat->avgWaitingTime = 0;
  withArguments = false;
  cStack = new char[4096];
}

Thread::Thread(void* (*function)(void*), void* arguments) {
  threadId = next_thread_Id++;
  state = CREATED;
  functionWithArg=function;
  thread_stat = new statistics;
  thread_stat->noOfBursts = -1;
  thread_stat->totalExecutionTime = 0;
  thread_stat->totalSleepingTime = 0;
  thread_stat->avgExecutionTimeQuantum = 0;
  thread_stat->avgWaitingTime = 0;
  withArguments = true;
  cStack = new char[4096];
  this->arguments = arguments;
}

int Thread::getID() {
  return threadId;
}

/*void (Thread::*getFunctionPointer(void))(void)
{
  cout<<threadId<<endl;
  return functionPointer;
}
*/
void Thread::sleep(int sec) {
  state = SLEEPING;
  /* Add code to start timer and on completion of timer add thread to ready queue */
}

void Thread::yield() {}

void Thread::setState(ThreadState state) {
  this->state = state;
}

int Thread::getState() {
  return state;
}

unsigned int Thread::getBurstCount() {  //why such a return type
  return thread_stat->noOfBursts;
}

double Thread::getTotalExecutionTime() {
  return thread_stat->totalExecutionTime;
}

double Thread::getTotalSleepingTime() {
  return thread_stat->totalSleepingTime;
}

double Thread::getAvgExecutionTimeQuantum() {
  return thread_stat->avgExecutionTimeQuantum;
}

double Thread::getAvgWaitingTime() {
  return thread_stat->avgWaitingTime;
}

statistics* Thread::getStatistics() {
  return thread_stat;
}
