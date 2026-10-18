/* Entry: 0x00523440; symbol: FUN_00523440; body bytes: 3184 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00523440(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar2;
  undefined3 extraout_var_02;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  undefined4 local_124;
  undefined4 local_120;
  void *local_11c;
  uint local_118;
  void *local_114;
  void *local_110;
  void *local_10c;
  undefined4 local_108;
  void *local_104;
  undefined4 local_100;
  void *local_fc;
  undefined4 local_f8;
  void *local_f4;
  undefined4 local_f0;
  void *local_ec;
  void *local_e8;
  void *local_e4;
  undefined4 local_e0;
  void *local_dc;
  void *local_d8;
  void *local_d4;
  void *local_d0;
  void *local_cc;
  void *local_c8;
  void *local_c4;
  void *local_c0;
  void *local_bc;
  void *local_b8;
  int local_b4;
  void *local_b0;
  void *local_ac;
  void *local_a8;
  void *local_a4;
  void *local_a0;
  void *local_9c;
  void *local_98;
  void *local_94;
  void *local_90;
  void *local_8c;
  void *local_88;
  void *local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  void *local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  int local_64;
  void *local_60;
  void *local_5c;
  uint local_58;
  undefined4 local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  uint local_40;
  uint local_3c;
  int local_38;
  int local_34;
  uint local_30;
  uint local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  undefined4 local_18;
  int local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  local_18 = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_18) {
  case 0:
    local_44 = (void *)((int)param_1 + 0x24);
    FUN_0049c4e0(local_44,0x19);
    FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005c612c % 0x19);
    local_48 = (void *)((int)local_8 + 0x108);
    FUN_0049c4e0(local_48,3);
    FUN_004bed50((void *)((int)local_8 + 0x108),DAT_005c612c / 0x19);
    local_4c = (void *)((int)local_8 + 0x108);
    FUN_004bfbe0(local_4c,1);
    DAT_005c612c = 0;
    local_50 = (void *)((int)local_8 + 0x390);
    bVar1 = FUN_00449430(local_50,0);
    if (bVar1) {
      local_54 = FUN_00437950(DAT_005c0698);
      puVar6 = FUN_00450c70(&local_120,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar6;
    }
    FUN_0052cf80(local_8,0x26);
    FUN_0052ce50(local_8,1);
    local_5c = (void *)((int)local_8 + 0x5740);
    local_58 = 0;
    FUN_0052c220(local_5c,&local_58);
    *(undefined4 *)((int)local_8 + 0x5734) = 0;
    *(uint *)((int)local_8 + 0x58d4) = *(uint *)((int)local_8 + 0x58d4) & 0xfffffff7;
    *(uint *)((int)local_8 + 0x58d4) = *(uint *)((int)local_8 + 0x58d4) & 0xfffffffb;
    local_64 = (int)local_8 + 0x5968;
    local_60 = local_8;
    FUN_0040b1d0(FUN_0051fd60,&local_60);
    bVar1 = FUN_0052c750(local_8,0);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_0052cf80(local_8,0);
      FUN_0052cc90(local_8,0);
    }
  case 1:
    local_68 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_00461070(local_68,6);
    if (bVar1) {
      FUN_0052ce50(local_8,2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    FUN_004bfc00((undefined4 *)((int)local_8 + 0x108));
    if (DAT_005b889c == (void *)0x0) {
      local_1c = 0;
    }
    else {
      local_6c = DAT_005b889c;
      local_1c = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_1c != 0) {
      local_70 = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_70);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_20 = 0;
    }
    else {
      local_74 = DAT_005b889c;
      local_20 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_20 != 0) {
      local_78 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_78);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_24 = 0;
    }
    else {
      local_7c = DAT_005b889c;
      local_24 = FUN_0041a280(DAT_005b889c,0x40);
    }
    if (local_24 != 0) {
      local_80 = (void *)((int)local_8 + 0x108);
      FUN_004bf040(local_80);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_28 = 0;
    }
    else {
      local_84 = DAT_005b889c;
      local_28 = FUN_0041a280(DAT_005b889c,0x80);
    }
    if (local_28 != 0) {
      local_88 = (void *)((int)local_8 + 0x108);
      FUN_004bf020(local_88);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x108));
    if (CONCAT31(extraout_var_00,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var_01,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_2c = 0;
    }
    else {
      local_8c = DAT_005b889c;
      local_2c = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_2c != 0) {
      FUN_0052ce50(local_8,5);
      FUN_00426d70(&DAT_005ba830,9,0);
      *(uint *)((int)local_8 + 0x58d4) = *(uint *)((int)local_8 + 0x58d4) | 4;
      return 1;
    }
    if (DAT_005b889c == (void *)0x0) {
      local_30 = 0;
    }
    else {
      local_90 = DAT_005b889c;
      local_30 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_30 == 0) {
      return 1;
    }
    local_94 = (void *)((int)local_8 + 0x5740);
    iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x108));
    piVar5 = (int *)FUN_00414580(local_94,iVar4 + iVar2 * 0x19);
    if (*piVar5 == 0) {
      return 1;
    }
    FUN_0052ce50(local_8,4);
    iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x108));
    *(int *)((int)local_8 + 0x5738) = iVar4 + iVar2 * 0x19;
    FUN_004c60b0((int)local_8 + 0x24);
    FUN_00426d70(&DAT_005ba830,7,0);
    local_98 = (void *)((int)local_8 + 0x24);
    FUN_0049c4e0(local_98,7);
    FUN_004bed50((void *)((int)local_8 + 0x24),0);
    for (local_10 = 0; local_10 < 7; local_10 = local_10 + 1) {
      local_9c = (void *)((int)local_8 + 0x5740);
      puVar6 = (undefined4 *)FUN_00414580(local_9c,*(int *)((int)local_8 + 0x5738));
      local_a0 = (void *)*puVar6;
      iVar4 = FUN_00488770(local_a0,local_10 + 1);
      if (iVar4 == 0) {
        local_a4 = (void *)((int)local_8 + 0x24);
        FUN_0051d790(local_a4,local_10);
      }
    }
    local_a8 = (void *)((int)local_8 + 0x24);
    FUN_004bf040(local_a8);
    local_ac = (void *)((int)local_8 + 0x24);
    FUN_004bf020(local_ac);
    local_b0 = (void *)((int)local_8 + 0x5740);
    piVar5 = (int *)FUN_00414580(local_b0,*(int *)((int)local_8 + 0x5738));
    local_b4 = FUN_00421810(*piVar5);
    if ((*(byte *)(local_b4 + 10) >> 1 & 1) != 1) {
      return 1;
    }
    goto LAB_00523c53;
  case 3:
    iVar4 = FUN_00437520(0);
    iVar4 = FUN_00438520(iVar4);
    if (iVar4 == 0) {
      local_d8 = (void *)((int)local_8 + 0x154);
      FUN_00429420(local_d8);
    }
    else {
      local_dc = (void *)((int)local_8 + 0x154);
      bVar1 = FUN_004785e0(local_dc,2);
      if (bVar1) {
        local_e0 = FUN_00437520(0);
        puVar6 = FUN_0049dcf0(&local_124,0,0,0);
        _DAT_005c4f04 = *puVar6;
        FUN_0051c6b0(&DAT_005c4f04,0);
        FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
      }
      local_e4 = (void *)((int)local_8 + 0x154);
      bVar1 = FUN_004235c0(local_e4,0x20);
      if ((bVar1) && ((*(uint *)((int)local_8 + 0x58d4) >> 3 & 1) != 0)) {
        FUN_0052ced0(local_8,2);
        FUN_004be360(&DAT_005ba568,*(int *)((int)local_8 + 0x573c) + 1);
        FUN_004be250(&DAT_005c4d40,0xd);
        local_e8 = (void *)((int)local_8 + 0x5740);
        piVar5 = (int *)FUN_00414580(local_e8,*(int *)((int)local_8 + 0x5738));
        iVar4 = FUN_0052c300(*piVar5);
        FUN_00548610(&DAT_005c4b20,0x100,iVar4);
        local_ec = (void *)((int)local_8 + 0x5740);
        piVar5 = (int *)FUN_00414580(local_ec,*(int *)((int)local_8 + 0x5738));
        local_c = FUN_00421810(*piVar5);
        FUN_0050a3e0(&DAT_005ba568,*(undefined4 *)(local_c + 0xd8));
        local_f4 = (void *)FUN_00464080(0);
        local_f0 = *(undefined4 *)(local_c + 0xdc);
        FUN_00412dc0(local_f4,local_f0);
        local_fc = (void *)FUN_00464080(0);
        local_f8 = *(undefined4 *)(local_c + 0xe0);
        FUN_0041de50(local_fc,local_f8);
        local_104 = (void *)FUN_00464080(0);
        local_100 = *(undefined4 *)(local_c + 0xe4);
        FUN_00412da0(local_104,local_100);
        local_10c = (void *)FUN_00464080(0);
        local_108 = *(undefined4 *)(local_c + 0xe8);
        FUN_0041de90(local_10c,local_108);
        FUN_004be1b0(&DAT_005ba568,*(undefined4 *)(local_c + 0xf0));
        if ((*(byte *)(local_c + 10) >> 1 & 1) == 1) {
          FUN_0050a4a0(&DAT_005ba568,2);
          FUN_0050a7f0(&DAT_005ba568,*(undefined4 *)(local_c + 0xfc));
        }
        else {
          FUN_0050a4a0(&DAT_005ba568,0);
          FUN_0050a7f0(&DAT_005ba568,0xffffffff);
        }
        DAT_005c6128 = 2;
        DAT_005c612c = *(int *)((int)local_8 + 0x5738);
      }
    }
    break;
  case 4:
    local_b8 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_00423560(local_b8,0xf);
    if (bVar1) {
      return 1;
    }
    FUN_004bfc00((undefined4 *)((int)local_8 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_34 = 0;
    }
    else {
      local_bc = DAT_005b889c;
      local_34 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_34 != 0) {
      local_c0 = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_c0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_38 = 0;
    }
    else {
      local_c4 = DAT_005b889c;
      local_38 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_38 != 0) {
      local_c8 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_c8);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var_02,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_3c = 0;
    }
    else {
      local_cc = DAT_005b889c;
      local_3c = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_3c != 0) {
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
      local_d0 = (void *)((int)local_8 + 0x24);
      FUN_0049c4e0(local_d0,0x19);
      FUN_004e68a0((int)local_8 + 0x24);
      FUN_0052ce50(local_8,2);
      FUN_00426d70(&DAT_005ba830,9,0);
      return 1;
    }
    if (DAT_005b889c == (void *)0x0) {
      local_40 = 0;
    }
    else {
      local_d4 = DAT_005b889c;
      local_40 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_40 == 0) {
      return 1;
    }
LAB_00523c53:
    uVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    *(undefined4 *)((int)local_8 + 0x573c) = uVar3;
    FUN_0052ce50(local_8,3);
    *(uint *)((int)local_8 + 0x58d4) = *(uint *)((int)local_8 + 0x58d4) | 4;
    FUN_00426d70(&DAT_005ba830,0x32,0);
    FUN_004d99d0(DAT_0056fe5c);
    break;
  case 5:
    local_110 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_110,6);
    if ((bVar1) && ((*(uint *)((int)local_8 + 0x58d4) >> 3 & 1) != 0)) {
      for (local_14 = 0; local_14 < 100; local_14 = local_14 + 1) {
        local_114 = (void *)((int)local_8 + 0x5740);
        puVar6 = (undefined4 *)FUN_00414580(local_114,local_14);
        FUN_004217c0((undefined4 *)*puVar6);
      }
      local_11c = (void *)((int)local_8 + 0x5740);
      local_118 = 0;
      FUN_0052c220(local_11c,&local_118);
      FUN_0052cc30(local_8,0x26);
      FUN_00479040((void *)((int)local_8 + 0x390));
      FUN_004117a0((void *)((int)local_8 + 0x390),0);
      FUN_0052ced0(local_8,1);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  return 1;
}

