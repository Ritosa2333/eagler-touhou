/* Entry: 0x005584c0; symbol: FUN_005584c0; body bytes: 81 */

LPVOID __cdecl FUN_005584c0(SIZE_T param_1)

{
  bool bVar1;
  int iVar2;
  undefined3 extraout_var;
  LPVOID pvVar3;
  __acrt_ptd *p_Var4;
  
  if (param_1 < 0xffffffe1) {
    if (param_1 == 0) {
      param_1 = 1;
    }
    do {
      pvVar3 = HeapAlloc(DAT_005e5990,0,param_1);
      if (pvVar3 != (LPVOID)0x0) {
        return pvVar3;
      }
      iVar2 = FUN_005571c0();
    } while ((iVar2 != 0) && (bVar1 = FUN_00552860(param_1), CONCAT31(extraout_var,bVar1) != 0));
  }
  p_Var4 = FUN_005516c1();
  *(undefined4 *)p_Var4 = 0xc;
  return (LPVOID)0x0;
}

