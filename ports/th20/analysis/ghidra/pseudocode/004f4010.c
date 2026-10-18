/* Entry: 0x004f4010; symbol: FUN_004f4010; body bytes: 61 */

undefined4 * __thiscall FUN_004f4010(void *this,undefined4 *param_1,undefined1 param_2)

{
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = param_1[1];
  *(undefined4 *)((int)this + 8) = param_1[2];
  *(undefined1 *)((int)this + 0xc) = param_2;
  *(undefined1 *)((int)this + 0xd) = 0;
  *(undefined1 *)((int)this + 0xe) = 0;
  return (undefined4 *)this;
}

