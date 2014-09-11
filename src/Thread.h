#ifndef THREAD_H
#define THREAD_H
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

  public:
    static int next_thread_Id; //////// convert to static
    Thread(void (*f)(void));
   // static void setThreadIdToZero();
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
