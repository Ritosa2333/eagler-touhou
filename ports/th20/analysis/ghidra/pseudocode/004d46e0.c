/* Entry: 0x004d46e0; symbol: FUN_004d46e0; body bytes: 272 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004d46e0(void *param_1,float param_2)

{
  float *pfVar1;
  undefined1 local_30 [12];
  int local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_20);
  FUN_00422e10(&local_14);
  pfVar1 = (float *)FUN_00429440(param_1,local_30,(float *)(local_24 + 0x58));
  local_20 = *pfVar1;
  local_1c = pfVar1[1];
  local_18 = pfVar1[2];
  FUN_00458fa0(&local_20,&local_14,&local_20,(float)(*(uint *)(local_24 + 0x70) ^ _DAT_0056d7d0));
  local_20 = local_14 - param_2;
  local_1c = local_10 - param_2;
  local_14 = local_14 + param_2;
  local_10 = local_10 + param_2;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

