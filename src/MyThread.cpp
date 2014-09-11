#include "MyThread.h"
#include<iostream>
using namespace std;
void start() {
  //code to start timer and execution of threads

  void (*functionPointer)(void);
  std::vector<Thread*>::iterator it;
  Thread *thread;
  it=readyQueue.begin();
  while(1)
  {
    thread=*it;
    functionPointer=thread->getFunctionPointer();
    functionPointer();
    it++;
  }

}

int create(void (*functionPointer)(void))
{
  Thread* thread=new Thread(functionPointer);
  threadMap.insert( std::pair<int, Thread*>(thread->getID(),thread) );
  readyQueue.push_back(thread);
  return thread->getID();
}

void run(int threadId) {
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    thread->setState(RUNNING);        ///// need for updateState() ??
    readyQueue.push_back(thread);
    //update ready queue here
  }
}

void suspend(int threadId) {
  int position=0;
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    thread->setState(SUSPENDED);
    
    //update ready queue here
    std::vector<Thread*>::iterator threadIt;
    threadIt=readyQueue.begin();
    while(threadIt!=readyQueue.end())
    {
      position++;
      if((*threadIt)->getID()!=thread->getID())
      {
        readyQueue.erase(threadIt);
      }
      threadIt++;
    }
  }
}

void resume(int threadId) {
  int position=0;
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;

    if (thread->getState() == SUSPENDED) {
      thread->setState(READY);
     
     //update ready queue here
     readyQueue.push_back(thread);
    }
  }
}

void deleteThread(int threadId) {
  int position=0;
  std::map<int, Thread*>::iterator it;
  it = threadMap.find(threadId);

  if (it != threadMap.end()) {
    Thread* thread = it->second;
    threadMap.erase(it);
    std::vector<Thread*>::iterator threadIt;
    threadIt=readyQueue.begin();
    while(threadIt!=readyQueue.end())
    {
      position++;
      if((*threadIt)->getID()!=thread->getID())
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
