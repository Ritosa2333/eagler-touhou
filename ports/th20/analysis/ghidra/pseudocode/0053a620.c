/* Entry: 0x0053a620; symbol: FUN_0053a620; body bytes: 46 */

undefined4 __thiscall FUN_0053a620(void *this,byte *param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = FUN_0053a350(this,param_1);
  if (puVar1 == (undefined4 *)0x0) {
    uVar2 = 0;
  }
  else {
    uVar2 = puVar1[2];
  }
  return uVar2;
}

