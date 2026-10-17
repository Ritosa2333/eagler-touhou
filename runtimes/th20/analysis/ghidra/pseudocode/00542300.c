/* Entry: 0x00542300; symbol: _Ios_base_dtor; body bytes: 64 */

/* Library Function - Single Match
    private: static void __cdecl std::ios_base::_Ios_base_dtor(class std::ios_base *)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl std::ios_base::_Ios_base_dtor(ios_base *param_1)

{
  int iVar1;
  LPVOID pvVar2;
  
  iVar1 = *(int *)(param_1 + 8);
  if ((iVar1 == 0) ||
     ((&DAT_005e4e60)[iVar1] = (&DAT_005e4e60)[iVar1] + -1, (char)(&DAT_005e4e60)[iVar1] < '\x01'))
  {
    FUN_00542340(param_1);
    pvVar2 = *(LPVOID *)(param_1 + 0x30);
    if (pvVar2 != (LPVOID)0x0) {
      FUN_004f5600((int)pvVar2);
      FUN_0054278d(pvVar2);
    }
  }
  return;
}

