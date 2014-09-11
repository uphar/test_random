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

int main() {
  create(f);
  create(g);
  create(h);
  start();
  
}
