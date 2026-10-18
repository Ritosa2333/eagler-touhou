/* Entry: 0x00467fa0; symbol: FUN_00467fa0; body bytes: 47 */

undefined4 * __thiscall FUN_00467fa0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_2);
  *(undefined4 *)this = *puVar1;
  _String_val<>((undefined4 *)((int)this + 4));
  return (undefined4 *)this;
}

