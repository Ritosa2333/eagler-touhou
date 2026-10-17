/* Entry: 0x004ab8b0; symbol: FUN_004ab8b0; body bytes: 54 */

void __thiscall FUN_004ab8b0(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x234) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x234));
  *(int *)((int)this + 0x238) = iVar1;
  return;
}

