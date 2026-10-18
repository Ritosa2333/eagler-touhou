/* Entry: 0x00414740; symbol: FUN_00414740; body bytes: 59 */

void __fastcall FUN_00414740(int param_1)

{
  bool bVar1;
  int *piVar2;
  
  bVar1 = FUN_00414a80(param_1);
  if (bVar1) {
    FUN_00540e98();
  }
  piVar2 = (int *)Getimpl(param_1);
  (**(code **)(*piVar2 + 8))();
  return;
}

