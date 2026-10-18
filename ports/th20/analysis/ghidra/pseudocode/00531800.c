/* Entry: 0x00531800; symbol: FUN_00531800; body bytes: 80 */

void __thiscall FUN_00531800(void *this,int param_1,int param_2,int param_3)

{
  int iVar1;
  void *this_00;
  
  if (*(char *)((int)this + 0x34) == '\0') {
    iVar1 = FUN_00460830(0);
    this_00 = (void *)FUN_004ff6b0(iVar1);
    FUN_00504d40(this_00,param_1,param_2,*(int *)((int)this + 4) * 0x14 + param_3);
  }
  return;
}

