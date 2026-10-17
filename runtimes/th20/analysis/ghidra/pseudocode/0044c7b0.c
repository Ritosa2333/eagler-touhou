/* Entry: 0x0044c7b0; symbol: FUN_0044c7b0; body bytes: 127 */

void FUN_0044c7b0(undefined4 *param_1,int param_2,int param_3)

{
  undefined4 local_24;
  undefined4 *local_20;
  undefined4 local_1c;
  undefined4 *local_18;
  undefined4 *local_14;
  void *local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00425cc0(&local_c);
  local_10 = (void *)FUN_0044ced0(local_18);
  FUN_004117a0(&local_c,0);
  if (local_10 == (void *)0x0) {
    local_1c = local_c;
    local_14 = &local_1c;
  }
  else {
    local_14 = (undefined4 *)FUN_0044c830(local_10,&local_24,param_2,param_3);
  }
  local_20 = local_14;
  *param_1 = *local_14;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

