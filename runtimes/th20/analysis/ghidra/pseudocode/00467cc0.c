/* Entry: 0x00467cc0; symbol: FUN_00467cc0; body bytes: 55 */

undefined4 * __thiscall FUN_00467cc0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_1);
  *(undefined4 *)this = *puVar1;
  puVar1 = (undefined4 *)move<>(param_2);
  *(undefined4 *)((int)this + 4) = *puVar1;
  return (undefined4 *)this;
}

