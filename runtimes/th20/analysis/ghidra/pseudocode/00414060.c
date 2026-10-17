/* Entry: 0x00414060; symbol: FUN_00414060; body bytes: 42 */

undefined4 * __thiscall FUN_00414060(void *this,undefined4 param_1,undefined4 param_2)

{
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 4) = param_1;
  *(undefined4 *)((int)this + 8) = param_2;
  return (undefined4 *)this;
}

