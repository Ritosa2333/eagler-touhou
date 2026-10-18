/* Entry: 0x0045ecc0; symbol: FUN_0045ecc0; body bytes: 42 */

undefined4 * __thiscall FUN_0045ecc0(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = move<>(param_1);
  uVar1 = move<>(uVar1);
  *(undefined4 *)this = uVar1;
  return (undefined4 *)this;
}

