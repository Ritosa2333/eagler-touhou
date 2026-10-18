/* Entry: 0x004c9350; symbol: FUN_004c9350; body bytes: 39 */

undefined4 __thiscall FUN_004c9350(void *this,undefined4 param_1,int param_2)

{
  if (param_2 == 0) {
    *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) & 0xfffffff9 | 2;
  }
  return 0;
}

