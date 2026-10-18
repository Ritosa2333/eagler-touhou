/* Entry: 0x004a1100; symbol: FUN_004a1100; body bytes: 70 */

undefined4 * __thiscall FUN_004a1100(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_1);
  *(undefined4 *)this = *puVar1;
  puVar1 = (undefined4 *)move<>(param_2);
  FUN_0046a830((void *)((int)this + 4),(uint *)*puVar1);
  return (undefined4 *)this;
}

