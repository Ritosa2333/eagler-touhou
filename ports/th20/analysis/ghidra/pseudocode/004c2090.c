/* Entry: 0x004c2090; symbol: FUN_004c2090; body bytes: 54 */

void __thiscall FUN_004c2090(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0xc458) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0xc458));
  *(int *)((int)this + 0xc45c) = iVar1;
  return;
}

