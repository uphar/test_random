#include <iostream>
#include "MyThread.h"
#define SECOND 1000000
using namespace std;
void f(void)
{
	int a=0;
	cout<<"in function f:"<<endl;
	
	while(1)
	{
	  a++;
	  cout<<dec<<a<<endl;
	  usleep(SECOND);
	}
}
void g(void)
{
	int a=10;
	cout<<"in function g"<<endl;
	while(1)
	{
	  a++;
	  cout<<dec<<a<<endl;
	  usleep(SECOND);
	}
}
void h(void)
{
	int a=20;
	cout<<"in function h"<<endl;
	while(1)
	{
	  a++;
	  cout<<dec<<a<<endl;
	  usleep(SECOND);
	}
}
void i(void)
{
        int a=30;
	cout<<"in function i"<<endl;
	while(1)
	{
	  a++;
	  cout<<dec<<a<<endl;
	  usleep(SECOND);
	}
}
void j(void)
{
        int a=40;
	cout<<"in function j"<<endl;
	while(1)
	{
	  a++;
	  cout<<dec<<a<<endl;
	  usleep(SECOND);
	}
}
int main() {
  
  create(f);
  create(g);
  create(h);
  create(i);
  create(j);
  // printStatus(0);
  // printStatus(1);
  // printStatus(2);
  start();
  // printStatus(0);
  // printStatus(1);
  // printStatus(2);
  // suspend(0);
  // printStatus(0);
  // resume(0);
  // printStatus(0);
  
}
