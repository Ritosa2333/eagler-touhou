/* Entry: 0x00506e40; symbol: FUN_00506e40; body bytes: 47 */

void __thiscall FUN_00506e40(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x68) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 8;
  return;
}

