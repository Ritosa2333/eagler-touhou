/* Entry: 0x0040de00; symbol: FUN_0040de00; body bytes: 33 */

undefined4 * __thiscall FUN_0040de00(void *this,undefined4 param_1,undefined4 param_2)

{
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 4) = param_2;
  return (undefined4 *)this;
}

