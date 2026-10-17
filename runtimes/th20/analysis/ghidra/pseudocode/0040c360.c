/* Entry: 0x0040c360; symbol: FUN_0040c360; body bytes: 111 */

void __fastcall FUN_0040c360(undefined4 *param_1)

{
  bool bVar1;
  DWORD DVar2;
  int iVar3;
  
  bVar1 = FUN_0040c3f0((int)param_1);
  if (!bVar1) {
    std::_Throw_Cpp_error(1);
  }
  DVar2 = GetCurrentThreadId();
  if (param_1[1] == DVar2) {
    std::_Throw_Cpp_error(5);
  }
  iVar3 = FUN_00540a2d((HANDLE)*param_1,param_1[1],(DWORD *)0x0);
  if (iVar3 != 0) {
    std::_Throw_Cpp_error(2);
  }
  *param_1 = 0;
  param_1[1] = 0;
  return;
}

