/* Entry: 0x005331e0; symbol: FUN_005331e0; body bytes: 198 */

void __thiscall FUN_005331e0(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  bool bVar1;
  int iVar2;
  void *this_00;
  
  (**(code **)(**(int **)((int)this + 0x28) + 8))(param_1,param_2,param_3);
  iVar2 = FUN_00460830(0);
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    (**(code **)(**(int **)((int)this + 0x2c) + 0xc))(param_1,param_2,param_3);
  }
  else {
    (**(code **)(**(int **)((int)this + 0x30) + 0x10))(param_1,param_2,param_3);
  }
  iVar2 = FUN_00460830(0);
  this_00 = (void *)FUN_004ff6b0(iVar2);
  FUN_00534700(this_00,0);
  return;
}

