/* Entry: 0x00506d80; symbol: FUN_00506d80; body bytes: 84 */

void __thiscall FUN_00506d80(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x118) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x118));
  *(int *)((int)this + 0x120) = iVar1;
  iVar1 = FUN_0040ff90(*(int *)((int)this + 0x120));
  iVar1 = FUN_004ff6b0(iVar1);
  *(int *)((int)this + 0x11c) = iVar1;
  return;
}

