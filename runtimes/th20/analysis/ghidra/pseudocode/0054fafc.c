/* Entry: 0x0054fafc; symbol: FUN_0054fafc; body bytes: 34 */

uint __fastcall FUN_0054fafc(int param_1)

{
  __acrt_ptd *p_Var1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_1 + 8);
  if ((uVar2 != 0) && (uVar2 <= *(uint *)(param_1 + 4))) {
    return CONCAT31((int3)(uVar2 >> 8),1);
  }
  p_Var1 = FUN_005516c1();
  *(undefined4 *)p_Var1 = 0x16;
  uVar2 = FUN_005480bc();
  return uVar2 & 0xffffff00;
}

