#include<stdio.h>

enum threadState={RUNNING,READY,SLEEPING,SUSPENDED};
struct Statistics
{
        int threadId;
        enum threadState state;
        int noOfBursts;
        double totalExecutionTime;
        double totalSleepingTime;
        double avgExecutionTimeQuantum;
        double avgWaitingTime;
};
struct readyQueue
{
        struct Statistics thread;
        struct readyQueue *next;
        int sizeOfQueue;
};

int main()
{
}
