#include "MyThread.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <setjmp.h>

#define INTERVAL 1

using namespace std;

std::map<int, Thread*> threadMap;
std::vector<int> readyQueue;

void start() {
  //code to start timer and execution of threads

  std::map<int, Thread*>::iterator it;
  Thread *thread;

  for (it=threadMap.begin(); it != threadMap.end(); it++) {
  	thread = it->second;
  	thread->setState(READY);
  	readyQueue.push_back(thread->getID());
  	cout<<"pushed::"<<thread->getID()<<endl;
  }

  signal(SIGALRM, dispatch);
  alarm(INTERVAL);

  thread = threadMap.begin()->second;
  thread->setState(RUNNING);
  thread->thread_stat->noOfBursts++;
  siglongjmp(thread->environment, 1);
  cout<<"Ready queue "<<readyQueue.size()<<endl;
  
  while(1) {}//infinite loop to stop termination of program

}

/*
void initTimer() {
  struct itimerval it_val;

  //if (signal(SIGVTALRM, (void (*)(int)) alarm_handl) == SIG_ERR) {
  if (signal(SIGVTALRM, alarm_handl) == SIG_ERR) {
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
}
*/

void displayReadyQueue()
{
  std::vector<int>::iterator it;
  it = readyQueue.begin();
  while(it!=readyQueue.end())
  {
    cout<<(*it)<<endl;
    it++;
  }
}

int saveContext() {
  if (readyQueue.size() > 0) {
  	int threadId = *readyQueue.begin();
  	cout<<"Save thread context "<<threadId<<endl;
  	Thread *thread = threadMap.find(threadId)->second;
  	readyQueue.push_back(threadId);
  	readyQueue.erase(readyQueue.begin());

  	if (thread->getState() == RUNNING) {
  	  thread->setState(READY);
      int ret_val = sigsetjmp(thread->environment,1);

      if (ret_val == 1) {
        return -1;
      } else {
        return ret_val;
      }
  	}
  }
}

void resumeContext() {
  if (readyQueue.size() > 0) {
    int threadId = *(readyQueue.begin());
  	std::map<int, Thread*>::iterator it;
    it = threadMap.find(threadId);

  	if (it != threadMap.end()) {
  	  Thread *thread = it->second;
  	  if (thread->getState() == READY) {
  	    thread->thread_stat->noOfBursts++;
  	    thread->setState(RUNNING);
        siglongjmp(thread->environment,1);
  	  }
  	}
  }
}

void dispatch(int sig) {
  if (readyQueue.size() > 0) {
    // cout<<"SWITCH: ret_val="<<ret_val<<endl;
    alarm(1);
    signal(SIGALRM, dispatch);
    if (saveContext() == -1) {
      return;
    }

    resumeContext();
    // cout<<endl<<"switching to other function "<<*readyQueue.begin()<<endl;
  	//initTimer();
  }
  // else {
  // 	cout<<"nothing to do"<<endl;
  // }
}

void yield() {
  dispatch(0);
}

int create(void (*functionPointer)(void)) {
  Thread* thread = new Thread(functionPointer);
  threadMap.insert(std::pair<int, Thread*>(thread->getID(),thread));
  //if status is made ready in constructor then update map and queue here
  return thread->getID();
}

int createWithArgs(void* (*functionPointer)(void*), void* arguments) {
  Thread* thread = new Thread(functionPointer, arguments);
  threadMap.insert(std::pair<int, Thread*>(thread->getID(),thread));
  return thread->getID();
}

void run(int threadId) {
  std::map<int, Thread*>::iterator threaMapIt;
  threaMapIt = threadMap.find(threadId);

  if (threaMapIt != threadMap.end()) {
    Thread *thread = threaMapIt->second;

    if (thread->getState() == RUNNING || thread->getState() == READY) {
      return;
    }

  	thread->setState(READY);
    readyQueue.push_back(threadId);
  }
}

void suspend(int threadId) {
  std::vector<int>::iterator it;
  for (it = readyQueue.begin(); it != readyQueue.end(); it++) {
    if (*it == threadId) {
      break;
    }
  }

  if (it != readyQueue.end()) {
  	if (it == readyQueue.begin()) {
  	  saveContext();
  	  resumeContext(); //check if we need to inittimer here again to reset
  	  readyQueue.pop_back();
  	} else {
  	  readyQueue.erase(it);
  	}
  }

  std::map<int, Thread*>::iterator threaMapIt;
  threaMapIt = threadMap.find(threadId);

  if (threaMapIt != threadMap.end()) {
    Thread* thread = threaMapIt->second;
    thread->setState(SUSPENDED);
  }
}

void resume(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;

    if (thread->getState() == SUSPENDED) {
      thread->setState(READY);
      readyQueue.push_back(threadId);
    }
  }
}

void deleteThread(int threadId) {
  std::vector<int>::iterator it;
  for (it = readyQueue.begin(); it != readyQueue.end(); it++) {
    if (*it == threadId) {
      break;
    }
  }

  if (it != readyQueue.end()) {
  	if (it == readyQueue.begin()) {
  	  readyQueue.erase(readyQueue.begin());
  	  resumeContext(); //check if we need to inittimer here again to reset
  	} else {
  	  readyQueue.erase(it);
  	}
  }

  std::map<int, Thread*>::iterator threaMapIt;
  threaMapIt = threadMap.find(threadId);

  if (threaMapIt != threadMap.end()) {
    Thread* thread = threaMapIt->second;
    threadMap.erase(threaMapIt);
    delete thread->getStatistics();
    delete thread->cStack;
    delete thread;
  }
}

void terminate(int threadId) {
  std::vector<int>::iterator it;
  for (it = readyQueue.begin(); it != readyQueue.end(); it++) {
    if (*it == threadId) {
      break;
    }
  }

  if (it != readyQueue.end()) {
  	if (it == readyQueue.begin()) {
  	  readyQueue.erase(readyQueue.begin());
  	  resumeContext(); //check if we need to inittimer here again to reset
  	  // cout<<"in terminate"<<endl;

  	  // readyQueue.pop_back();
  	} else {
  	  readyQueue.erase(it);
  	}
  }

  std::map<int, Thread*>::iterator threaMapIt;
  threaMapIt = threadMap.find(threadId);

  if (threaMapIt != threadMap.end()) {
    Thread* thread = threaMapIt->second;
    thread->setState(TERMINATED);
  }

  // cout<<"in terminate"<<endl;
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
    delete thread->cStack;
    delete thread;
  }

  threadMap.clear();
  readyQueue.clear();
}
