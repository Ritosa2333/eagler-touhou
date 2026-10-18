/* Entry: 0x004d7e10; symbol: FUN_004d7e10; body bytes: 45 */

void __thiscall FUN_004d7e10(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x50) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x50));
  *(int *)((int)this + 0x54) = iVar1;
  return;
}

