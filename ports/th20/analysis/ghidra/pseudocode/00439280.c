/* Entry: 0x00439280; symbol: FUN_00439280; body bytes: 76 */

void __thiscall FUN_00439280(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)((int)this + 0x44) = param_1;
  *(undefined4 *)((int)this + 0x48) = param_2;
  *(undefined4 *)((int)this + 0x4c) = param_3;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 0x80000;
  return;
}

