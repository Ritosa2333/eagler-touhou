/* Entry: 0x00456690; symbol: FUN_00456690; body bytes: 313 */

void __cdecl
FUN_00456690(float param_1,float param_2,float param_3,float param_4,float param_5,
            undefined4 param_6,float param_7,int param_8)

{
  bool bVar1;
  float local_20 [3];
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00456fe0(param_4,param_5,param_1,param_2,param_3);
  if (!bVar1) {
    FUN_00422dd0(&local_14,param_4 - param_1,param_5 - param_2,0);
    FUN_004532c0(&local_14,&local_14,param_3);
    FUN_00422dd0(local_20,param_1,param_2,0);
    FUN_004296e0(&local_14,local_20);
    FUN_004570f0(local_14,local_10,param_4,param_5,param_6,param_7,param_8);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

