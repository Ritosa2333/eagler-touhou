/* Entry: 0x00454d80; symbol: FUN_00454d80; body bytes: 343 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00454d80(float *param_1,float param_2,float param_3,void *param_4,undefined4 *param_5)

{
  float *pfVar1;
  undefined4 *puVar2;
  float10 fVar3;
  undefined1 local_3c [12];
  undefined1 local_30 [12];
  float local_24;
  float local_20 [4];
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(local_20 + 3);
  FUN_00422e10(local_20);
  pfVar1 = (float *)FUN_00429440(param_4,local_30,param_1);
  local_20[3] = *pfVar1;
  local_10 = pfVar1[1];
  local_c = pfVar1[2];
  FUN_00458fa0(local_20 + 3,local_20 + 3,local_20 + 3,(float)((uint)param_3 ^ _DAT_0056d7d0));
  if ((local_20[3] <= (float)((uint)param_2 ^ _DAT_0056d7d0) / DAT_0056c8d0) ||
     (param_2 / DAT_0056c8d0 <= local_20[3])) {
    if (local_20[3] <= _DAT_0056e0e8) {
      if (local_20[3] < 0.0) {
        local_20[0] = (float)((uint)param_2 ^ _DAT_0056d7d0) / DAT_0056c8d0;
      }
    }
    else {
      local_20[0] = param_2 / DAT_0056c8d0;
    }
  }
  else {
    local_20[0] = local_20[3];
  }
  local_20[1] = 0.0;
  local_20[2] = 0.0;
  fVar3 = (float10)FUN_00456030(local_20,local_20 + 3);
  local_24 = (float)fVar3;
  FUN_00458fa0(local_20,local_20,local_20,param_3);
  puVar2 = (undefined4 *)FUN_00429570(param_1,local_3c,local_20);
  *param_5 = *puVar2;
  param_5[1] = puVar2[1];
  param_5[2] = puVar2[2];
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

