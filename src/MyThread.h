#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "Thread.h"
#include <map>
#include <vector>

// extern std::map<int, Thread*> threadMap;
// extern std::vector<Thread*> readyQueue;  // can just maintain list of threadIds instead of entire thread contents



int create(void (*f)(void));
void dispatch();
void alarm_handler(int);
void start();
void run(int threadId);
void suspend(int threadId);
void resume(int threadId);
void deleteThread(int threadId);
statistics* getStatus(int threadId);
void printStatus(int threadId);
int createWithArgs(void* (*f)(void*), void *arg);
void clean();
void yield();
void initTimer();
void saveContext();
void resumeContext();
void terminate(int threadId);


#endif /* MYTHREAD_H */
