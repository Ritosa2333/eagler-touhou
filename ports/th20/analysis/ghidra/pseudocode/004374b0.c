/* Entry: 0x004374b0; symbol: FUN_004374b0; body bytes: 65 */

float10 __fastcall FUN_004374b0(int param_1)

{
  float10 fVar1;
  
  if ((*(int *)(param_1 + 0x558) == 0) || ((*(uint *)(param_1 + 0x49c) >> 0xc & 1) != 0)) {
    fVar1 = (float10)*(float *)(param_1 + 0x560);
  }
  else {
    fVar1 = FUN_004374b0(*(int *)(param_1 + 0x558));
  }
  return fVar1;
}

