/* Entry: 0x00500950; symbol: FUN_00500950; body bytes: 158 */

undefined4 __fastcall FUN_00500950(int param_1)

{
  float *pfVar1;
  float10 fVar2;
  undefined1 local_28 [4];
  void *local_24;
  void *local_20;
  float local_1c;
  int local_18;
  float local_14;
  void *local_10;
  uint local_c;
  int local_8;
  
  local_8 = param_1;
  local_10 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x120));
  local_c = *(uint *)(local_8 + 0xd8);
  local_18 = FUN_00506480(local_10,local_c);
  local_24 = (void *)(local_8 + 0x50);
  local_20 = (void *)FUN_00421830(local_8 + 0x50);
  fVar2 = FUN_004298e0(0x5ba4a8);
  local_14 = (float)fVar2;
  local_1c = local_14 * *(float *)(local_18 + 0x68);
  pfVar1 = (float *)FUN_00452fc0(local_20,local_28,local_1c);
  fVar2 = FUN_004292e0(pfVar1);
  FUN_0047a560(local_24,(float)fVar2);
  return 0;
}

