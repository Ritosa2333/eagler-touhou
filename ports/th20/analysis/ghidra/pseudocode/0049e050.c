/* Entry: 0x0049e050; symbol: FUN_0049e050; body bytes: 45 */

void __thiscall FUN_0049e050(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x1c) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x1c));
  *(int *)((int)this + 0x20) = iVar1;
  return;
}

