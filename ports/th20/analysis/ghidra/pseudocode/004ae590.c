/* Entry: 0x004ae590; symbol: FUN_004ae590; body bytes: 65 */

void __thiscall FUN_004ae590(void *this,undefined4 param_1)

{
  char cVar1;
  undefined4 uVar2;
  
  cVar1 = _Test_callable<>();
  if (cVar1 != '\0') {
    uVar2 = move<>(param_1);
    uVar2 = FUN_004ad8e0(uVar2);
    Set(this,uVar2);
  }
  return;
}

