/* Entry: 0x00530750; symbol: FUN_00530750; body bytes: 140 */

void __thiscall FUN_00530750(void *this,int param_1,int param_2,int param_3)

{
  int iVar1;
  void *pvVar2;
  
  if (*(char *)((int)this + 0x34) == '\0') {
    iVar1 = FUN_00460830(0);
    pvVar2 = (void *)FUN_004ff6b0(iVar1);
    FUN_00504d40(pvVar2,param_1,param_2,*(int *)((int)this + 4) * 0x14 + param_3);
  }
  else {
    iVar1 = FUN_00460830(0);
    pvVar2 = (void *)FUN_004ff6b0(iVar1);
    FUN_00504d40(pvVar2,param_1,param_2,*(int *)((int)this + 4) * 0x14 + 0xf + param_3);
  }
  return;
}

