/* Entry: 0x004ab8f0; symbol: FUN_004ab8f0; body bytes: 54 */

void __thiscall FUN_004ab8f0(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 300) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 300));
  *(int *)((int)this + 0x130) = iVar1;
  return;
}

