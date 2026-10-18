/* Entry: 0x00439480; symbol: FUN_00439480; body bytes: 38 */

void __thiscall FUN_00439480(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0x18) = *param_1;
  *(undefined4 *)((int)this + 0x1c) = param_1[1];
  *(undefined4 *)((int)this + 0x20) = param_1[2];
  return;
}

