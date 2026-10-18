/* Entry: 0x00414d50; symbol: FUN_00414d50; body bytes: 97 */

void __fastcall FUN_00414d50(void *param_1)

{
  code *pcVar1;
  bool bVar2;
  int *piVar3;
  bool local_5;
  
  bVar2 = FUN_00414a80((int)param_1);
  if (!bVar2) {
    piVar3 = (int *)Getimpl((int)param_1);
    pcVar1 = *(code **)(*piVar3 + 0x10);
    local_5 = FUN_00414af0((int)param_1);
    local_5 = !local_5;
    (*pcVar1)(local_5);
    Set(param_1,0);
  }
  return;
}

