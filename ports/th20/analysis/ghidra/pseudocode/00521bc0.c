/* Entry: 0x00521bc0; symbol: FUN_00521bc0; body bytes: 2042 */

undefined4 FUN_00521bc0(void)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined3 extraout_var;
  undefined4 uVar5;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  void *local_8c;
  void *local_88;
  void *local_84;
  void *local_80;
  undefined4 *local_7c;
  undefined4 *local_78;
  void *local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  undefined4 *local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  undefined4 local_38;
  void *local_34;
  uint local_30;
  uint local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  iVar3 = FUN_004640a0(0x5ba568);
  if (iVar3 < 4) {
    local_10 = 0x3a;
  }
  else {
    local_10 = 0x3b;
  }
  local_c = local_10;
  local_14 = *(undefined4 *)((int)local_8 + 0x20);
  switch(local_14) {
  case 0:
    local_34 = (void *)((int)local_8 + 0x390);
    bVar2 = FUN_00449430(local_34,0);
    if (bVar2) {
      local_38 = FUN_00437950(DAT_005c0698);
      puVar4 = FUN_00450c70(&local_90,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar4;
    }
    local_3c = (void *)((int)local_8 + 0x24);
    iVar3 = FUN_004640a0(0x5ba568);
    if (iVar3 < 4) {
      local_18 = 4;
    }
    else {
      local_18 = 1;
    }
    FUN_0049c4e0(local_3c,local_18);
    local_40 = (void *)((int)local_8 + 0x24);
    FUN_004bfbe0(local_40,0);
    FUN_0052cc30(local_8,local_c);
    FUN_0052cf80(local_8,local_c);
    FUN_0052cc90(local_8,local_c);
    local_44 = (void *)((int)local_8 + 0x24);
    iVar3 = FUN_0051e080(local_44,7);
    FUN_0052ccd0(local_8,local_c,(int)(short)((short)iVar3 + 6));
    if (DAT_005c6128 != 4) {
      FUN_0052cf80(local_8,0x22);
      FUN_0052ce50(local_8,1);
      iVar3 = FUN_004640a0(0x5ba568);
      if (iVar3 < 4) {
        cVar1 = FUN_0052c640(DAT_005c6108,0);
        if (cVar1 == '\0') {
          FUN_0052c240(local_8,&local_98,local_c,0x11);
          FUN_00450120();
        }
        cVar1 = FUN_0052c640(DAT_005c6108,1);
        if (cVar1 == '\0') {
          FUN_0052c240(local_8,&local_9c,local_c,0x12);
          FUN_00450120();
        }
        cVar1 = FUN_0052c640(DAT_005c6108,2);
        if (cVar1 == '\0') {
          FUN_0052c240(local_8,&local_a0,local_c,0x13);
          FUN_00450120();
        }
        cVar1 = FUN_0052c640(DAT_005c6108,3);
        if (cVar1 == '\0') {
          FUN_0052c240(local_8,&local_a4,local_c,0x14);
          FUN_00450120();
        }
      }
      else {
        cVar1 = FUN_0052c640(DAT_005c6108,4);
        if (cVar1 == '\0') {
          FUN_0052c240(local_8,&local_a8,local_c,0x15);
          FUN_00450120();
        }
      }
      goto switchD_00521c07_caseD_1;
    }
    iVar3 = FUN_004640a0(0x5ba568);
    FUN_004bed50((void *)((int)local_8 + 0x24),iVar3);
    FUN_0052cc90(local_8,local_c);
    local_48 = (void *)((int)local_8 + 0x24);
    iVar3 = FUN_0051e080(local_48,7);
    FUN_0052cd10(local_8,local_c,(int)(short)iVar3);
    FUN_0052cd10(local_8,local_c,6);
    iVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    local_4c = FUN_0052c240(local_8,&local_94,local_c,iVar3 + 0x30);
    FUN_0044ef90(local_4c,2);
    goto LAB_005221ec;
  case 1:
switchD_00521c07_caseD_1:
    local_50 = (void *)((int)local_8 + 0x154);
    bVar2 = FUN_00461070(local_50,6);
    if (bVar2) {
      FUN_0052ce50(local_8,2);
    }
    break;
  case 2:
    iVar3 = FUN_004640a0(0x5ba568);
    if (iVar3 < 4) {
      FUN_004bfc00((undefined4 *)((int)local_8 + 0x24));
      if (DAT_005b889c == (void *)0x0) {
        local_1c = 0;
      }
      else {
        local_54 = DAT_005b889c;
        local_1c = FUN_0041a280(DAT_005b889c,0x10);
      }
      if (local_1c == 0) {
        if (DAT_005b889c == (void *)0x0) {
          local_20 = 0;
        }
        else {
          local_58 = DAT_005b889c;
          local_20 = FUN_0041a280(DAT_005b889c,0x40);
        }
        if (local_20 != 0) goto LAB_00521f77;
      }
      else {
LAB_00521f77:
        local_5c = (void *)((int)local_8 + 0x24);
        FUN_004bf040(local_5c);
      }
      if (DAT_005b889c == (void *)0x0) {
        local_24 = 0;
      }
      else {
        local_60 = DAT_005b889c;
        local_24 = FUN_0041a280(DAT_005b889c,0x20);
      }
      if (local_24 == 0) {
        if (DAT_005b889c == (void *)0x0) {
          local_28 = 0;
        }
        else {
          local_64 = DAT_005b889c;
          local_28 = FUN_0041a280(DAT_005b889c,0x80);
        }
        if (local_28 != 0) goto LAB_00522012;
      }
      else {
LAB_00522012:
        local_68 = (void *)((int)local_8 + 0x24);
        FUN_004bf020(local_68);
      }
      bVar2 = FUN_0045d030((int *)((int)local_8 + 0x24));
      if (CONCAT31(extraout_var,bVar2) != 0) {
        FUN_00426d70(&DAT_005ba830,10,0);
        FUN_0052cc90(local_8,local_c);
        local_6c = (void *)((int)local_8 + 0x24);
        iVar3 = FUN_0051e080(local_6c,7);
        FUN_0052ccd0(local_8,local_c,(int)(short)iVar3);
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_2c = 0;
    }
    else {
      local_70 = DAT_005b889c;
      local_2c = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_2c == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_30 = 0;
      }
      else {
        local_74 = DAT_005b889c;
        local_30 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_30 != 0) {
        FUN_0052ccd0(local_8,local_c,6);
        iVar3 = FUN_004640a0(0x5ba568);
        if (iVar3 < 4) {
          iVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          local_78 = FUN_0052c240(local_8,&local_ac,local_c,iVar3 + 0x30);
          FUN_0044ef90(local_78,2);
        }
        else {
          local_7c = FUN_0052c240(local_8,&local_b0,local_c,0x34);
          FUN_0044ef90(local_7c,2);
        }
        FUN_0052ce50(local_8,3);
        FUN_00426d70(&DAT_005ba830,7,0);
      }
    }
    else {
      FUN_0052ce50(local_8,4);
      FUN_00426d70(&DAT_005ba830,9,0);
      FUN_0052cc30(local_8,local_c);
    }
    break;
  case 3:
    local_80 = (void *)((int)local_8 + 0x154);
    bVar2 = FUN_004235c0(local_80,0xe);
    if (!bVar2) {
      return 1;
    }
    FUN_0052cc30(local_8,0x22);
LAB_005221ec:
    FUN_0052ced0(local_8,6);
    iVar3 = FUN_004640a0(0x5ba568);
    if (iVar3 < 4) {
      uVar5 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      FUN_004be1b0(&DAT_005ba568,uVar5);
      DAT_005b0a60 = FUN_004640a0(0x5ba568);
    }
    FUN_004c60b0((int)local_8 + 0x24);
    local_84 = (void *)((int)local_8 + 0x24);
    FUN_004bfbe0(local_84,1);
    local_88 = (void *)((int)local_8 + 0x24);
    FUN_0049c4e0(local_88,2);
    FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005c6140);
    FUN_0050a3e0(&DAT_005ba568,DAT_005c6140);
    break;
  case 4:
    local_8c = (void *)((int)local_8 + 0x154);
    bVar2 = FUN_004235c0(local_8c,6);
    if (bVar2) {
      FUN_0052cc30(local_8,0x22);
      bVar2 = FUN_004bd820(0x5ba568);
      if (bVar2) {
        DAT_005b0a60 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        FUN_004be1b0(&DAT_005ba568,DAT_005b0a60);
        FUN_0052ced0(local_8,1);
        FUN_00479040((void *)((int)local_8 + 0x390));
        FUN_004117a0((void *)((int)local_8 + 0x390),0);
        FUN_0052ced0(local_8,1);
      }
      else {
        iVar3 = FUN_004640a0(0x5ba568);
        if (iVar3 < 4) {
          FUN_0052ced0(local_8,1);
          uVar5 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          FUN_004be1b0(&DAT_005ba568,uVar5);
          DAT_005b0a60 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        }
        else {
          FUN_004be1b0(&DAT_005ba568,DAT_005b0a60);
          FUN_0052ced0(local_8,1);
          FUN_00479040((void *)((int)local_8 + 0x390));
          FUN_004117a0((void *)((int)local_8 + 0x390),0);
          FUN_0052ced0(local_8,1);
        }
      }
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  return 1;
}

