#include "Thread.h"
#include <iostream>

using namespace std;

int Thread::next_thread_Id = 0;

Thread::Thread(void (*function)(void)) {
  threadId = next_thread_Id++;
  state = CREATED; //check if we need to make status as ready here
  functionPointer=function;
  thread_stat = new statistics;
  thread_stat->noOfBursts = -1;
  thread_stat->totalExecutionTime = 0;
  thread_stat->totalSleepingTime = 0;
  thread_stat->avgExecutionTimeQuantum = 0;
  thread_stat->avgWaitingTime = 0;
  withArguments = false;
  cStack = new char[4096];
  unsigned long programCounter = (unsigned long)functionPointer; /* func is a pointer to function associated with the thread */
  unsigned long stackPointer = (unsigned long)cStack; /* stack is a pointer to the execution stack */
  cout<<hex<<programCounter<<"    "<<stackPointer<<endl;
  #if defined(__x86_64__)
    cout<<"hie"<<endl;
    #define JB_SP 6 
    #define JB_PC 7
    __asm__ __volatile__("xorq %%fs:0x30, %0\n\trolq $0x11, %0" : "=r"(programCounter) : "r"(programCounter));
    __asm__ __volatile__("xorq %%fs:0x30, %0\n\trolq $0x11, %0" : "=r"(stackPointer) : "r"(stackPointer));
  #elif defined(__i386__)
    cout<<"bye"<<endl;
    #define JB_SP 4 
    #define JB_PC 5
    __asm__ __volatile__("xorl %%gs:0x18, %0\n\troll $9, %0" : "=r"(programCounter) : "r"(programCounter));
    __asm__ __volatile__("xorl %%gs:0x18, %0\n\troll $9, %0" : "=r"(stackPointer) : "r"(stackPointer));
  #endif
  cout<<hex<<programCounter<<"    "<<stackPointer<<endl;
  cout<<dec<<JB_PC<<"    "<<JB_SP<<endl;
  environment[0].__jmpbuf[JB_SP] = stackPointer;
  environment[0].__jmpbuf[JB_PC] = programCounter;
  cout<<hex<<environment[0].__jmpbuf[JB_SP]<<"    "<<environment[0].__jmpbuf[JB_PC]<<endl;
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
  returnValue = NULL;
  unsigned long programCounter = (unsigned long)functionWithArg; /* func is a pointer to function associated with the thread */
  unsigned long stackPointer = (unsigned long)cStack; /* stack is a pointer to the execution stack */
  // cout<<hex<<p1<<"    "<<p2<<endl;
  #if defined(__x86_64__)
    cout<<"hie"<<endl;
    #define JB_SP 6 
    #define JB_PC 7
    __asm__ __volatile__("xorq %%fs:0x30, %0\n\trolq $0x11, %0" : "=r"(programCounter) : "r"(programCounter));
    __asm__ __volatile__("xorq %%fs:0x30, %0\n\trolq $0x11, %0" : "=r"(stackPointer) : "r"(stackPointer));
  #elif defined(__i386__)
    cout<<"bye"<<endl;
    #define JB_SP 4 
    #define JB_PC 5
    __asm__ __volatile__("xorl %%gs:0x18, %0\n\troll $9, %0" : "=r"(programCounter) : "r"(programCounter));
    __asm__ __volatile__("xorl %%gs:0x18, %0\n\troll $9, %0" : "=r"(stackPointer) : "r"(stackPointer));
  #endif
  // cout<<hex<<p1<<"    "<<p2<<endl;
  environment[0].__jmpbuf[JB_SP] = stackPointer;
  environment[0].__jmpbuf[JB_PC] = programCounter;
}

int Thread::getID() {
  return threadId;
}

bool Thread::isWithArguments() {
  return withArguments;
}

void* Thread::getArguments() {
  return arguments;
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
