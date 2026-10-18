/* Entry: 0x005330a0; symbol: FUN_005330a0; body bytes: 217 */

void __thiscall FUN_005330a0(void *this,char param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  
  if (param_1 == '\0') {
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_00411700(iVar2);
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_0041ca90(iVar3);
    if (iVar2 != iVar3) {
      iVar2 = FUN_00460830(0);
      bVar1 = FUN_004ff5e0(iVar2);
      if (bVar1) {
        (**(code **)(**(int **)((int)this + 0x30) + 0x24))();
      }
      else {
        (**(code **)(**(int **)((int)this + 0x2c) + 0x20))();
      }
    }
  }
  else {
    (**(code **)(**(int **)((int)this + 0x28) + 0x1c))();
    iVar2 = FUN_00460830(0);
    bVar1 = FUN_004ff5e0(iVar2);
    if (bVar1) {
      (**(code **)(**(int **)((int)this + 0x2c) + 0x20))();
    }
    else {
      (**(code **)(**(int **)((int)this + 0x30) + 0x24))();
    }
  }
  return;
}

