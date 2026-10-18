/* Entry: 0x00521060; symbol: FUN_00521060; body bytes: 2881 */

/* WARNING: Type propagation algorithm not settling */

void FUN_00521060(void)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 uVar4;
  undefined4 local_140;
  undefined4 local_13c;
  undefined4 local_138;
  undefined4 local_134;
  undefined4 local_130;
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_124;
  undefined4 local_120;
  undefined4 local_11c;
  undefined4 local_118;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  undefined4 local_104;
  void *local_100;
  void *local_fc;
  int local_f8;
  GpImage *local_f4;
  undefined4 local_f0;
  void *local_ec;
  int local_e8;
  GpImage *local_e4;
  undefined4 local_e0;
  void *local_dc;
  int local_d8;
  GpImage *local_d4;
  undefined4 local_d0;
  void *local_cc;
  int local_c8;
  GpImage *local_c4;
  undefined4 local_c0;
  void *local_bc;
  void *local_b8;
  void *local_b4;
  void *local_b0;
  undefined4 *local_ac;
  undefined4 *local_a8;
  undefined4 *local_a4;
  undefined4 *local_a0;
  void *local_9c;
  void *local_98;
  void *local_94;
  void *local_90;
  void *local_8c;
  undefined4 *local_88;
  int local_84;
  undefined4 *local_80;
  int local_7c;
  void *local_78;
  undefined4 *local_74;
  undefined4 *local_70;
  undefined4 *local_6c;
  undefined4 *local_68;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  void *local_54;
  int local_50;
  uint local_4c;
  uint local_48;
  int local_44;
  int local_40;
  undefined4 *local_3c;
  int local_38;
  int *local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  void *local_24;
  int local_20 [3];
  undefined4 local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar3 = FUN_004640a0(0x5ba568);
  if (iVar3 == 4) {
    local_2c = 0xe;
  }
  else {
    local_2c = 0xc;
  }
  local_28 = local_2c;
  local_30 = *(undefined4 *)((int)local_24 + 0x20);
  switch(local_30) {
  case 0:
    local_54 = (void *)((int)local_24 + 0x24);
    FUN_0049c4e0(local_54,2);
    iVar3 = FUN_004640a0(0x5ba568);
    if (iVar3 == 4) {
      local_58 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      cVar1 = FUN_0052c870(DAT_005c6108,local_58);
      if (cVar1 == '\0') {
        local_20[1] = 0;
        local_34 = local_20 + 1;
        FUN_00461510(&local_c);
        FUN_0050ae60(local_20);
        while (bVar2 = FUN_00449460(&local_c,local_20), bVar2) {
          local_38 = FUN_0040c300(&local_c);
          cVar1 = FUN_0052c870(DAT_005c6108,local_38);
          if (cVar1 != '\0') {
            FUN_004bed50((void *)((int)local_24 + 0x24),local_38);
            break;
          }
          FUN_00461860(&local_c);
        }
      }
      local_14 = 0;
      local_3c = &local_14;
      FUN_00461510(&local_10);
      FUN_0050ae60(local_20 + 2);
      while (bVar2 = FUN_00449460(&local_10,local_20 + 2), bVar2) {
        local_50 = FUN_0040c300(&local_10);
        cVar1 = FUN_0052c870(DAT_005c6108,local_50);
        if (cVar1 == '\0') {
          local_5c = (int)local_24 + 0x24;
          FUN_0051d810(local_50);
        }
        FUN_00461860(&local_10);
      }
    }
    bVar2 = FUN_0052c750(local_24,0x23);
    if (CONCAT31(extraout_var,bVar2) == 0) {
      FUN_0052cf80(local_24,0x23);
    }
    bVar2 = FUN_0052c750(local_24,local_28);
    if (CONCAT31(extraout_var_00,bVar2) == 0) {
      FUN_0052cc30(local_24,local_28);
      FUN_0052cf80(local_24,local_28);
      FUN_0052cc90(local_24,local_28);
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052ccd0(local_24,local_28,(int)(short)((short)uVar4 + 7));
    }
    else {
      FUN_0052cc90(local_24,local_28);
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052ccd0(local_24,local_28,(int)(short)((short)uVar4 + 7));
    }
    FUN_0052ce50(local_24,1);
    local_60 = FUN_004640a0(0x5ba568);
    iVar3 = FUN_0052c4f0(DAT_005c6108,local_60,0);
    if (iVar3 == 0) {
      FUN_0052c240(local_24,&local_104,local_28,0xf);
      FUN_00450120();
    }
    else {
      FUN_0052c240(local_24,&local_108,local_28,0xf);
      FUN_0044fa30();
    }
    local_64 = FUN_004640a0(0x5ba568);
    iVar3 = FUN_0052c4f0(DAT_005c6108,local_64,1);
    if (iVar3 == 0) {
      FUN_0052c240(local_24,&local_10c,local_28,0x10);
      FUN_00450120();
    }
    else {
      FUN_0052c240(local_24,&local_110,local_28,0x10);
      FUN_0044fa30();
    }
    if ((DAT_005c6128 != 4) && (DAT_005c6128 != 5)) goto switchD_005210b1_caseD_1;
    iVar3 = FUN_004b7f70(0x5ba568);
    FUN_004bed50((void *)((int)local_24 + 0x24),iVar3);
    uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
    FUN_0052cd10(local_24,local_28,(int)(short)((short)uVar4 + 7));
    iVar3 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
    local_68 = FUN_0052c240(local_24,&local_114,local_28,iVar3 + 6);
    FUN_0044ef90(local_68,6);
    iVar3 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
    local_6c = FUN_0052c240(local_24,&local_118,local_28,iVar3 + 8);
    FUN_0044ef90(local_6c,6);
    local_70 = FUN_0052c240(local_24,&local_11c,local_28,4);
    FUN_0044ef90(local_70,6);
    local_74 = FUN_0052c240(local_24,&local_120,local_28,5);
    FUN_0044ef90(local_74,6);
    goto LAB_0052183f;
  case 1:
switchD_005210b1_caseD_1:
    local_78 = (void *)((int)local_24 + 0x154);
    bVar2 = FUN_00461070(local_78,6);
    if (bVar2) {
      FUN_0052ce50(local_24,2);
      local_80 = FUN_0052c240(local_24,&local_124,local_28,6);
      iVar3 = FUN_00464100((int)DAT_005c6108);
      local_7c = iVar3 / 2 + 0x1e;
      FUN_00464510(local_80,local_7c);
      local_88 = FUN_0052c240(local_24,&local_128,local_28,7);
      iVar3 = FUN_00464100((int)DAT_005c6108);
      local_84 = iVar3 / 2 + 0x1e;
      FUN_00464510(local_88,local_84);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)local_24 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_40 = 0;
    }
    else {
      local_8c = DAT_005b889c;
      local_40 = FUN_0041a280(DAT_005b889c,0x40);
    }
    if (local_40 != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052cd10(local_24,local_28,(int)(short)((short)uVar4 + 0x19));
      local_90 = (void *)((int)local_24 + 0x24);
      FUN_004bf040(local_90);
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052ccd0(local_24,local_28,(int)(short)((short)uVar4 + 0xd));
    }
    if (DAT_005b889c == (void *)0x0) {
      local_44 = 0;
    }
    else {
      local_94 = DAT_005b889c;
      local_44 = FUN_0041a280(DAT_005b889c,0x80);
    }
    if (local_44 != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052cd10(local_24,local_28,(int)(short)((short)uVar4 + 0x13));
      local_98 = (void *)((int)local_24 + 0x24);
      FUN_004bf020(local_98);
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052ccd0(local_24,local_28,(int)(short)((short)uVar4 + 7));
    }
    if (DAT_005b889c == (void *)0x0) {
      local_48 = 0;
    }
    else {
      local_9c = DAT_005b889c;
      local_48 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_48 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_4c = 0;
      }
      else {
        local_b0 = DAT_005b889c;
        local_4c = FUN_00419c00(DAT_005b889c,0x106);
      }
      if (local_4c != 0) {
        FUN_0052ce50(local_24,4);
        FUN_00426d70(&DAT_005ba830,9,0);
      }
    }
    else {
      iVar3 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      local_a0 = FUN_0052c240(local_24,&local_12c,local_28,iVar3 + 6);
      FUN_0044ef90(local_a0,6);
      iVar3 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      local_a4 = FUN_0052c240(local_24,&local_130,local_28,iVar3 + 8);
      FUN_0044ef90(local_a4,6);
      local_a8 = FUN_0052c240(local_24,&local_134,local_28,4);
      FUN_0044ef90(local_a8,6);
      local_ac = FUN_0052c240(local_24,&local_138,local_28,5);
      FUN_0044ef90(local_ac,6);
      FUN_00426d70(&DAT_005ba830,7,0);
      FUN_0052ce50(local_24,3);
    }
    break;
  case 3:
    local_b4 = (void *)((int)local_24 + 0x154);
    bVar2 = FUN_004235c0(local_b4,0xe);
    if (!bVar2) break;
