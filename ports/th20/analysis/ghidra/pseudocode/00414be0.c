/* Entry: 0x00414be0; symbol: FUN_00414be0; body bytes: 125 */

void __thiscall FUN_00414be0(void *this,void *param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 uVar3;
  
  bVar1 = FUN_00414a80((int)param_1);
  if (!bVar1) {
    bVar1 = FUN_00414af0((int)param_1);
    if (bVar1) {
      piVar2 = (int *)Getimpl((int)param_1);
      uVar3 = (**(code **)(*piVar2 + 4))(this);
      Set(this,uVar3);
      FUN_00414d50(param_1);
    }
    else {
      uVar3 = Getimpl((int)param_1);
      Set(this,uVar3);
      Set(param_1,0);
    }
  }
  return;
}

