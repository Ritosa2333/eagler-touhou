/* Entry: 0x004ffe80; symbol: FUN_004ffe80; body bytes: 54 */

void __thiscall FUN_004ffe80(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x124) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x124));
  *(int *)((int)this + 0x128) = iVar1;
  return;
}

