/* Entry: 0x004f44d0; symbol: FUN_004f44d0; body bytes: 49 */

undefined4 * __thiscall
FUN_004f44d0(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 4) = param_2;
  *(undefined4 *)((int)this + 8) = param_3;
  *(undefined4 *)((int)this + 0xc) = 0;
  return (undefined4 *)this;
}

