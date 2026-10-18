/* Entry: 0x004543d0; symbol: FUN_004543d0; body bytes: 119 */

void __fastcall FUN_004543d0(float *param_1)

{
  float10 fVar1;
  
  fVar1 = FUN_004592b0(*param_1 * DAT_0056e04c);
  *param_1 = (float)fVar1 / DAT_0056e04c;
  fVar1 = FUN_004592b0(param_1[1] * DAT_0056e04c);
  param_1[1] = (float)fVar1 / DAT_0056e04c;
  return;
}

