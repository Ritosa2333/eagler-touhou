/* Entry: 0x0054c3d7; symbol: FUN_0054c3d7; body bytes: 49 */

undefined4 * __thiscall
FUN_0054c3d7(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined1 *param_4
            )

{
  *(undefined4 *)((int)this + 0x10) = 0;
  *(undefined4 *)((int)this + 0x14) = 0;
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 8) = param_2;
  *(undefined4 *)((int)this + 0xc) = param_3;
  *(undefined1 **)((int)this + 0x18) = param_4;
  if (param_4 != (undefined1 *)0x0) {
    *param_4 = 1;
  }
  return (undefined4 *)this;
}

