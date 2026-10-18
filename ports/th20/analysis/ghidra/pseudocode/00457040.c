/* Entry: 0x00457040; symbol: FUN_00457040; body bytes: 173 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00457040(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7)

{
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  local_14 = param_1 - param_3;
  local_10 = param_2 - param_4;
  FUN_00458fa0(&local_14,&local_14,&local_14,(float)((uint)param_7 ^ _DAT_0056d7d0));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

