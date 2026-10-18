/* Entry: 0x004ab930; symbol: FUN_004ab930; body bytes: 54 */

void __thiscall FUN_004ab930(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x2e8) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x2e8));
  *(int *)((int)this + 0x2ec) = iVar1;
  return;
}

