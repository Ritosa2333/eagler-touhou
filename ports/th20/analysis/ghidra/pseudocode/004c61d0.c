/* Entry: 0x004c61d0; symbol: FUN_004c61d0; body bytes: 55 */

void __thiscall FUN_004c61d0(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0xfc) = param_1;
  *(undefined4 *)((int)this + 0x100) = 0;
  FUN_00423520((void *)((int)this + 0xc0),0);
  return;
}

