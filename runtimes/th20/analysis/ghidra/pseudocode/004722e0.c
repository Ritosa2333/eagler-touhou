/* Entry: 0x004722e0; symbol: FUN_004722e0; body bytes: 1375 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004722e0(int param_1)

{
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *extraout_ECX_01;
  void *extraout_ECX_02;
  float10 fVar1;
  float fVar2;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  void *local_28;
  int local_20;
  float local_1c;
  float local_18;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  for (local_20 = 0; local_20 < 2; local_20 = local_20 + 1) {
    if (*(int *)(param_1 + 0x32b8 + local_20 * 4) != 0) {
      local_34 = *(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 0x18);
      FUN_00422e10(&local_1c);
      local_c = *(float *)(param_1 + 0x32f8 + local_20 * 4);
      local_10 = *(float *)(param_1 + 0x3300 + local_20 * 4);
      if (*(int *)(param_1 + 0x3308) == 1) {
        FUN_0049d850(*(void **)(param_1 + 0x32b8 + local_20 * 4),DAT_0056fa6c,0.0,DAT_0056cda4,
                     DAT_0056cd98);
        local_28 = *(void **)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 0x14);
        for (local_38 = 0; local_38 < **(int **)(param_1 + 0x32b8 + local_20 * 4);
            local_38 = local_38 + 1) {
          for (local_2c = 0; local_2c < *(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 4);
              local_2c = local_2c + 1) {
            *(undefined1 *)((int)local_28 + 0x13) = 0xc0;
            fVar2 = DAT_0056f7b4 -
                    ((float)local_2c * DAT_0056f7b4) /
                    (float)(*(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 4) + -1);
            fVar1 = FUN_004292e0(&local_c);
            fVar1 = FUN_00439820(extraout_ECX,(float)fVar1);
            local_1c = (float)fVar1 * fVar2;
            fVar1 = FUN_004292e0(&local_10);
            fVar1 = FUN_00439820(extraout_ECX_00,(float)fVar1);
            local_18 = (float)fVar1 * fVar2;
            if (((local_38 != 0) && (local_2c != 0)) &&
               ((local_38 != **(int **)(param_1 + 0x32b8 + local_20 * 4) + -1 &&
                (local_2c != *(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 4) + -1)))) {
              FUN_004296e0(local_28,&local_1c);
              *(undefined4 *)((int)local_28 + 8) = 0;
              *(undefined4 *)(local_34 + 8) = 0;
            }
            FUN_004530c0(&local_c,DAT_0056e0f0 / _DAT_0056fa24);
            local_34 = local_34 + 0xc;
            local_28 = (void *)((int)local_28 + 0x1c);
          }
          FUN_00472060(&local_10,DAT_0056e0f0 / _DAT_0056fa20);
        }
        FUN_004530c0((void *)(param_1 + 0x32f8 + local_20 * 4),DAT_0056e0f0 / DAT_0056fa28);
        FUN_004530c0((void *)(param_1 + 0x3300 + local_20 * 4),DAT_0056e0f0 / DAT_0056fa30);
      }
      else if (*(int *)(param_1 + 0x3308) == 2) {
        FUN_0049d850(*(void **)(param_1 + 0x32b8 + local_20 * 4),DAT_0056fa6c,0.0,DAT_0056cda4,
                     DAT_0056d7c0);
        local_28 = *(void **)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 0x14);
        for (local_3c = 0; local_3c < **(int **)(param_1 + 0x32b8 + local_20 * 4);
            local_3c = local_3c + 1) {
          for (local_30 = 0; local_30 < *(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 4);
              local_30 = local_30 + 1) {
            *(undefined1 *)((int)local_28 + 0x13) = 0xc0;
            fVar2 = DAT_0056f7b4 -
                    ((float)local_30 * DAT_0056f7b4) /
                    (float)(*(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 4) + -1);
            fVar1 = FUN_004292e0(&local_c);
            fVar1 = FUN_00439820(extraout_ECX_01,(float)fVar1);
            local_1c = (float)fVar1 * fVar2;
            fVar1 = FUN_004292e0(&local_10);
            fVar1 = FUN_00439820(extraout_ECX_02,(float)fVar1);
            local_18 = (float)fVar1 * fVar2;
            if ((((local_3c != 0) && (local_30 != 0)) &&
                (local_3c != **(int **)(param_1 + 0x32b8 + local_20 * 4) + -1)) &&
               (local_30 != *(int *)(*(int *)(param_1 + 0x32b8 + local_20 * 4) + 4) + -1)) {
              FUN_004296e0(local_28,&local_1c);
              *(undefined4 *)((int)local_28 + 8) = 0;
              *(undefined4 *)(local_34 + 8) = 0;
            }
            FUN_004530c0(&local_c,DAT_0056e0f0 / _DAT_0056fa24);
            local_34 = local_34 + 0xc;
            local_28 = (void *)((int)local_28 + 0x1c);
          }
          FUN_00472060(&local_10,DAT_0056e0f0 / _DAT_0056fa20);
        }
        FUN_004530c0((void *)(param_1 + 0x32f8 + local_20 * 4),DAT_0056e0f0 / DAT_0056fa28);
        FUN_004530c0((void *)(param_1 + 0x3300 + local_20 * 4),DAT_0056e0f0 / DAT_0056fa30);
        FUN_00423540((undefined4 *)(param_1 + 0x32d8 + local_20 * 0x10));
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

