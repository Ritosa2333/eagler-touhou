/* Entry: 0x00506de0; symbol: FUN_00506de0; body bytes: 47 */

void __thiscall FUN_00506de0(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x70) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 4;
  return;
}

