/* Entry: 0x005422cd; symbol: _Callfns; body bytes: 51 */

/* Library Function - Single Match
    private: void __thiscall std::ios_base::_Callfns(enum std::ios_base::event)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall std::ios_base::_Callfns(ios_base *this,event param_1)

{
  undefined4 *puVar1;
  code *pcVar2;
  
  for (puVar1 = *(undefined4 **)(this + 0x2c); puVar1 != (undefined4 *)0x0;
      puVar1 = (undefined4 *)*puVar1) {
    pcVar2 = (code *)puVar1[2];
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,this,puVar1[1]);
    (*pcVar2)();
  }
  return;
}

