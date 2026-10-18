/* Entry: 0x00414c60; symbol: Set; body bytes: 30 */

/* Library Function - Multiple Matches With Same Base Name
    private: void __thiscall std::_Func_class<void,class Concurrency::message<unsigned int>
   *>::_Set(class std::_Func_base<void,class Concurrency::message<unsigned int> *> *)
    private: void __thiscall std::_Func_class<void,class Concurrency::message<enum
   Concurrency::agent_status> *>::_Set(class std::_Func_base<void,class Concurrency::message<enum
   Concurrency::agent_status> *> *)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall Set(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x24) = param_1;
  return;
}

