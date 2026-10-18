/* Entry: 0x004396c0; symbol: FUN_004396c0; body bytes: 150 */

float10 __cdecl FUN_004396c0(float param_1,float param_2)

{
  float10 fVar1;
  
  if (param_1 - param_2 <= DAT_0056e0f0) {
    if (param_2 - param_1 <= DAT_0056e0f0) {
      fVar1 = (float10)(param_1 - param_2);
    }
    else {
      fVar1 = (float10)(param_1 - (param_2 - DAT_0056e0f0 * DAT_0056c8d0));
    }
  }
  else {
    fVar1 = (float10)(param_1 - (DAT_0056e0f0 * DAT_0056c8d0 + param_2));
  }
  return fVar1;
}

