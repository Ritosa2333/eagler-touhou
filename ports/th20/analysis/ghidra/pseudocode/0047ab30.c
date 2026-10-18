/* Entry: 0x0047ab30; symbol: FUN_0047ab30; body bytes: 43 */

void __thiscall FUN_0047ab30(void *this,undefined4 *param_1)

{
  Decref((int)param_1);
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = param_1[1];
  return;
}

