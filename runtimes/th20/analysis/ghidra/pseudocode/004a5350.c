/* Entry: 0x004a5350; symbol: FUN_004a5350; body bytes: 630 */

void FUN_004a5350(float *param_1)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  int *piVar4;
  int iVar5;
  float10 fVar6;
  uint local_50;
  float local_4c;
  float local_48;
  void *local_44;
  float local_40;
  int *local_3c;
  int *local_38;
  int *local_34;
  int *local_30;
  int *local_2c;
  float local_28;
  int local_24;
  int local_20;
  int local_1c;
  int *local_18;
  float local_14 [2];
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(local_14);
  fVar6 = FUN_004297d0(0x5ba4a8);
  local_28 = (float)fVar6;
  for (local_1c = 0; local_1c < 0xf; local_1c = local_1c + 1) {
    local_2c = local_18 + 3;
    piVar4 = (int *)FUN_00414580(local_2c,local_1c);
    local_24 = *piVar4;
    if (local_18[0x23] != 0) {
      local_30 = local_18 + 0x24;
      bVar3 = FUN_00423560(local_30,local_18[0x23]);
      if (bVar3) {
        local_34 = local_18 + 0x13;
        piVar4 = (int *)FUN_00414580(local_34,local_1c);
        if (0 < *piVar4) {
          local_3c = local_18 + 0x13;
          local_38 = local_18 + 0x13;
          iVar5 = FUN_0040ff90((int)(local_18 + 0x24));
          piVar4 = (int *)FUN_00414580(local_38,local_1c);
          iVar1 = *piVar4;
          iVar2 = local_18[0x23];
          piVar4 = (int *)FUN_00414580(local_3c,local_1c);
          local_24 = (*piVar4 - (iVar5 * iVar1) / iVar2) + local_24;
        }
      }
    }
    for (local_20 = 0; local_20 < local_24; local_20 = local_20 + 1) {
      FUN_00459190(local_18,local_14,local_28,(float)local_18[0x28],(float)local_18[0x29]);
      fVar6 = FUN_00429830(0x5ba4a8);
      local_40 = (float)fVar6;
      FUN_00429690(local_14,local_40 * DAT_0056e0ec + DAT_0056e0ec);
      local_c = 0;
      FUN_004296e0(local_14,param_1);
      local_44 = (void *)FUN_00498fd0(*local_18);
      FUN_004c3c90(local_44,local_1c + 1,local_14,0xffffffff,DAT_0056e0f8 / DAT_0056c8d0,
                   DAT_0056fd34,0,0,0xffffffff);
      local_4c = DAT_0056e0f0 / DAT_0056c8d0 + local_28;
      fVar6 = FUN_004297d0(0x5ba4a8);
      local_48 = (float)fVar6;
      fVar6 = FUN_00438540(local_4c + local_48 / DAT_0056c8e0);
      local_28 = (float)fVar6;
    }
  }
  local_50 = 0;
  FUN_004981f0(local_18 + 3,&local_50);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

