/* Entry: 0x0040c180; symbol: FUN_0040c180; body bytes: 88 */

void __fastcall FUN_0040c180(undefined4 *param_1)

{
  bool bVar1;
  int iVar2;
  
  bVar1 = FUN_0040c3f0((int)param_1);
  if (!bVar1) {
    std::_Throw_Cpp_error(1);
  }
  iVar2 = FUN_00540a0f((HANDLE)*param_1);
  if (iVar2 != 0) {
    std::_Throw_Cpp_error(1);
  }
  *param_1 = 0;
  param_1[1] = 0;
  return;
}

