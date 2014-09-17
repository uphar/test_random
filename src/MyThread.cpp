#include "MyThread.h"
#include <iostream>
#include <ctime>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <setjmp.h>

#define INTERVAL 1

using namespace std;

struct function_thread_map {
  void (*signalHandler)(int);
  bool type_dispatcher;
};

std::map<int, Thread*> threadMap;
std::map<signal_time, int> sleepingQueue;
std::vector<int> readyQueue;
std::map<signal_time, function_thread_map*> signalHandlerMap;
signal_time nextDispatcherInvokeTime = 0;

void addSignalHandlerToMap(signal_time sec, void (*fptr)(int), bool isDispatch, int threadId = 0) {
  function_thread_map *f_t_map = new function_thread_map;
  f_t_map->signalHandler = fptr;
  f_t_map->type_dispatcher = isDispatch;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  signal_time execution_time = (signal_time)(tv.tv_sec + sec)*1000000 + tv.tv_usec;

  if (isDispatch) {
    std::map<signal_time, function_thread_map*>::iterator it;

    it = signalHandlerMap.find(nextDispatcherInvokeTime);

    if (it != signalHandlerMap.end()) {
      signalHandlerMap.erase(it);
    }

    nextDispatcherInvokeTime = execution_time;
  } else {
    sleepingQueue.insert(std::pair<signal_time, int>(execution_time, threadId));
  }

  signalHandlerMap.insert(std::pair<signal_time, function_thread_map*>(execution_time, f_t_map));
}

void initTimer() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  signal_time current_time = (signal_time)tv.tv_sec*1000000 + tv.tv_usec;
  // cout<<"Current time "<<current_time<<endl;
  // cout<<"Exec time "<<nextDispatcherInvokeTime<<endl;

  if(signalHandlerMap.size() > 0) {
    signal_time execution_time = signalHandlerMap.begin()->first;
    function_thread_map *f_t_map = signalHandlerMap.begin()->second;

    signalHandlerMap.erase(signalHandlerMap.begin());

    if (execution_time <= current_time) {
      f_t_map->signalHandler(0);
    } else {
      signal(SIGALRM, f_t_map->signalHandler);
      ualarm(execution_time - current_time, 0);
    }
  }
}

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

  addSignalHandlerToMap(INTERVAL, dispatch, true);
  initTimer();

  thread = threadMap.begin()->second;
  thread->setState(RUNNING);
  thread->thread_stat->noOfBursts++;
  siglongjmp(thread->environment, 1);
  cout<<"Ready queue "<<readyQueue.size()<<endl;
  
  while(1) {}//infinite loop to stop termination of program

}

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
        cout<<"RUNNING  "<<thread->getID()<<endl;
  	  }
  	}
  }
}

void dispatch(int sig) {
  if (readyQueue.size() > 0) {
    // cout<<"SWITCH: ret_val="<<ret_val<<endl;
    addSignalHandlerToMap(INTERVAL, dispatch, true);
    initTimer();
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

    if (thread->getState() == RUNNING || thread->getState() == READY || thread->getState() == TERMINATED) {
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
      readyQueue.pop_back();
      addSignalHandlerToMap(INTERVAL, dispatch, true);
      initTimer();
  	  resumeContext();
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
      addSignalHandlerToMap(INTERVAL, dispatch, true);
      initTimer();
  	  resumeContext();
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
      addSignalHandlerToMap(INTERVAL, dispatch, true);
      initTimer();
  	  resumeContext();
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

void sleep(int sec) {
  if (readyQueue.size() > 0) {
    std::map<int, Thread*>::iterator threaMapIt;
    threaMapIt = threadMap.find(*(readyQueue.begin()));

    if (threaMapIt != threadMap.end()) {
      Thread *thread = threaMapIt->second;

      if (thread->getState() == RUNNING) {
        saveContext();
        readyQueue.pop_back();
        struct timeval tv;
        gettimeofday(&tv, NULL);
        thread->sleepTime = (signal_time)tv.tv_sec*1000000 + tv.tv_usec;
        thread->thread_stat->totalRequestedSleepingTime += sec;
        thread->setState(SLEEPING);
        addSignalHandlerToMap(sec, wakeUp, false, thread->getID());
        initTimer();
        resumeContext();
      }
    }
  }
}

void wakeUp(int sig) {
  if (sleepingQueue.size() > 0) {
    std::map<int, Thread*>::iterator threaMapIt;
    threaMapIt = threadMap.find(sleepingQueue.begin()->second);
    sleepingQueue.erase(sleepingQueue.begin());

    if (threaMapIt != threadMap.end()) {
      Thread *thread = threaMapIt->second;

      if (thread->getState() == SLEEPING) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        thread->wakeUpTime = (signal_time)tv.tv_sec*1000000 + tv.tv_usec;
        thread->thread_stat->totalSleepingTime += (thread->wakeUpTime - thread->sleepTime)/1000000.0;
        thread->setState(READY);
        readyQueue.push_back(thread->getID());
      }
    }
  }

  initTimer();
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

    if (thread->thread_stat->totalRequestedSleepingTime == 0) {
      cout<<"Thread Total Requested Sleeping Time: N/A"<<endl;
    } else {
      cout<<"Thread Total Requested Sleeping Time: "<<thread->thread_stat->totalRequestedSleepingTime<<endl;
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
