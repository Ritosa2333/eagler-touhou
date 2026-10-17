/* Entry: 0x004c4f90; symbol: FUN_004c4f90; body bytes: 54 */

void __thiscall FUN_004c4f90(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0xc44) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0xc44));
  *(int *)((int)this + 0xc48) = iVar1;
  return;
}

