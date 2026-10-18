/* Entry: 0x004cfbb0; symbol: FUN_004cfbb0; body bytes: 1287 */

void __fastcall FUN_004cfbb0(int param_1)

{
  int iVar1;
  float *pfVar2;
  void *this;
  void *this_00;
  float10 fVar3;
  undefined1 local_78 [4];
  undefined1 local_74 [4];
  undefined1 local_70 [4];
  undefined1 local_6c [4];
  void *local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  void *local_4c;
  int local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  void *local_2c;
  int local_28;
  float local_24;
  int local_20;
  float *local_1c;
  int local_18;
  float *local_14;
  undefined4 local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = *(float **)(param_1 + 0x132c);
  local_1c = *(float **)(param_1 + 0x1328);
  local_24 = 0.0;
  local_18 = param_1;
  FUN_00447de0(&local_c);
  FUN_00447de0(&local_10);
  for (local_20 = 0; local_20 < *(int *)(local_18 + 0x718); local_20 = local_20 + 1) {
    local_14[3] = DAT_0056c8cc;
    local_14[4] = -NAN;
    local_14[5] = local_24;
    local_28 = local_18 + 0x754;
    fVar3 = FUN_004d6bb0(local_28,0);
    local_14[6] = (float)fVar3;
    if (local_20 == 0) {
      FUN_00452f20(&local_c,DAT_0056e0f0 / DAT_0056c8d0 + local_1c[6]);
    }
    else {
      FUN_00452f20(&local_c,DAT_0056e0f0 / DAT_0056c8d0 + local_1c[6]);
      FUN_00452f20(&local_10,DAT_0056e0f0 / DAT_0056c8d0 + local_1c[-2]);
      local_2c = FUN_004294e0(&local_10,local_6c,&local_c);
      pfVar2 = (float *)FUN_004c90d0(local_2c,local_70,DAT_0056c8d0);
      FUN_004c9130(&local_c,pfVar2);
    }
    local_30 = *(float *)(local_18 + 0x708) * DAT_0056e0ec;
    fVar3 = FUN_004292e0(&local_c);
    local_34 = (float)fVar3;
    FUN_00439330(this,local_14,local_34,local_30);
    FUN_004296e0(local_14,local_1c);
    iVar1 = FUN_0049df00(&DAT_005b6758,*(int *)(local_18 + 0x6ec));
    local_3c = (float)iVar1;
    local_38 = *local_14;
    *local_14 = local_38 + local_3c;
    iVar1 = FUN_0049df20(&DAT_005b6758,*(int *)(local_18 + 0x6ec));
    local_44 = (float)iVar1;
    local_40 = local_14[1];
    local_14[1] = local_40 + local_44;
    local_14[2] = 0.0;
    local_14[10] = DAT_0056c8cc;
    local_14[0xb] = -NAN;
    local_14[0xc] = local_24;
    local_48 = local_18 + 0x754;
    local_14 = local_14 + 7;
    fVar3 = FUN_004d6bb0(local_48,2);
    local_14[6] = (float)fVar3;
    if (local_20 == 0) {
      FUN_00452f20(&local_c,local_1c[6] - DAT_0056e0f0 / DAT_0056c8d0);
    }
    else {
      FUN_00452f20(&local_c,local_1c[6] - DAT_0056e0f0 / DAT_0056c8d0);
      FUN_00452f20(&local_10,local_1c[-2] - DAT_0056e0f0 / DAT_0056c8d0);
      local_4c = FUN_004294e0(&local_10,local_74,&local_c);
      pfVar2 = (float *)FUN_004c90d0(local_4c,local_78,DAT_0056c8d0);
      FUN_004c9130(&local_c,pfVar2);
    }
    local_50 = *(float *)(local_18 + 0x708) * DAT_0056e0ec;
    fVar3 = FUN_004292e0(&local_c);
    local_54 = (float)fVar3;
    FUN_00439330(this_00,local_14,local_54,local_50);
    FUN_004296e0(local_14,local_1c);
    iVar1 = FUN_0049df00(&DAT_005b6758,*(int *)(local_18 + 0x6ec));
    local_5c = (float)iVar1;
    local_58 = *local_14;
    *local_14 = local_58 + local_5c;
    iVar1 = FUN_0049df20(&DAT_005b6758,*(int *)(local_18 + 0x6ec));
    local_64 = (float)iVar1;
    local_60 = local_14[1];
    local_14[1] = local_60 + local_64;
    local_14[2] = 0.0;
    local_14 = local_14 + 7;
    local_1c = local_1c + 8;
    local_24 = DAT_0056c8cc / (float)(*(int *)(local_18 + 0x718) + -1) + local_24;
  }
  *(uint *)(local_18 + 0xbf0) = *(uint *)(local_18 + 0xbf0) | 0x200000;
  FUN_00445350(DAT_005c0028,local_18 + 0x754,*(float **)(local_18 + 0x132c),
               *(int *)(local_18 + 0x718) << 1);
  iVar1 = FUN_0040ff90(local_18 + 0x48);
  if (iVar1 <= *(int *)(local_18 + 0x718)) {
    local_68 = (void *)(local_18 + 0xd38);
    FUN_00470af0(local_68,(undefined4 *)
                          (*(int *)(local_18 + 0x1328) + (*(int *)(local_18 + 0x718) + -1) * 0x20));
    FUN_0044c570((void *)(local_18 + 0xd38));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

