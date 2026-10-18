/* Entry: 0x004ad770; symbol: FUN_004ad770; body bytes: 126 */

void __cdecl FUN_004ad770(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  void *this;
  undefined4 *puVar4;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    uVar2 = move<>(param_1);
    this = (void *)move<>(8,uVar2);
    puVar4 = (undefined4 *)move<>(param_2);
    FUN_00413f50(this,*puVar4);
  }
  else {
    uVar2 = move<>(param_2);
    uVar3 = move<>(param_1);
    FUN_004ae7f0(uVar3,uVar2);
  }
  return;
}

