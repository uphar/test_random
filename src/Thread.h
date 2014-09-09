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
  static int next_thread_Id;
  int threadId;
  ThreadState state;
  statistics *thread_stat;

  public:
    Thread();
    int getID();
    void sleep(int);
    void yield();
    void setState(int);
    int getState();
    unsigned int getBurstCount();
    double getTotalExecutionTime();
    double getTotalSleepingTime();
    double getAvgExecutionTimeQuantum();
    double getAvgWaitingTime();
    statistics*  getStatistics();
};

#endif /* THREAD_H */