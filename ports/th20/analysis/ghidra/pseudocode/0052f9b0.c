/* Entry: 0x0052f9b0; symbol: FUN_0052f9b0; body bytes: 139 */

void __cdecl FUN_0052f9b0(undefined4 *param_1,int *param_2,uint param_3)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  p_Var1 = FUN_005516c1();
  iVar2 = FUN_0040ff30(param_1);
  *(undefined4 *)p_Var1 = 0;
  FUN_005516d6(iVar2,&local_c,param_3);
  if (iVar2 == local_c) {
    FUN_00540eb5(0x5756a4);
  }
  if (*(int *)p_Var1 == 0x22) {
    std::_Xout_of_range("stoi argument out of range");
  }
  if (param_2 != (int *)0x0) {
    *param_2 = local_c - iVar2;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

