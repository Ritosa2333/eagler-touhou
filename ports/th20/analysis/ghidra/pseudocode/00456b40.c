/* Entry: 0x00456b40; symbol: FUN_00456b40; body bytes: 517 */

void __cdecl
FUN_00456b40(float param_1,float param_2,float param_3,float param_4,undefined4 param_5,
            undefined4 param_6,undefined4 param_7,float param_8,int param_9)

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
  FUN_00422dd0(local_2c,param_7,0,0);
  for (local_30 = 0; local_30 < param_9; local_30 = local_30 + 1) {
    FUN_00458fa0((void *)0xc,&local_20,local_2c,param_8);
    fVar2 = FUN_00438540((DAT_0056e0f0 * DAT_0056c8d0) / (float)param_9 + param_8);
    param_8 = (float)fVar2;
    FUN_00458fa0(local_2c,&local_14,local_2c,param_8);
    FUN_00422dd0(local_3c,param_5,param_6,0);
    FUN_004296e0(&local_20,local_3c);
    FUN_00422dd0(local_48,param_5,param_6,0);
    FUN_004296e0(&local_14,local_48);
    cVar1 = FUN_00456920(local_20,fStack_1c,local_14,fStack_10,param_1,param_2,param_3,param_4);
    if (cVar1 != '\0') break;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

