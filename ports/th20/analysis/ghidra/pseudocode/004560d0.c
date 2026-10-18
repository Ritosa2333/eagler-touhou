/* Entry: 0x004560d0; symbol: FUN_004560d0; body bytes: 88 */

float10 __cdecl FUN_004560d0(float *param_1,float *param_2)

{
  return (float10)((*param_1 - *param_2) * (*param_1 - *param_2) +
                  (param_1[1] - param_2[1]) * (param_1[1] - param_2[1]));
}

