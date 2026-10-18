/* Entry: 0x00486570; symbol: FUN_00486570; body bytes: 54 */

void __thiscall FUN_00486570(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x51c) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x51c));
  *(int *)((int)this + 0x520) = iVar1;
  return;
}

