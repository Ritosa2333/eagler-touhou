/* Entry: 0x0049e820; symbol: FUN_0049e820; body bytes: 65 */

void __thiscall FUN_0049e820(void *this,undefined4 param_1)

{
  char cVar1;
  undefined4 uVar2;
  
  cVar1 = _Test_callable<>();
  if (cVar1 != '\0') {
    uVar2 = move<>(param_1);
    uVar2 = FUN_0049e550(uVar2);
    Set(this,uVar2);
  }
  return;
}

