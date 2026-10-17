/* Entry: 0x005345d0; symbol: FUN_005345d0; body bytes: 48 */

void __thiscall FUN_005345d0(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x7c) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x7c));
  *(int *)((int)this + 0x80) = iVar1;
  return;
}

