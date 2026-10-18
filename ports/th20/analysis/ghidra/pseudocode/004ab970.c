/* Entry: 0x004ab970; symbol: FUN_004ab970; body bytes: 81 */

void __thiscall FUN_004ab970(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x420) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x420));
  *(int *)((int)this + 0x424) = iVar1;
  FUN_004ab930((void *)((int)this + 0x88),param_1);
  return;
}

