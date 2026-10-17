/* Entry: 0x00422dd0; symbol: FUN_00422dd0; body bytes: 54 */

undefined4 * __thiscall
FUN_00422dd0(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 4) = param_2;
  *(undefined4 *)((int)this + 8) = param_3;
  return (undefined4 *)this;
}

