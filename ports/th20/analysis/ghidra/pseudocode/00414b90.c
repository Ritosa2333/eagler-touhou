/* Entry: 0x00414b90; symbol: FUN_00414b90; body bytes: 71 */

void __thiscall FUN_00414b90(void *this,int param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  bVar1 = FUN_00414a80(param_1);
  if (!bVar1) {
    puVar2 = (undefined4 *)Getimpl(param_1);
    uVar3 = (**(code **)*puVar2)(this);
    Set(this,uVar3);
  }
  return;
}

