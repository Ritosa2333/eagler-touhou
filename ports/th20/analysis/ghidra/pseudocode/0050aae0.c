/* Entry: 0x0050aae0; symbol: FUN_0050aae0; body bytes: 47 */

undefined4 * __thiscall FUN_0050aae0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_1);
  *(undefined4 *)this = *puVar1;
  *(undefined4 *)((int)this + 4) = puVar1[1];
  *(undefined4 *)((int)this + 8) = puVar1[2];
  return (undefined4 *)this;
}

