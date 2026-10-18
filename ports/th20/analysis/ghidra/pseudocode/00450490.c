/* Entry: 0x00450490; symbol: FUN_00450490; body bytes: 47 */

void __thiscall FUN_00450490(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x38) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 2;
  return;
}

