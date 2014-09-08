#include <iostream>
#include "Thread.h"

Thread::Thread() {
  threadId = next_thread_Id++;
  state = CREATED;
  thread_stat.noOfBursts = -1;
  thread_stat.totalExecutionTime = 0;
  thread_stat.totalSleepingTime = 0;
  thread_stat.avgExecutionTimeQuantum = 0;
  thread_stat.avgWaitingTime = 0;
}

int Thread::getID() {
  return threadId;
}

void Thread:sleep(int sec) {
  state = SLEEPING;
  /* Add code to start timer and on completion of timer add thread to ready queue */
}