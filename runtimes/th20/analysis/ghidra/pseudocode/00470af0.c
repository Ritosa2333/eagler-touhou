/* Entry: 0x00470af0; symbol: FUN_00470af0; body bytes: 38 */

void __thiscall FUN_00470af0(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0x2c) = *param_1;
  *(undefined4 *)((int)this + 0x30) = param_1[1];
  *(undefined4 *)((int)this + 0x34) = param_1[2];
  return;
}

