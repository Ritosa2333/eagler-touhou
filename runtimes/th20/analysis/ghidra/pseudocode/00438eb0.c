/* Entry: 0x00438eb0; symbol: FUN_00438eb0; body bytes: 95 */

void __cdecl FUN_00438eb0(float *param_1,float *param_2)

{
  *param_1 = *param_2 / DAT_0056cdac;
  param_1[1] = param_2[1] / DAT_0056cda8;
  if (*param_1 <= 0.0 && *param_1 != 0.0) {
    *param_1 = 0.0;
  }
  if (param_1[1] <= 0.0 && param_1[1] != 0.0) {
    param_1[1] = 0.0;
  }
  return;
}

