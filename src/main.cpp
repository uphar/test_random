#include <iostream>
#include "MyThread.h"

using namespace std;
void f(void)
{
  cout<<"in function f..\n";
}
void g(void)
{
  cout<<"in function g..\n";
}
void h(void)
{
  cout<<"in function h..\n";
}
void i(void)
{
  cout<<"in function h..\n";
}
void j(void)
{
  cout<<"in function h..\n";
}
int main() {
  create(f);
  create(g);
  create(h);
  printStatus(0);
  printStatus(1);
  printStatus(2);
  start();
  printStatus(0);
  printStatus(1);
  printStatus(2);
  suspend(0);
  printStatus(0);
  resume(0);
  printStatus(0);
  
}
