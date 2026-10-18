/* Entry: 0x0054fadb; symbol: FUN_0054fadb; body bytes: 33 */

uint __fastcall FUN_0054fadb(int *param_1)

{
  undefined4 in_EAX;
  __acrt_ptd *p_Var1;
  uint uVar2;
  
  if ((*param_1 != 0) && (param_1[6] != 0)) {
    return CONCAT31((int3)((uint)in_EAX >> 8),1);
  }
  p_Var1 = FUN_005516c1();
  *(undefined4 *)p_Var1 = 0x16;
  uVar2 = FUN_005480bc();
  return uVar2 & 0xffffff00;
}