LAB_0052183f:
    FUN_0050f660((int)DAT_005c6108);
    FUN_0052ced0(local_24,7);
    uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
    FUN_0050a3e0(&DAT_005ba568,uVar4);
    DAT_005c6140 = FUN_004b7f70(0x5ba568);
    FUN_004c60b0((int)local_24 + 0x24);
    local_b8 = (void *)((int)local_24 + 0x24);
    FUN_004bfbe0(local_b8,1);
    local_bc = (void *)((int)local_24 + 0x24);
    FUN_0049c4e0(local_bc,9);
    FUN_004bed50((void *)((int)local_24 + 0x24),DAT_005c6144);
    local_cc = (void *)FUN_00464230(0);
    local_c0 = FUN_004b7f70(0x5ba568);
    local_c4 = (GpImage *)FUN_00464100((int)DAT_005c6108);
    local_c8 = FUN_004b7f70(0x5ba568);
    FUN_00534420(local_cc,local_c8,local_c4);
    local_dc = (void *)FUN_00464230(0);
    local_d0 = FUN_004b7f70(0x5ba568);
    local_d4 = (GpImage *)FUN_00464100((int)DAT_005c6108);
    local_d8 = FUN_004b7f70(0x5ba568);
    FUN_005347d0(local_dc,local_d8,local_d4);
    local_ec = (void *)FUN_00464230(0);
    local_e0 = FUN_004b7f70(0x5ba568);
    local_e4 = (GpImage *)FUN_00464100((int)DAT_005c6108);
    local_e8 = FUN_004b7f70(0x5ba568);
    FUN_005344d0(local_ec,local_e8,local_e4);
    local_fc = (void *)FUN_00464230(0);
    local_f0 = FUN_004b7f70(0x5ba568);
    local_f4 = (GpImage *)FUN_00464100((int)DAT_005c6108);
    local_f8 = FUN_004b7f70(0x5ba568);
    FUN_00534600(local_fc,local_f8,local_f4);
    FUN_0052c240(local_24,&local_13c,local_28,0xf);
    FUN_00450120();
    FUN_0052c240(local_24,&local_140,local_28,0x10);
    FUN_00450120();
    FUN_0052ccd0(local_24,local_28,6);
    FUN_0052cc30(local_24,0x23);
    break;
  case 4:
    local_100 = (void *)((int)local_24 + 0x154);
    bVar2 = FUN_004235c0(local_100,6);
    if (bVar2) {
      FUN_0050f660((int)DAT_005c6108);
      bVar2 = FUN_00488850(0x5ba568);
      if (bVar2) {
        FUN_0052cc30(local_24,local_28);
        FUN_0052cc30(local_24,0x23);
        FUN_0052ced0(local_24,1);
        uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
        FUN_0050a3e0(&DAT_005ba568,uVar4);
        FUN_004c6010((undefined4 *)((int)local_24 + 0x24));
        DAT_005c6140 = FUN_004b7f70(0x5ba568);
      }
      else {
        FUN_0052cc30(local_24,local_28);
        FUN_0052cc30(local_24,0x23);
        FUN_0052ced0(local_24,5);
        uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
        FUN_0050a3e0(&DAT_005ba568,uVar4);
        FUN_004c6010((undefined4 *)((int)local_24 + 0x24));
        DAT_005c6140 = FUN_004b7f70(0x5ba568);
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

