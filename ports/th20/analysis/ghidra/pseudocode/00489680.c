/* Entry: 0x00489680; symbol: FUN_00489680; body bytes: 162 */

void __cdecl FUN_00489680(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  void *this;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    uVar2 = move<>(param_1);
    this = (void *)move<>(0x58,uVar2);
    uVar2 = move<>(param_3);
    uVar3 = move<>(param_2);
    FUN_00488fb0(this,uVar3,uVar2);
  }
  else {
    uVar2 = move<>(param_3);
    uVar3 = move<>(param_2);
    uVar4 = move<>(param_1);
    FUN_0048aee0(uVar4,uVar3,uVar2);
  }
  return;
}

