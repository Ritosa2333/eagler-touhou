/* Entry: 0x004570f0; symbol: FUN_004570f0; body bytes: 517 */

void __cdecl
FUN_004570f0(float param_1,float param_2,float param_3,float param_4,undefined4 param_5,
            float param_6,int param_7)

{
  char cVar1;
  float10 fVar2;
  float local_48 [3];
  float local_3c [3];
  int local_30;
  float local_2c [3];
  float local_20;
  float fStack_1c;
  float local_14;
  float fStack_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040bc20(&local_20,0xc,2,FUN_00422e10);
  FUN_00422dd0(local_2c,param_5,0,0);
  for (local_30 = 0; local_30 < param_7; local_30 = local_30 + 1) {
    FUN_00458fa0((void *)0xc,&local_20,local_2c,param_6);
    fVar2 = FUN_00438540((DAT_0056e0f0 * DAT_0056c8d0) / (float)param_7 + param_6);
    param_6 = (float)fVar2;
    FUN_00458fa0(local_2c,&local_14,local_2c,param_6);
    FUN_00422dd0(local_3c,param_3,param_4,0);
    FUN_004296e0(&local_20,local_3c);
    FUN_00422dd0(local_48,param_3,param_4,0);
    FUN_004296e0(&local_14,local_48);
    cVar1 = FUN_00456920(local_20,fStack_1c,local_14,fStack_10,param_1,param_2,param_3,param_4);
    if (cVar1 != '\0') break;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

