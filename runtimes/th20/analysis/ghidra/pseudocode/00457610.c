/* Entry: 0x00457610; symbol: FUN_00457610; body bytes: 669 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00457610(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8)

{
  float10 fVar1;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  local_14 = param_6 - param_1;
  local_10 = param_7 - param_2;
  FUN_00458fa0(&local_14,&local_14,&local_14,(float)((uint)param_5 ^ _DAT_0056d7d0));
  fVar1 = FUN_00445680(local_14);
  if (((param_3 / DAT_0056c8d0 + param_8 < (float)fVar1) ||
      (fVar1 = FUN_00445680(local_10), param_4 / DAT_0056c8d0 < (float)fVar1)) &&
     (fVar1 = FUN_00445680(local_14), (float)fVar1 <= param_3 / DAT_0056c8d0)) {
    FUN_00445680(local_10);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

