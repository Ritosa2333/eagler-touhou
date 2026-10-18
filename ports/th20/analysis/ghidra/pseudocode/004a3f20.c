/* Entry: 0x004a3f20; symbol: FUN_004a3f20; body bytes: 83 */

void __fastcall FUN_004a3f20(int param_1)

{
  code *pcVar1;
  bool bVar2;
  int *piVar3;
  undefined4 uVar4;
  
  bVar2 = FUN_00414a80(param_1);
  if (bVar2) {
    FUN_00540e98();
  }
  piVar3 = (int *)Getimpl(param_1);
  pcVar1 = *(code **)(*piVar3 + 8);
  uVar4 = move<>(&stack0x00000004);
  (*pcVar1)(uVar4);
  return;
}

