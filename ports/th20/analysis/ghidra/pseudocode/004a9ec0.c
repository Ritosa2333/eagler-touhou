/* Entry: 0x004a9ec0; symbol: FUN_004a9ec0; body bytes: 83 */

void __thiscall FUN_004a9ec0(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  *(int *)((int)this + 100) = *(int *)((int)this + 100) + param_1;
  iVar1 = FUN_004aafa0();
  if (iVar1 <= *(int *)((int)this + 100)) {
    uVar2 = FUN_004aafa0();
    *(undefined4 *)((int)this + 100) = uVar2;
    if (*(int *)((int)this + 0x98) < 0) {
      *(undefined4 *)((int)this + 0x98) = 0;
    }
  }
  return;
}

