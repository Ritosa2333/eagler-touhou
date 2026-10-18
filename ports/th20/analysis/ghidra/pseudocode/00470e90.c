/* Entry: 0x00470e90; symbol: FUN_00470e90; body bytes: 60 */

void __thiscall FUN_00470e90(void *this,undefined4 param_1,undefined4 param_2)

{
  *(undefined4 *)((int)this + 0x70) = param_1;
  *(undefined4 *)((int)this + 0x74) = param_2;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 4;
  return;
}

