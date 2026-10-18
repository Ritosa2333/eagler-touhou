/* Entry: 0x004393f0; symbol: FUN_004393f0; body bytes: 35 */

void __thiscall FUN_004393f0(void *this,undefined4 *param_1)

{
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = param_1[1];
  *(undefined4 *)((int)this + 8) = param_1[2];
  return;
}

