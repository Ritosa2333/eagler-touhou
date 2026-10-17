/* Entry: 0x00540cef; symbol: _Throw_Cpp_error; body bytes: 53 */

/* Library Function - Single Match
    void __cdecl std::_Throw_Cpp_error(int)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release */

void __cdecl std::_Throw_Cpp_error(int param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  uint *puVar3;
  int local_18 [5];
  
  puVar3 = (uint *)(&PTR_s_device_or_resource_busy_00576cb4)[param_1];
  uVar2 = FUN_00411160();
  FUN_00540c9f(local_18,*(undefined4 *)(&DAT_00576cd0 + param_1 * 4),uVar2,puVar3);
  FUN_00544a7c(local_18,&DAT_005a96b8);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

