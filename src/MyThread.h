#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "Thread.h"
#include <map>
#include <vector>




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
