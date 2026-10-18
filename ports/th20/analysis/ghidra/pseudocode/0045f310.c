/* Entry: 0x0045f310; symbol: FUN_0045f310; body bytes: 44 */

undefined4 * __thiscall FUN_0045f310(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(&param_1);
  *(undefined4 *)this = *puVar1;
  *(undefined4 *)((int)this + 4) = param_2;
  return (undefined4 *)this;
}

