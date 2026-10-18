/* Entry: 0x00488a20; symbol: FUN_00488a20; body bytes: 54 */

void __thiscall FUN_00488a20(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0xbc) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0xbc));
  *(int *)((int)this + 0xc0) = iVar1;
  return;
}

