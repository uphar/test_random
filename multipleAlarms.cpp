#include <signal.h>
#include<iostream>
#include <stdlib.h>
#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>

volatile sig_atomic_t keep_going = 1;
class alarmHandler
{
        public:
        int i;
        int arr[10];
        alarmHandler()
        {
                i=0;
                arr[0]=2;
                arr[1]=3;
                arr[2]=4;  
                arr[3]=5;  
                arr[4]=6;  
                arr[5]=7;              
        }
        
        
};
alarmHandler A;
void f()
{
        if(A.i==6)
                printf("comes here\n");
}
void catch_alarm(int sig)
        {
                A.i++;
                if(A.i==6)
                {       
                      keep_going=0;
                      return;
                }
                alarm(A.arr[A.i]-A.arr[A.i-1]);
                signal (sig, catch_alarm);
                printf("alarm! \n");
                f();
        }

void dispatch()
{
        signal (SIGALRM, catch_alarm);
        alarm (1);
        while(keep_going)
                f();
        printf("end of program\n");


}


int main (void)
{
  dispatch();
  return EXIT_SUCCESS;
}
