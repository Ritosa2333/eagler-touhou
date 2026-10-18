/* Entry: 0x004d9680; symbol: FUN_004d9680; body bytes: 132 */

void __cdecl FUN_004d9680(float *param_1,float *param_2,float *param_3)

{
  *param_1 = param_2[1] * param_3[2] - param_2[2] * param_3[1];
  param_1[1] = param_2[2] * *param_3 - *param_2 * param_3[2];
  param_1[2] = *param_2 * param_3[1] - param_2[1] * *param_3;
  return;
}

