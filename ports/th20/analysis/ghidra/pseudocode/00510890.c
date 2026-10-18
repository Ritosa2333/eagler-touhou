/* Entry: 0x00510890; symbol: FUN_00510890; body bytes: 54 */

void __thiscall FUN_00510890(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0xf90) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0xf90));
  *(int *)((int)this + 0xf94) = iVar1;
  return;
}

