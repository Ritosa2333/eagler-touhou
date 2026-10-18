/* Entry: 0x00449630; symbol: FUN_00449630; body bytes: 83 */

void __thiscall FUN_00449630(void *this,undefined4 param_1)

{
  code *pcVar1;
  bool bVar2;
  int *piVar3;
  undefined4 uVar4;
  
  bVar2 = FUN_00414a80((int)this);
  if (bVar2) {
    FUN_00540e98();
  }
  piVar3 = (int *)Getimpl((int)this);
  pcVar1 = *(code **)(*piVar3 + 8);
  uVar4 = move<>(param_1);
  (*pcVar1)(uVar4);
  return;
}

