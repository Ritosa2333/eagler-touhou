/* Entry: 0x004d7280; symbol: FUN_004d7280; body bytes: 1203 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004d7280(int *param_1)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  float *pfVar4;
  float10 fVar5;
  undefined1 local_78 [12];
  float local_6c;
  int *local_68;
  int *local_64;
  float local_60;
  int *local_5c;
  int *local_58;
  int *local_54;
  void *local_50;
  void *local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  void *local_30;
  int *local_2c;
  void *local_28;
  int *local_24;
  int *local_20;
  int local_1c;
  int *local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = param_1;
  if ((((uint)param_1[7] >> 5 & 1) != 0) || (param_1[0x1b6] == 0)) goto switchD_004d75b0_default;
  param_1[7] = param_1[7] | 0x20;
  param_1[7] = param_1[7] & 0xffffffbf;
  (**(code **)(*param_1 + 8))();
  local_20 = local_18;
  if (local_18[0x1a6] != 0 || local_18[0x1a7] != 0) {
    if (((uint)local_18[0x1a6] >> 0x1e & 1) != 0) {
      local_24 = local_18 + 0x24;
      local_28 = (void *)FUN_0047c680(local_24,5);
      bVar1 = FUN_00423590(local_28,0);
      if (bVar1) {
        local_18[0x1a6] = local_18[0x1a6] & 0xbfffffff;
      }
      else {
        local_2c = local_18 + 0x24;
        local_30 = (void *)FUN_0047c680(local_2c,5);
        FUN_00429420(local_30);
      }
    }
    if (local_18[0x1b3] != 0) {
      local_18[0x1b3] = local_18[0x1b3] + -1;
    }
  }
  if ((float)local_18[0x1d] <= (float)local_18[0x1c6] &&
      (float)local_18[0x1c6] != (float)local_18[0x1d]) {
    fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
    local_34 = (float)fVar5;
    local_3c = local_34 * (float)local_18[0x1f];
    local_38 = (float)local_18[0x1d];
    local_18[0x1d] = (int)(local_38 + local_3c);
    if ((float)local_18[0x1c6] <= (float)local_18[0x1d] &&
        (float)local_18[0x1d] != (float)local_18[0x1c6]) {
      local_18[0x1d] = local_18[0x1c6];
    }
  }
  fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
  local_40 = (float)fVar5;
  local_44 = local_40 * (float)local_18[0x1c5];
  local_48 = (float)local_18[0x1c];
  fVar5 = (float10)FUN_00438600(local_48,local_44);
  local_18[0x1c] = (int)(float)fVar5;
  local_18[0x16] = local_18[0x1be];
  local_18[0x17] = local_18[0x1bf];
  local_18[0x18] = local_18[0x1c0];
  if ((local_18[0x1d5] & 1U) != 0) {
    local_4c = (void *)FUN_00412730(local_18[0x1bc]);
    iVar2 = FUN_00485660(local_4c,0);
    if (iVar2 != 0) {
      local_50 = (void *)FUN_00412730(local_18[0x1bc]);
      iVar2 = FUN_00485660(local_50,0);
      piVar3 = (int *)FUN_0047a2c0(iVar2);
      local_18[0x16] = *piVar3;
      local_18[0x17] = piVar3[1];
      local_18[0x18] = piVar3[2];
    }
  }
  local_54 = local_18 + 0x1c1;
  fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
  pfVar4 = (float *)FUN_004292f0(local_54,local_78,(float)fVar5);
  FUN_004296e0(local_18 + 0x16,pfVar4);
  if ((float)local_18[0x1d1] != _DAT_0056e0e8) {
    FUN_00422e10(&local_14);
    FUN_00439330(local_18,&local_14,(float)local_18[0x1c],(float)local_18[0x1d1]);
    local_18[0x16] = (int)((float)local_18[0x16] + local_14);
    local_18[0x17] = (int)((float)local_18[0x17] + local_10);
  }
  local_1c = local_18[8] + -2;
  switch(local_1c) {
  case 1:
    local_58 = local_18 + 10;
    bVar1 = FUN_004235c0(local_58,local_18[0x1ca]);
    if (bVar1) {
      FUN_00423520(local_18 + 10,0);
      local_18[8] = 4;
    }
    break;
  case 2:
    local_5c = local_18 + 10;
    bVar1 = FUN_004235c0(local_5c,local_18[0x1cb]);
    if (!bVar1) {
      fVar5 = FUN_00423bd0((int)(local_18 + 10));
      local_60 = (float)fVar5;
      local_18[0x1e] = (int)((local_60 * (float)local_18[0x1c8]) / (float)local_18[0x1cb]);
      break;
    }
    FUN_00423520(local_18 + 10,0);
    local_18[8] = 2;
    local_18[0x1e] = local_18[0x1c8];
  case 0:
    local_64 = local_18 + 10;
    bVar1 = FUN_004235c0(local_64,local_18[0x1cc]);
    if (bVar1) {
      FUN_00423520(local_18 + 10,0);
      local_18[8] = 5;
switchD_004d75b0_caseD_3:
      local_68 = local_18 + 10;
      bVar1 = FUN_004235c0(local_68,local_18[0x1cd]);
      if (!bVar1) {
        fVar5 = FUN_00423bd0((int)(local_18 + 10));
        local_6c = (float)fVar5;
        local_18[0x1e] =
             (int)((float)local_18[0x1c8] -
                  (local_6c * (float)local_18[0x1c8]) / (float)local_18[0x1cd]);
      }
    }
    break;
  case 3:
    goto switchD_004d75b0_caseD_3;
  }
switchD_004d75b0_default:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

