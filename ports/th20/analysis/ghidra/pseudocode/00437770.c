/* Entry: 0x00437770; symbol: FUN_00437770; body bytes: 196 */

int __fastcall FUN_00437770(int param_1)

{
  float *pfVar1;
  float10 fVar2;
  
  *(undefined4 *)(param_1 + 0x5d0) = *(undefined4 *)(param_1 + 0x38);
  *(undefined4 *)(param_1 + 0x5d4) = *(undefined4 *)(param_1 + 0x3c);
  *(undefined4 *)(param_1 + 0x5d8) = *(undefined4 *)(param_1 + 0x40);
  if ((*(int *)(param_1 + 0x558) != 0) && ((*(uint *)(param_1 + 0x49c) >> 0xc & 1) == 0)) {
    pfVar1 = (float *)FUN_00437770(*(int *)(param_1 + 0x558));
    FUN_004296e0((void *)(param_1 + 0x5d0),pfVar1);
    fVar2 = FUN_00438540(*(float *)(param_1 + 0x38));
    *(float *)(param_1 + 0x38) = (float)fVar2;
    fVar2 = FUN_00438540(*(float *)(param_1 + 0x3c));
    *(float *)(param_1 + 0x3c) = (float)fVar2;
    fVar2 = FUN_00438540(*(float *)(param_1 + 0x40));
    *(float *)(param_1 + 0x40) = (float)fVar2;
  }
  return param_1 + 0x5d0;
}

