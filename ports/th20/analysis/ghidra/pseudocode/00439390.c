/* Entry: 0x00439390; symbol: FUN_00439390; body bytes: 62 */

void __thiscall FUN_00439390(void *this,undefined4 param_1,undefined4 param_2)

{
  *(undefined4 *)((int)this + 0x60) = param_1;
  *(undefined4 *)((int)this + 100) = param_2;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 0x80000;
  return;
}

