/* Entry: 0x00422c50; symbol: FUN_00422c50; body bytes: 85 */

undefined4 * __thiscall FUN_00422c50(void *this,undefined4 param_1)

{
  *(undefined4 *)this = 0;
  std::locale::locale((locale *)((int)this + 4));
  *(undefined4 *)((int)this + 8) = 0;
  *(undefined4 *)((int)this + 0xc) = 0xffff00;
  *(undefined4 *)((int)this + 0x10) = 0;
  *(undefined4 *)((int)this + 0x14) = 0;
  *(undefined4 *)((int)this + 0x18) = param_1;
  return (undefined4 *)this;
}

