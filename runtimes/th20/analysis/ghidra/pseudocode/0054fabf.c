/* Entry: 0x0054fabf; symbol: FUN_0054fabf; body bytes: 28 */

uint __fastcall FUN_0054fabf(int param_1)

{
  undefined4 in_EAX;
  __acrt_ptd *p_Var1;
  uint uVar2;
  
  if (*(int *)(param_1 + 8) == 0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    uVar2 = FUN_005480bc();
    return uVar2 & 0xffffff00;
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

