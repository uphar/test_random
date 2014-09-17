#ifndef THREAD_H
#define THREAD_H
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

typedef unsigned long long signal_time;

enum ThreadState { RUNNING, READY, SLEEPING, SUSPENDED, TERMINATED, CREATED };

struct statistics {
  unsigned int noOfBursts;
  double totalExecutionTime;
  double totalSleepingTime;
  double avgExecutionTimeQuantum;
  double avgWaitingTime;
  int totalRequestedSleepingTime;
};

class Thread {
  static int next_thread_Id;
  int threadId;
  ThreadState state;

  public:
    sigjmp_buf environment;
    void (*functionPointer)(void);
    void* (*functionWithArg) (void*);
    statistics *thread_stat;
    bool withArguments; //TODO check if we really need this try using other pointer as null
    void *arguments;
    void *returnValue;
    char *cStack;
    signal_time sleepTime;
    signal_time wakeUpTime;

    Thread(void (*f)(void));
    Thread(void* (*f)(void*), void *);
    int getID();
    bool isWithArguments();
    void* getArguments();
    void (*getFunctionPointer(void))(void)
    {
      return functionPointer;
    }
    void setState(ThreadState);
    int getState();
    unsigned int getBurstCount();
    double getTotalExecutionTime();
    double getTotalSleepingTime();
    double getAvgExecutionTimeQuantum();
    double getAvgWaitingTime();
    statistics*  getStatistics();
};

#endif /* THREAD_H */
