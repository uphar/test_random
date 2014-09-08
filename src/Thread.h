#ifndef THREAD_H
#define THREAD_H
enum ThreadState = { RUNNING, READY, SLEEPING, SUSPENDED, CREATED };

struct Statistics {
  unsigned int noOfBursts;
  double totalExecutionTime;
  double totalSleepingTime;
  double avgExecutionTimeQuantum;
  double avgWaitingTime;
};

Class Thread {
  static int next_thread_Id = 0;
  int threadId;
  ThreadState state;
  Statistics thread_stat;

  public:
    Thread();
    int getID();
    void sleep(int sec);
};

#endif /* THREAD_H */