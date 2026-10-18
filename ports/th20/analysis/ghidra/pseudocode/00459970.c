/* Entry: 0x00459970; symbol: `scalar_deleting_destructor'; body bytes: 49 */

/* Library Function - Single Match
    public: void * __thiscall Concurrency::details::SchedulingNode::`scalar deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall
Concurrency::details::SchedulingNode::_scalar_deleting_destructor_
          (SchedulingNode *this,uint param_1)

{
  FUN_004597b0((undefined4 *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

