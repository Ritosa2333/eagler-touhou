/* Entry: 0x00542a0a; symbol: thunk_FUN_00557ba0; body bytes: 5 */

void __cdecl thunk_FUN_00557ba0(LPVOID param_1)

{
  BOOL BVar1;
  DWORD DVar2;
  uint uVar3;
  __acrt_ptd *p_Var4;
  
  if (param_1 != (LPVOID)0x0) {
    BVar1 = HeapFree(DAT_005e5990,0,param_1);
    if (BVar1 == 0) {
      DVar2 = GetLastError();
      uVar3 = FUN_00551606(DVar2);
      p_Var4 = FUN_005516c1();
      *(uint *)p_Var4 = uVar3;
    }
  }
  return;
}

