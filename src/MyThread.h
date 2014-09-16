#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "Thread.h"
#include <map>
#include <vector>

int create(void (*f)(void));
void dispatch(int);
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
int saveContext();
void resumeContext();
void terminate(int threadId);

#endif /* MYTHREAD_H */
