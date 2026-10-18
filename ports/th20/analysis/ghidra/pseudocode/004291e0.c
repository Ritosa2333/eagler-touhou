/* Entry: 0x004291e0; symbol: FUN_004291e0; body bytes: 42 */

undefined4 * __thiscall
FUN_004291e0(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)this = param_3;
  *(undefined4 *)((int)this + 4) = param_2;
  *(undefined4 *)((int)this + 8) = param_1;
  return (undefined4 *)this;
}

