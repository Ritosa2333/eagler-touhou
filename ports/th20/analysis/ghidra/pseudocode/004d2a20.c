/* Entry: 0x004d2a20; symbol: FUN_004d2a20; body bytes: 930 */

void __thiscall FUN_004d2a20(void *this,int param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  float *pfVar4;
  undefined1 local_ac [12];
  undefined1 local_a0 [12];
  int *local_94;
  int *local_90;
  int *local_8c;
  void *local_88;
  int local_84;
  void *local_80;
  int *local_7c;
  code *local_78;
  int local_74;
  float local_70;
  void *local_6c;
  float local_68;
  float local_64;
  float local_60;
  code *local_5c;
  void *local_58;
  int local_54;
  int local_50;
  float *local_4c;
  int *local_48;
  undefined1 local_44 [12];
  undefined4 local_38 [3];
  float local_2c;
  float local_28;
  float local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_4c = *(float **)((int)this + 0x6d8);
  local_58 = (void *)((int)this + 0x770);
  local_48 = (int *)this;
  FUN_004863c0(local_58,0);
  if (((uint)local_48[7] >> 6 & 1) == 0) {
    local_5c = *(code **)(*local_48 + 0x10);
    uVar1 = (*local_5c)(0);
  }
  else {
    uVar1 = (uint)local_4c[0xe] >> 3 & 1;
  }
  if (uVar1 != 0) {
    FUN_00422e10(&local_14);
    FUN_00422e10(&local_20);
    local_70 = 0.0;
    local_14 = *local_4c;
    local_10 = local_4c[1];
    local_c = local_4c[2];
    local_6c = (void *)FUN_0040ff90(local_48[0x1bc]);
    local_60 = local_4c[6];
    local_64 = local_4c[7];
    local_68 = local_4c[9];
    local_54 = FUN_004f91d0(local_6c,&local_14,local_68,local_64,local_60,param_1);
    if (local_54 == 2) {
      iVar2 = FUN_0040ff90(local_48[0x1bc]);
      puVar3 = (undefined4 *)FUN_00460850(iVar2);
      local_20 = *puVar3;
      local_1c = puVar3[1];
      local_18 = puVar3[2];
      local_70 = local_4c[7] + DAT_0056edd4 + DAT_0056f280;
    }
    if (local_54 == 1) {
      FUN_00422dd0(local_44,DAT_0056cd94,DAT_0056cd94,0);
      local_78 = *(code **)(*local_48 + 0x2c);
      iVar2 = FUN_0040ff90(local_48[0x1bc]);
      local_74 = FUN_00460850(iVar2);
      (*local_78)(local_74,local_44,0,0,1);
    }
    else if (local_54 == 2) {
      local_48[0x1b5] = local_48[0x1b5] + 1;
      local_7c = local_48 + 0xe;
      iVar2 = FUN_00472040(local_7c,8);
      if (iVar2 == 0) {
        FUN_00422e10(local_38);
        FUN_00422e10(&local_2c);
        FUN_00454d80(&local_14,local_4c[6],local_4c[9],&local_20,local_38);
        iVar2 = FUN_0040ff90(local_48[0x1bc]);
        pfVar4 = (float *)FUN_00460850(iVar2);
        local_80 = FUN_00429570(local_38,local_a0,pfVar4);
        pfVar4 = (float *)FUN_00452ff0(local_80,local_ac,DAT_0056c8d0);
        local_2c = *pfVar4;
        local_28 = pfVar4[1];
        local_24 = pfVar4[2];
        local_88 = (void *)FUN_0040ff90(local_48[0x1bc]);
        local_84 = local_48[0x1b4];
        FUN_004f8b90(local_88,&local_2c,local_84);
      }
      if (((uint)local_48[0x1a7] >> 1 & 1) == 0) {
        local_50 = local_48[0x1b5] * -2 + 0xd0;
        if (0xf0 < local_50) {
          local_50 = 0xf0;
        }
        if (local_50 < 0x60) {
          local_50 = 0x60;
        }
        local_8c = local_48 + 0x1dc;
        FUN_004863c0(local_8c,1);
        local_90 = local_48 + 0x1dc;
        FUN_004864b0(local_90,0xff,(char)local_50,0x80,0xff);
      }
      local_94 = local_48 + 0xe;
      FUN_00423540(local_94);
    }
    else {
      local_48[0x1b5] = 0;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

