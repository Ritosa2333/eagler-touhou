/* Entry: 0x00438a90; symbol: FUN_00438a90; body bytes: 38 */

void __thiscall FUN_00438a90(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0xc) = *param_1;
  *(undefined4 *)((int)this + 0x10) = param_1[1];
  *(undefined4 *)((int)this + 0x14) = param_1[2];
  return;
}

