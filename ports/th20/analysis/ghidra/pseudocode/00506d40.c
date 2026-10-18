/* Entry: 0x00506d40; symbol: FUN_00506d40; body bytes: 54 */

void __thiscall FUN_00506d40(void *this,undefined4 param_1)

{
  int iVar1;
  
  *(undefined4 *)((int)this + 0x12590) = param_1;
  iVar1 = FUN_0040bbc0(&DAT_005ba568,*(int *)((int)this + 0x12590));
  *(int *)((int)this + 0x12594) = iVar1;
  return;
}

