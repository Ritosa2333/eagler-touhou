/* Entry: 0x00457380; symbol: FUN_00457380; body bytes: 651 */

void __cdecl
FUN_00457380(float param_1,float param_2,float param_3,float param_4,undefined4 param_5,
            undefined4 param_6,float param_7,int param_8)

{
  char cVar1;
  uint uVar2;
  float10 fVar3;
  float local_64 [3];
  float local_58 [3];
  float local_4c;
  float local_48;
  float *local_44;
  float *local_40;
  uint local_3c;
  float local_38 [3];
  float local_2c [3];
  float local_20;
  float fStack_1c;
  float local_14;
  float fStack_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040bc20(&local_20,0xc,2,FUN_00422e10);
  FUN_00422dd0(local_2c,param_5,0,0);
  FUN_00422dd0(local_38,param_6,0,0);
  for (local_3c = 0; SBORROW4(local_3c,param_8 * 2) != (int)(local_3c + param_8 * -2) < 0;
      local_3c = local_3c + 1) {
    local_48 = param_7;
    uVar2 = local_3c & 0x80000001;
    if ((int)uVar2 < 0) {
      uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
    }
    if (uVar2 == 0) {
      local_40 = local_2c;
    }
    else {
      local_40 = local_38;
    }
    FUN_00458fa0((void *)0x0,&local_20,local_40,param_7);
    fVar3 = FUN_00438540(((DAT_0056e0f0 * DAT_0056c8d0) / (float)param_8) / DAT_0056c8d0 + param_7);
    param_7 = (float)fVar3;
    uVar2 = local_3c & 0x80000001;
    if ((int)uVar2 < 0) {
      uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
    }
    if (uVar2 == 0) {
      local_44 = local_38;
    }
    else {
      local_44 = local_2c;
    }
    local_4c = param_7;
    FUN_00458fa0((void *)0xc,&local_14,local_44,param_7);
    FUN_00422dd0(local_58,param_3,param_4,0);
    FUN_004296e0(&local_20,local_58);
    FUN_00422dd0(local_64,param_3,param_4,0);
    FUN_004296e0(&local_14,local_64);
    cVar1 = FUN_00456920(local_20,fStack_1c,local_14,fStack_10,param_1,param_2,param_3,param_4);
    if (cVar1 != '\0') break;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

