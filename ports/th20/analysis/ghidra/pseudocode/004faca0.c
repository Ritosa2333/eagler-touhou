/* Entry: 0x004faca0; symbol: FUN_004faca0; body bytes: 1518 */

void __thiscall FUN_004faca0(void *this,int param_1)

{
  undefined4 uVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  float *pfVar6;
  undefined1 local_a8 [8];
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  void *local_8c;
  void *local_88;
  void *local_84;
  undefined4 local_80;
  int local_7c;
  int local_78;
  int local_74;
  void *local_70;
  void *local_6c;
  int *local_68;
  int *local_64;
  void *local_60;
  void *local_5c;
  undefined4 *local_58;
  void *local_54;
  undefined4 *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  undefined4 local_40;
  int local_3c;
  int local_38;
  int local_34;
  void *local_30;
  void *local_2c;
  undefined4 *local_28;
  int *local_24;
  int local_20;
  uint local_1c;
  void *local_18;
  undefined4 *local_14;
  
  local_18 = this;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)((int)this + 0x14854));
  local_2c = (void *)((int)local_18 + 0x22b4);
  FUN_004ffd50(local_2c,1);
  if (param_1 < 0) {
    iVar3 = FUN_0041caf0(*(int *)((int)local_18 + 0x14858));
    FUN_004b81a0(iVar3);
  }
  iVar3 = FUN_0041caf0(*(int *)((int)local_18 + 0x14858));
  local_20 = FUN_004b81a0(iVar3);
  iVar3 = FUN_0041caf0(*(int *)((int)local_18 + 0x14858));
  iVar3 = FUN_004993b0(iVar3);
  iVar4 = FUN_0041caf0(*(int *)((int)local_18 + 0x14858));
  iVar4 = FUN_004b8210(iVar4);
  if (iVar3 < iVar4) {
    local_44 = (void *)((int)local_18 + 0x684);
    local_14 = (undefined4 *)FUN_004ba3e0(local_44,0);
    for (local_1c = 0; local_1c < 10; local_1c = local_1c + 1) {
      FUN_00479040(local_14 + 0x38);
      FUN_0049da50(local_14 + 0x38);
      local_14 = local_14 + 0x4b;
    }
  }
  else {
    local_30 = (void *)((int)local_18 + 0x684);
    local_14 = (undefined4 *)FUN_004ba3e0(local_30,0);
    for (local_1c = 0; (int)local_1c < local_20; local_1c = local_1c + 1) {
      FUN_0044fcd0(local_14 + 0x38);
      iVar3 = FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
      iVar3 = FUN_00421810(iVar3);
      if (iVar3 != 0) {
        iVar3 = FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
        local_40 = FUN_00421810(iVar3);
        local_34 = *(int *)((int)local_18 + 0x2208) + 0xa8;
        iVar3 = FUN_00464230(0);
        local_38 = FUN_00534130(iVar3);
        local_38 = local_38 * 4;
        local_3c = *(int *)(local_34 + local_38);
        puVar5 = FUN_0049cbf0(&local_a0,(uint *)0x0,local_3c,-1,(undefined4 *)0x0);
        local_14[0x38] = *puVar5;
      }
      iVar3 = FUN_0044ced0(local_14 + 0x38);
      iVar3 = FUN_0044cb20(iVar3);
      *(undefined4 *)(iVar3 + 4) = DAT_0056fd7c;
      local_14 = local_14 + 0x4b;
    }
  }
  iVar3 = FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
  iVar3 = FUN_004ff670(iVar3);
  if (iVar3 != local_20) {
    local_48 = (void *)((int)local_18 + 0x684);
    local_14 = (undefined4 *)FUN_004ba3e0(local_48,0);
    for (local_1c = 0; (int)local_1c < local_20; local_1c = local_1c + 1) {
      iVar3 = FUN_00460830(0);
      bVar2 = FUN_004ff5e0(iVar3);
      *(bool *)(local_14 + 0x41) = bVar2;
      FUN_0044fcd0(local_14 + 0x37);
      local_14[0x3e] = local_1c;
      local_50 = local_14 + 0x20;
      local_4c = (void *)FUN_00464250(*(int *)((int)local_18 + 0x14858));
      pfVar6 = (float *)FUN_004ff760(local_4c,local_20,local_1c);
      FUN_004fee50(local_50,pfVar6);
      local_58 = local_14 + 0x22;
      local_54 = (void *)FUN_00464250(*(int *)((int)local_18 + 0x14858));
      pfVar6 = (float *)FUN_004ff630(local_54,local_20,local_1c);
      FUN_004fee50(local_58,pfVar6);
      iVar3 = FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
      local_6c = (void *)FUN_004ff690(iVar3);
      iVar3 = FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
      bVar2 = FUN_004ff5e0(iVar3);
      if (bVar2) {
        local_5c = (void *)((int)local_18 + 0x684);
        iVar3 = FUN_004ba3e0(local_5c,local_1c);
        local_24 = (int *)(iVar3 + 0x88);
      }
      else {
        local_60 = (void *)((int)local_18 + 0x684);
        iVar3 = FUN_004ba3e0(local_60,local_1c);
        local_24 = (int *)(iVar3 + 0x80);
      }
      local_64 = local_24;
      local_68 = local_24;
      puVar5 = (undefined4 *)FUN_004f58f0(local_6c,local_a8,local_24);
      local_98 = *puVar5;
      local_94 = puVar5[1];
      local_70 = (void *)((int)local_18 + 0x684);
      iVar3 = FUN_004ba3e0(local_70,local_1c);
      local_28 = (undefined4 *)(iVar3 + 0x70);
      *local_28 = local_98;
      *(undefined4 *)(iVar3 + 0x74) = local_94;
      uVar1 = *(undefined4 *)(iVar3 + 0x74);
      local_14[0x1e] = *local_28;
      local_14[0x1f] = uVar1;
      iVar3 = FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
      local_80 = FUN_00421810(iVar3);
      local_74 = *(int *)((int)local_18 + 0x2208) + 0x88;
      iVar3 = FUN_00464230(0);
      local_78 = FUN_00534130(iVar3);
      local_78 = local_78 * 4;
      local_7c = *(int *)(local_74 + local_78);
      puVar5 = FUN_00450c70(&local_9c,(uint *)0x0,local_7c,0xe,(undefined4 *)0x0);
      local_14[0x37] = *puVar5;
      iVar3 = FUN_0044ced0(local_14 + 0x37);
      iVar3 = FUN_0044cb20(iVar3);
      *(undefined4 *)(iVar3 + 4) = DAT_0056fd7c;
      *local_14 = 2;
      local_14[0x42] = 0;
      local_14[0x40] = local_14[0x40] & 0xfffffffd;
      local_14[0x43] = 0;
      local_84 = (void *)FUN_00464230(0);
      FUN_00533180(local_84,local_14,local_1c);
      local_14 = local_14 + 0x4b;
    }
    for (; local_1c < 10; local_1c = local_1c + 1) {
      *local_14 = 0;
      local_14[0x42] = 0;
      FUN_00479040(local_14 + 0x37);
      local_14 = local_14 + 0x4b;
    }
    *(int *)((int)local_18 + 0x680) = local_20;
    local_88 = (void *)FUN_0040ff90(*(int *)((int)local_18 + 0x14858));
    FUN_004fb290(local_88,1);
  }
  local_8c = (void *)((int)local_18 + 0x123c);
  local_14 = (undefined4 *)FUN_004ba3e0(local_8c,0);
  for (local_1c = 0; local_1c < 0xc; local_1c = local_1c + 1) {
    *local_14 = 0;
    local_14[0x42] = 0;
    local_14[0x40] = local_14[0x40] & 0xfffffffd;
    local_14 = local_14 + 0x4b;
  }
  return;
}

