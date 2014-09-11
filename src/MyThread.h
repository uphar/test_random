#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "Thread.h"
#include <map>
#include <vector>


std::map<int, Thread*> threadMap;
std::vector<Thread*> readyQueue; // can just maintain list of threadIds instead of entire thread contents

int create(void (*f)(void));
void dispatch(int sig);
void start();
void run(int threadId);
void suspend(int threadId);
void resume(int threadId);
void deleteThread(int threadId);
statistics* getStatus(int threadId);
int createWithArgs(void* (*f)(void*), void *arg);
void clean();

#endif /* MYTHREAD_H */
