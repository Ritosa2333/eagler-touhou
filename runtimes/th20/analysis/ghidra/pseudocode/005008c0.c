/* Entry: 0x005008c0; symbol: FUN_005008c0; body bytes: 130 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_005008c0(int param_1)

{
  float *pfVar1;
  float10 fVar2;
  undefined1 local_1c [4];
  void *local_18;
  void *local_14;
  float local_10;
  float local_c;
  int local_8;
  
  local_18 = (void *)(param_1 + 0x50);
  local_8 = param_1;
  local_14 = (void *)FUN_00421830(param_1 + 0x50);
  fVar2 = FUN_004298e0(0x5ba4a8);
  local_c = (float)fVar2;
  local_10 = (local_c * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_005732c8;
  pfVar1 = (float *)FUN_00452fc0(local_14,local_1c,local_10);
  fVar2 = FUN_004292e0(pfVar1);
  FUN_0047a560(local_18,(float)fVar2);
  return 0;
}

