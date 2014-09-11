#ifndef THREAD_H
#define THREAD_H
#include<setjmp.h>
enum ThreadState { RUNNING, READY, SLEEPING, SUSPENDED, CREATED };

struct statistics {
  unsigned int noOfBursts;
  double totalExecutionTime;
  double totalSleepingTime;
  double avgExecutionTimeQuantum;
  double avgWaitingTime;
};

class Thread {
  
  int threadId;
  void (*functionPointer)(void);
  ThreadState state;
  statistics *thread_stat;
  jmp_buf environment;
  public:
    
    static int next_thread_Id; 
    Thread(void (*f)(void));
    int getID();
    void (*getFunctionPointer(void))(void)
    {
      return functionPointer;
    }
    void sleep(int);
    void yield();
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
