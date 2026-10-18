/* Entry: 0x004580c0; symbol: FUN_004580c0; body bytes: 1452 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_004580c0(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8,float param_9,float param_10)

{
  char cVar1;
  float10 fVar2;
  float10 fVar3;
  float10 fVar4;
  int local_54;
  int local_50;
  int local_4c;
  float local_48 [5];
  float fStack_34;
  float fStack_30;
  float fStack_2c;
  float local_28 [5];
  float fStack_14;
  float fStack_10;
  float fStack_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040bc20(local_48,8,4,FUN_004398a0);
  FUN_0040bc20(local_28,8,4,FUN_004398a0);
  fVar2 = (float10)FUN_00455fe0(param_1,param_6,param_2,param_7);
  fVar3 = (float10)FUN_00455fb0(param_3 / DAT_0056c8d0,param_4 / DAT_0056c8d0);
  fVar4 = (float10)FUN_00455fb0(param_8 / DAT_0056c8d0,param_9 / DAT_0056c8d0);
  if ((float)fVar2 < (float)fVar3 + (float)fVar4) {
    local_48[0] = (float)((uint)param_3 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_48[1] = (float)((uint)param_4 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_48[2] = (float)((uint)param_3 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_48[3] = param_4 / DAT_0056c8d0;
    local_48[4] = param_3 / DAT_0056c8d0;
    fStack_34 = param_4 / DAT_0056c8d0;
    fStack_30 = param_3 / DAT_0056c8d0;
    fStack_2c = (float)((uint)param_4 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    if (param_5 != _DAT_0056e0e8) {
      FUN_00459030((void *)0x18,local_48,local_48,param_5,4);
    }
    local_28[0] = (float)((uint)param_8 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_28[1] = (float)((uint)param_9 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_28[2] = (float)((uint)param_8 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_28[3] = param_9 / DAT_0056c8d0;
    local_28[4] = param_8 / DAT_0056c8d0;
    fStack_14 = param_9 / DAT_0056c8d0;
    fStack_10 = param_8 / DAT_0056c8d0;
    fStack_c = (float)((uint)param_9 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    if (param_10 != _DAT_0056e0e8) {
      FUN_00459030(local_28,local_28,local_28,param_10,4);
    }
    for (local_4c = 0; local_4c < 4; local_4c = local_4c + 1) {
      local_48[local_4c * 2] = local_48[local_4c * 2] + param_1;
      local_48[local_4c * 2 + 1] = local_48[local_4c * 2 + 1] + param_2;
      local_28[local_4c * 2] = local_28[local_4c * 2] + param_6;
      local_28[local_4c * 2 + 1] = local_28[local_4c * 2 + 1] + param_7;
    }
    cVar1 = FUN_00458b60(param_1,param_2,param_3,param_4,param_5,local_28);
    if ((cVar1 == '\0') &&
       (cVar1 = FUN_00458b60(param_6,param_7,param_8,param_9,param_10,local_48), cVar1 == '\0')) {
      for (local_54 = 0; local_54 < 4; local_54 = local_54 + 1) {
        for (local_50 = 0; local_50 < 4; local_50 = local_50 + 1) {
          cVar1 = FUN_00456920(local_48[*(int *)(&DAT_0056e688 + local_54 * 8) * 2],
                               local_48[*(int *)(&DAT_0056e688 + local_54 * 8) * 2 + 1],
                               local_48[*(int *)(&DAT_0056e68c + local_54 * 8) * 2],
                               local_48[*(int *)(&DAT_0056e68c + local_54 * 8) * 2 + 1],
                               local_28[*(int *)(&DAT_0056e688 + local_50 * 8) * 2],
                               local_28[*(int *)(&DAT_0056e688 + local_50 * 8) * 2 + 1],
                               local_28[*(int *)(&DAT_0056e68c + local_50 * 8) * 2],
                               local_28[*(int *)(&DAT_0056e68c + local_50 * 8) * 2 + 1]);
          if (cVar1 != '\0') goto LAB_0045865e;
        }
      }
    }
  }
LAB_0045865e:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

