/* Entry: 0x0055f730; symbol: FUN_0055f730; body bytes: 151 */

LPVOID __cdecl FUN_0055f730(LPCVOID param_1,uint param_2)

{
  bool bVar1;
  __acrt_ptd *p_Var2;
  SIZE_T SVar3;
  LPVOID pvVar4;
  DWORD DVar5;
  uint uVar6;
  
  if (param_1 == (LPCVOID)0x0) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    pvVar4 = (LPVOID)0x0;
  }
  else if (param_2 < 0xffffffe1) {
    SVar3 = HeapSize(DAT_005e5990,0,param_1);
    if (param_2 == 0) {
      param_2 = 1;
    }
    pvVar4 = HeapReAlloc(DAT_005e5990,0x10,param_1,param_2);
    if ((pvVar4 == (LPVOID)0x0) &&
       ((SVar3 < param_2 || (bVar1 = is_contraction_possible(SVar3), pvVar4 = param_1, bVar1)))) {
      DVar5 = GetLastError();
      uVar6 = FUN_00551606(DVar5);
      p_Var2 = FUN_005516c1();
      *(uint *)p_Var2 = uVar6;
      pvVar4 = (LPVOID)0x0;
    }
  }
  else {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0xc;
    pvVar4 = (LPVOID)0x0;
  }
  return pvVar4;
}

