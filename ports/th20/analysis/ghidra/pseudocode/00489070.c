/* Entry: 0x00489070; symbol: FUN_00489070; body bytes: 47 */

undefined4 * __thiscall FUN_00489070(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_2);
  *(undefined4 *)this = *puVar1;
  FUN_00414090((undefined4 *)((int)this + 4));
  return (undefined4 *)this;
}

