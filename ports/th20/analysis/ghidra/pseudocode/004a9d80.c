/* Entry: 0x004a9d80; symbol: FUN_004a9d80; body bytes: 83 */

void __thiscall FUN_004a9d80(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  *(int *)((int)this + 0x68) = *(int *)((int)this + 0x68) + param_1;
  iVar1 = FUN_004aafa0();
  if (iVar1 <= *(int *)((int)this + 0x68)) {
    uVar2 = FUN_004aafa0();
    *(undefined4 *)((int)this + 0x68) = uVar2;
    if (*(int *)((int)this + 0x98) < 0) {
      *(undefined4 *)((int)this + 0x98) = 1;
    }
  }
  return;
}

