/* Entry: 0x00438b20; symbol: FUN_00438b20; body bytes: 38 */

void __thiscall FUN_00438b20(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0x24) = *param_1;
  *(undefined4 *)((int)this + 0x28) = param_1[1];
  *(undefined4 *)((int)this + 0x2c) = param_1[2];
  return;
}

