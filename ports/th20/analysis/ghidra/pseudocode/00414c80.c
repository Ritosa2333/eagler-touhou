/* Entry: 0x00414c80; symbol: Swap; body bytes: 188 */

/* Library Function - Multiple Matches With Same Base Name
    protected: void __thiscall std::_Func_class<void>::_Swap(class std::_Func_class<void> &)
    protected: void __thiscall std::_Func_class<void,class Concurrency::message<unsigned int>
   *>::_Swap(class std::_Func_class<void,class Concurrency::message<unsigned int> *> &)
    protected: void __thiscall std::_Func_class<void,class Concurrency::message<enum
   Concurrency::agent_status> *>::_Swap(class std::_Func_class<void,class Concurrency::message<enum
   Concurrency::agent_status> *> &)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall Swap(void *this,void *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  void *pvVar4;
  undefined1 local_30 [40];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00414af0((int)this);
  if (!bVar1) {
    bVar1 = FUN_00414af0((int)param_1);
    if (!bVar1) {
      uVar2 = Getimpl((int)this);
      uVar3 = Getimpl((int)param_1);
      Set(this,uVar3);
      Set(param_1,uVar2);
      goto LAB_00414d2c;
    }
  }
  _Func_class<>(local_30);
  pvVar4 = (void *)move<>(this);
  FUN_00414be0(local_30,pvVar4);
  pvVar4 = (void *)move<>(param_1);
  FUN_00414be0(this,pvVar4);
  pvVar4 = (void *)move<>(local_30);
  FUN_00414be0(param_1,pvVar4);
  FUN_00414310(local_30);
LAB_00414d2c:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

