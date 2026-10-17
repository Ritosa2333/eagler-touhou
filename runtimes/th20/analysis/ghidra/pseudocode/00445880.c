/* Entry: 0x00445880; symbol: FUN_00445880; body bytes: 93 */

float10 __fastcall FUN_00445880(int param_1)

{
  float10 fVar1;
  undefined4 local_c;
  
  local_c = DAT_0056c8cc;
  if ((*(int *)(param_1 + 0x558) != 0) && ((*(uint *)(param_1 + 0x49c) >> 0xc & 1) == 0)) {
    fVar1 = FUN_00445880(*(int *)(param_1 + 0x558));
    local_c = (float)fVar1;
  }
  return (float10)(*(float *)(param_1 + 0x58) * local_c);
}

