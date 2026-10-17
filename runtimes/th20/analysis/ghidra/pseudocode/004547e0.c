/* Entry: 0x004547e0; symbol: FUN_004547e0; body bytes: 1439 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_004547e0(float *param_1,float *param_2,float *param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8,float param_9)

{
  int iVar1;
  float10 fVar2;
  float10 fVar3;
  int local_64;
  int local_60;
  int local_5c;
  float local_58 [5];
  float fStack_44;
  float fStack_40;
  float fStack_3c;
  float local_38 [4];
  float fStack_28;
  float local_20;
  float fStack_1c;
  float local_18;
  float fStack_14;
  float local_10;
  float fStack_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040bc20(local_58,8,4,FUN_004398a0);
  FUN_0040bc20(&local_20,8,3,FUN_004398a0);
  local_58[0] = (float)((uint)param_7 ^ _DAT_0056d7d0) / DAT_0056c8d0;
  local_58[1] = (float)((uint)param_8 ^ _DAT_0056d7d0) / DAT_0056c8d0;
  local_58[2] = (float)((uint)param_7 ^ _DAT_0056d7d0) / DAT_0056c8d0;
  local_58[3] = param_8 / DAT_0056c8d0;
  local_58[4] = param_7 / DAT_0056c8d0;
  fStack_44 = param_8 / DAT_0056c8d0;
  fStack_40 = param_7 / DAT_0056c8d0;
  fStack_3c = (float)((uint)param_8 ^ _DAT_0056d7d0) / DAT_0056c8d0;
  if (param_4 != _DAT_0056e0e8) {
    FUN_00459030((void *)0x18,local_58,local_58,param_9,4);
  }
  for (local_5c = 0; local_5c < 4; local_5c = local_5c + 1) {
    local_58[local_5c * 2] = local_58[local_5c * 2] + param_5;
    local_58[local_5c * 2 + 1] = local_58[local_5c * 2 + 1] + param_6;
  }
  local_20 = *param_3;
  fStack_1c = param_3[1];
  local_18 = DAT_0056cdb0;
  fStack_14 = 0.0;
  FUN_00458fa0(&local_10,&local_10,&local_18,param_4);
  local_18 = local_20 + local_10;
  fStack_14 = fStack_1c + fStack_c;
  local_20 = local_20 - local_10;
  fStack_1c = fStack_1c - fStack_c;
  FUN_0040bc20(local_38,0xc,2,FUN_00422e10);
  local_64 = 0;
  local_60 = 0;
  while ((local_60 < 4 &&
         ((iVar1 = FUN_004545d0(local_38 + local_64 * 3,local_38 + local_64 * 3 + 1,local_20,
                                fStack_1c,local_18,fStack_14,
                                local_58[*(int *)(&DAT_0056e688 + local_60 * 8) * 2],
                                local_58[*(int *)(&DAT_0056e688 + local_60 * 8) * 2 + 1],
                                local_58[*(int *)(&DAT_0056e68c + local_60 * 8) * 2],
                                local_58[*(int *)(&DAT_0056e68c + local_60 * 8) * 2 + 1]),
          iVar1 == 0 || (local_64 = local_64 + 1, local_64 < 2))))) {
    local_60 = local_60 + 1;
  }
  if (local_64 != 0) {
    if (local_64 == 2) {
      fVar2 = FUN_00456050(local_20 - local_38[0],fStack_1c - local_38[1]);
      fVar3 = FUN_00456050(local_20 - local_38[3],fStack_1c - fStack_28);
      if ((float)fVar3 <= (float)fVar2) {
        *param_1 = local_38[3];
        param_1[1] = fStack_28;
        *param_2 = local_38[0];
        param_2[1] = local_38[1];
      }
      else {
        *param_1 = local_38[0];
        param_1[1] = local_38[1];
        *param_2 = local_38[3];
        param_2[1] = fStack_28;
      }
    }
    else {
      *param_1 = local_38[0];
      param_1[1] = local_38[1];
      *param_2 = local_38[0];
      param_2[1] = local_38[1];
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

