/* Entry: 0x0047aab0; symbol: FUN_0047aab0; body bytes: 124 */

void __cdecl FUN_0047aab0(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  void *this;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    uVar2 = move<>(param_1);
    this = (void *)move<>(0x58,uVar2);
    uVar2 = move<>(param_2);
    FUN_0047a660(this,uVar2);
  }
  else {
    uVar2 = move<>(param_2);
    uVar3 = move<>(param_1);
    FUN_0047b5b0(uVar3,uVar2);
  }
  return;
}

