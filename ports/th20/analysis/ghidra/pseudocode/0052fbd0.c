/* Entry: 0x0052fbd0; symbol: FUN_0052fbd0; body bytes: 174 */

void __thiscall FUN_0052fbd0(void *this,int param_1,int param_2,int param_3)

{
  int iVar1;
  void *pvVar2;
  
  if (*(char *)((int)this + 0x34) == '\0') {
    iVar1 = FUN_00464230(0);
    FUN_00530580(iVar1);
    iVar1 = FUN_00460830(0);
    pvVar2 = (void *)FUN_004ff6b0(iVar1);
    FUN_00504d40(pvVar2,param_1,param_2,*(int *)((int)this + 4) * 0x14 + param_3);
  }
  else {
    iVar1 = FUN_00464230(0);
    FUN_005305a0(iVar1);
    iVar1 = FUN_00460830(0);
    pvVar2 = (void *)FUN_004ff6b0(iVar1);
    FUN_00504d40(pvVar2,param_1,param_2,*(int *)((int)this + 4) * 0x14 + 0xf + param_3);
  }
  return;
}

