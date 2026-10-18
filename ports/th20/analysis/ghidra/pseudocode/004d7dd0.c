/* Entry: 0x004d7dd0; symbol: FUN_004d7dd0; body bytes: 54 */

void __thiscall FUN_004d7dd0(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x6ec) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x6ec));
  *(int *)((int)this + 0x6f0) = iVar1;
  return;
}

