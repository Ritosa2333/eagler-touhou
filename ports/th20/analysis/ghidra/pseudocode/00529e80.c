/* Entry: 0x00529e80; symbol: FUN_00529e80; body bytes: 2054 */

undefined4 __fastcall FUN_00529e80(void *param_1)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 *puVar3;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar4;
  undefined4 local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  undefined4 local_40;
  void *local_3c;
  undefined4 local_38;
  void *local_34;
  int local_30;
  void *local_2c;
  void *local_28;
  undefined4 local_24;
  undefined4 local_20;
  uint local_1c;
  uint local_18;
  int local_14;
  int local_10;
  undefined4 local_c;
  void *local_8;
  
  local_c = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_c) {
  case 0:
    FUN_00479040((void *)((int)param_1 + 0x390));
    FUN_004117a0((void *)((int)local_8 + 0x390),0);
    local_28 = (void *)((int)local_8 + 0x24);
    FUN_0049c4e0(local_28,10);
    local_2c = (void *)((int)local_8 + 0x24);
    FUN_004bfbe0(local_2c,1);
    cVar1 = FUN_0052c7b0(DAT_005c6108);
    if (cVar1 == '\0') {
      local_30 = (int)local_8 + 0x24;
      FUN_0051d810(1);
    }
    bVar2 = FUN_00488850(0x5ba568);
    if (bVar2) {
      FUN_004bed50((void *)((int)local_8 + 0x24),3);
      FUN_0050a4a0(&DAT_005ba568,0);
    }
    else {
      bVar2 = FUN_004bd820(0x5ba568);
      if (bVar2) {
        FUN_004bed50((void *)((int)local_8 + 0x24),2);
        FUN_0050a4a0(&DAT_005ba568,0);
      }
    }
    FUN_0052ce50(local_8,1);
    if ((*(uint *)((int)local_8 + 0x58d4) >> 1 & 1) == 0) {
      bVar2 = FUN_0052c750(local_8,0);
      if (CONCAT31(extraout_var,bVar2) == 0) {
        FUN_0052cf80(local_8,0);
        FUN_0052cbf0(local_8,0);
      }
      bVar2 = FUN_0052c750(local_8,0x1f);
      if (CONCAT31(extraout_var_00,bVar2) == 0) {
        FUN_0052cf80(local_8,0x1f);
        FUN_0052cbf0(local_8,0x1f);
      }
      if (*(int *)((int)local_8 + 0x1c) != 3) {
        FUN_0052cbf0(local_8,0);
      }
      FUN_00423520((void *)((int)local_8 + 0x154),0x78);
    }
    else {
      FUN_0052cf80(local_8,0);
      FUN_0052cf80(local_8,0x1f);
      *(uint *)((int)local_8 + 0x58d4) = *(uint *)((int)local_8 + 0x58d4) & 0xfffffffd;
    }
  case 1:
    local_34 = (void *)((int)local_8 + 0x154);
    bVar2 = FUN_004785e0(local_34,0x78);
    if ((bVar2) && (bVar2 = FUN_0047a340((undefined4 *)((int)local_8 + 0x474)), !bVar2)) {
      local_38 = *(undefined4 *)((int)local_8 + 0x14);
      puVar3 = FUN_00450c70(&local_60,(uint *)0x0,0,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x474) = *puVar3;
    }
    local_3c = (void *)((int)local_8 + 0x154);
    bVar2 = FUN_00461070(local_3c,0x82);
    if (bVar2) {
      FUN_0052ce50(local_8,2);
    }
    break;
  case 2:
    if (DAT_005c5b38 == (undefined4 *)0x0) {
      bVar2 = FUN_0052c9f0((int)DAT_005c6108);
      if (CONCAT31(extraout_var_01,bVar2) != 0) {
        local_40 = FUN_0052c320((int)DAT_005c6108);
        FUN_004dfb90(local_40);
        return 1;
      }
    }
    else {
      iVar4 = FUN_00426370((int)DAT_005c5b38);
      if (iVar4 == 0) {
        return 1;
      }
      FUN_004217c0(DAT_005c5b38);
    }
    FUN_004bfc00((undefined4 *)((int)local_8 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_10 = 0;
    }
    else {
      local_44 = DAT_005b889c;
      local_10 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_10 != 0) {
      local_48 = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_48);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_14 = 0;
    }
    else {
      local_4c = DAT_005b889c;
      local_14 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_14 != 0) {
      local_50 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_50);
    }
    bVar2 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var_02,bVar2) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      FUN_00423520((void *)((int)local_8 + 0x5904),8);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_18 = 0;
    }
    else {
      local_54 = DAT_005b889c;
      local_18 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_18 == 0) {
LAB_0052a264:
      if (DAT_005b889c == (void *)0x0) {
        local_1c = 0;
      }
      else {
        local_58 = DAT_005b889c;
        local_1c = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_1c == 0) {
        return 1;
      }
      FUN_00423520((void *)((int)local_8 + 0x5914),0x1e);
      local_20 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      switch(local_20) {
      case 0:
      case 1:
      case 2:
      case 3:
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052cc30(local_8,0x1f);
        FUN_00479040((void *)((int)local_8 + 0x474));
        FUN_0052ce50(local_8,4);
        break;
      case 4:
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052cc30(local_8,0x1f);
        FUN_00479040((void *)((int)local_8 + 0x474));
        FUN_0052ce50(local_8,4);
        break;
      case 5:
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052cc30(local_8,0x1f);
        FUN_00479040((void *)((int)local_8 + 0x474));
        FUN_0052ce50(local_8,4);
        break;
      case 6:
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052cc30(local_8,0x1f);
        FUN_00479040((void *)((int)local_8 + 0x474));
        FUN_0052ce50(local_8,4);
        break;
      case 7:
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052cc30(local_8,0x1f);
        FUN_00479040((void *)((int)local_8 + 0x474));
        FUN_0052ce50(local_8,4);
        break;
      case 8:
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052cc30(local_8,0x1f);
        FUN_00479040((void *)((int)local_8 + 0x474));
        FUN_0052ce50(local_8,4);
        break;
      case 9:
        goto switchD_0052a2d9_caseD_9;
      }
    }
    else {
      iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      if (iVar4 != 9) {
        FUN_00426d70(&DAT_005ba830,9,0);
        FUN_004bed50((void *)((int)local_8 + 0x24),9);
        goto LAB_0052a264;
      }
switchD_0052a2d9_caseD_9:
      FUN_00426d70(&DAT_005ba830,9,0);
      FUN_0052ce50(local_8,4);
    }
    break;
  case 4:
    local_5c = (void *)((int)param_1 + 0x154);
    bVar2 = FUN_004235c0(local_5c,0x14);
    if (bVar2) {
      local_24 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      switch(local_24) {
      case 0:
        FUN_0050a4a0(&DAT_005ba568,0);
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,5);
        FUN_004c60b0((int)local_8 + 0x24);
        FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005b0a60);
        FUN_004be1b0(&DAT_005ba568,DAT_005b0a60);
        break;
      case 1:
        FUN_0050a4a0(&DAT_005ba568,0);
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,5);
        FUN_004c60b0((int)local_8 + 0x24);
        FUN_004be1b0(&DAT_005ba568,4);
        FUN_004bed50((void *)((int)local_8 + 0x24),0);
        break;
      case 2:
        FUN_0050a4a0(&DAT_005ba568,1);
        FUN_0052cc90(local_8,0);
        FUN_004c60b0((int)local_8 + 0x24);
        FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005b0a60);
        FUN_004be1b0(&DAT_005ba568,DAT_005b0a60);
        FUN_0052ced0(local_8,5);
        break;
      case 3:
        FUN_0050a4a0(&DAT_005ba568,2);
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,6);
        FUN_004c60b0((int)local_8 + 0x24);
        FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005c6140);
        FUN_0050a3e0(&DAT_005ba568,DAT_005c6140);
        break;
      case 4:
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,0xc);
        FUN_004c60b0((int)local_8 + 0x24);
        break;
      case 5:
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,10);
        FUN_004c60b0((int)local_8 + 0x24);
        FUN_004bed50((void *)((int)local_8 + 0x24),0);
        break;
      case 6:
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,0xe);
        FUN_004c60b0((int)local_8 + 0x24);
        break;
      case 7:
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,3);
        FUN_004c60b0((int)local_8 + 0x24);
        break;
      case 8:
        FUN_0052cc90(local_8,0);
        FUN_0052ced0(local_8,0x11);
        FUN_004c60b0((int)local_8 + 0x24);
        break;
      case 9:
        FUN_0052ced0(local_8,2);
      }
    }
  }
  return 1;
}

