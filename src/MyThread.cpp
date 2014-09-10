#include "MyThread.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>

#define INTERVAL 1000

using namespace std;

std::map<int, Thread*> threadMap;
std::vector<int> readyQueue;
std::vector<int>::iterator currentThread;

void start() {
  //code to start timer and execution of threads

  std::map<int, Thread*>::iterator it;
  Thread *thread;
  struct itimerval it_val;

  for (it=threadMap.begin(); it != threadMap.end(); it++) {
  	thread = it->second;
  	thread->setState(READY);
  	readyQueue.push_back(thread->getID());
  	currentThread = readyQueue.end();
  }

  if (signal(SIGVTALRM, (void (*)(int)) dispatch) == SIG_ERR) {
    cout<<"Unable to catch SIGALRM"<<endl;
    exit(1);
  }

  it_val.it_value.tv_sec =     INTERVAL/1000;
  it_val.it_value.tv_usec =    (INTERVAL*1000) % 1000000;        
  it_val.it_interval = it_val.it_value;
  if (setitimer(ITIMER_VIRTUAL, &it_val, NULL) == -1) {
    cout<<"error calling setitimer()"<<endl;
    exit(1);
  }
  
  while(1) {}//infinite loop to stop termination of program

}

void dispatch(int signal) {
  if (readyQueue.size() > 0) {
  	if (currentThread != readyQueue.end()) {
  	  //save context
  		cout<<"Save thread context "<<*currentThread<<endl;
    }

    currentThread = (currentThread != readyQueue.end() && currentThread + 1 != readyQueue.end() ? ++currentThread : readyQueue.begin());
	int threadId = *currentThread;
  	std::map<int, Thread*>::iterator it;
    it = threadMap.find(threadId);

  	if (it != threadMap.end()) {
  	  //thread.run();
  	  cout<<"Run thread "<<threadId<<endl;
  	}
  }
}

int create(void (*functionPointer)(void)) {
  Thread* thread = new Thread(functionPointer);
  threadMap.insert(std::pair<int, Thread*>(thread->getID(),thread));
  return thread->getID();
}

int createWithArgs(void* (*functionPointer)(void*), void* arguments) {
  Thread* thread = new Thread(functionPointer, arguments);
  threadMap.insert(std::pair<int, Thread*>(thread->getID(),thread));
  return thread->getID();
}

void run(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    thread->setState(RUNNING);        ///// need for updateState() ??
    readyQueue.push_back(thread->getID());
    //update ready queue here
  }
}

void suspend(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    thread->setState(SUSPENDED);
  }

  //update ready queue here
  std::vector<int>::iterator threadIt;
  threadIt = readyQueue.begin();
  while(threadIt != readyQueue.end()) {
    if(*threadIt == threadId) {
      readyQueue.erase(threadIt);
    }

    threadIt++;
  }
}

void resume(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;

    if (thread->getState() == SUSPENDED) {
      thread->setState(READY);

      //update ready queue here
      readyQueue.push_back(threadId);
    }
  }
}

void deleteThread(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    threadMap.erase(it);
    std::vector<int>::iterator threadIt;
    threadIt = readyQueue.begin();
    while(threadIt != readyQueue.end())
    {
      if(*threadIt == threadId)
      {
        readyQueue.erase(threadIt);
      }
      threadIt++;
    }

    delete thread->getStatistics();
    delete thread;
    //update ready queue here
  }
}

statistics* getStatus(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    return thread->getStatistics();
  }

  return NULL;
}

void printStatus(int threadId)
{
  std::map<int,Thread*>::iterator it;
  it=threadMap.find(threadId);
  Thread *thread = it->second;
  cout<<"Thread Id: "<<thread->getID()<<endl;
  int threadState = thread->getState();

    switch(threadState) {
      case 0:
        cout<<"Thread State: RUNNING"<<endl;
        break;
      case 1:
        cout<<"Thread State: READY"<<endl;
        break;
      case 2:
        cout<<"Thread State: SLEEPING"<<endl;
        break;
      case 3:
        cout<<"Thread State: SUSPENDED"<<endl;
        break;
      case 4:
        cout<<"Thread State: CREATED"<<endl;
        break;
    }

    if (thread->getState() == CREATED) {
      cout<<"Thread Burst Count: N/A"<<endl;
      cout<<"Thread Total Execution Time: N/A"<<endl;
      cout<<"Thread Total Sleeping Time: N/A"<<endl;
      cout<<"Thread Average Execution Time Quantum: N/A"<<endl;
      cout<<"Thread Average Waiting Time: N/A"<<endl;
    } else {
      cout<<"Thread Burst Count: "<<thread->getBurstCount()<<endl;
      cout<<"Thread Total Execution Time: "<<thread->getTotalExecutionTime()<<endl;
      cout<<"Thread Total Sleeping Time: "<<thread->getTotalSleepingTime()<<endl;
      cout<<"Thread Average Execution Time Quantum: "<<thread->getAvgExecutionTimeQuantum()<<endl;
      cout<<"Thread Average Waiting Time: "<<thread->getAvgWaitingTime()<<endl;
    }
    cout<<endl;
  


}
void clean() {
  //stop timer
  std::map<int, Thread*>::iterator it;
  Thread* thread;
  int threadState;

  for(it = threadMap.begin(); it != threadMap.end(); it++) {
    thread = it->second;
    cout<<"Thread Id: "<<thread->getID()<<endl;
    threadState = thread->getState();

    switch(threadState) {
      case 0:
        cout<<"Thread State: RUNNING"<<endl;
        break;
      case 1:
        cout<<"Thread State: READY"<<endl;
        break;
      case 2:
        cout<<"Thread State: SLEEPING"<<endl;
        break;
      case 3:
        cout<<"Thread State: SUSPENDED"<<endl;
        break;
      case 4:
        cout<<"Thread State: CREATED"<<endl;
        break;
    }

    cout<<"Thread Burst Count: "<<thread->getBurstCount()<<endl;
    cout<<"Thread Total Execution Time: "<<thread->getTotalExecutionTime()<<endl;
    cout<<"Thread Total Sleeping Time: "<<thread->getTotalSleepingTime()<<endl;
    cout<<"Thread Average Execution Time Quantum: "<<thread->getAvgExecutionTimeQuantum()<<endl;
    cout<<"Thread Average Waiting Time: "<<thread->getAvgWaitingTime()<<endl;
    cout<<endl;

    delete thread->getStatistics();
    delete thread;
  }

  threadMap.clear();
  readyQueue.clear();
}