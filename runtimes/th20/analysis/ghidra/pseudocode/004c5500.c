/* Entry: 0x004c5500; symbol: FUN_004c5500; body bytes: 33 */

undefined4 * __thiscall FUN_004c5500(void *this,undefined4 param_1)

{
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 4) = param_1;
  return (undefined4 *)this;
}

