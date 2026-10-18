/* Entry: 0x0040c590; symbol: FUN_0040c590; body bytes: 103 */

void __thiscall FUN_0040c590(void *this,undefined4 param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = (undefined4 *)move<>(this);
  uVar2 = FUN_0040b2a0(&param_1);
  FUN_0040bd90();
  if (param_2 == 0) {
    *puVar1 = uVar2;
  }
  else if (param_2 == 3) {
    *puVar1 = uVar2;
  }
  else {
    FUN_0040c550(this);
  }
  return;
}

