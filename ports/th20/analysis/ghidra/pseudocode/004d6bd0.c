/* Entry: 0x004d6bd0; symbol: FUN_004d6bd0; body bytes: 110 */

void __thiscall FUN_004d6bd0(void *this,float *param_1)

{
  FUN_00439330(this,param_1,*(float *)((int)this + 0x70),*(float *)((int)this + 0x74));
  param_1[2] = 0.0;
  *param_1 = *param_1 + *(float *)((int)this + 0x58);
  param_1[1] = param_1[1] + *(float *)((int)this + 0x5c);
  return;
}

