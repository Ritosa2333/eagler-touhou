/* Entry: 0x005205d0; symbol: FUN_005205d0; body bytes: 1691 */

undefined4 __fastcall FUN_005205d0(void *param_1)

{
  char cVar1;
  bool bVar2;
  undefined4 *puVar3;
  undefined3 extraout_var;
  int iVar4;
  undefined4 uVar5;
  char *pcVar6;
  void *pvVar7;
  undefined4 local_8c;
  void *local_88;
  void *local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  int local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  int local_48;
  undefined4 local_44;
  undefined4 local_40;
  void *local_3c;
  uint local_38;
  uint local_34;
  int local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
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
    local_3c = (void *)((int)param_1 + 0x390);
    bVar2 = FUN_00449430(local_3c,0);
    if (bVar2) {
      local_40 = FUN_00437950(DAT_005c0698);
      puVar3 = FUN_00450c70(&local_8c,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar3;
    }
    FUN_0052cf80(local_8,0x28);
    FUN_0040baa0((void *)((int)local_8 + 0x478),0);
    local_48 = (int)local_8 + 0x5968;
    local_44 = 0;
    FUN_0050ab10(local_8,FUN_0051f890,&local_44);
    *(undefined4 *)((int)local_8 + 0x480) = 0;
    *(undefined4 *)((int)local_8 + 0x484) = 0;
    *(undefined4 *)((int)local_8 + 0x48c) = 0;
    FUN_0052ce50(local_8,1);
  case 1:
    FUN_00423520((void *)((int)local_8 + 0x154),-1);
    cVar1 = FUN_0046b230((int)local_8 + 0x478);
    if (cVar1 == '\0') {
      return 1;
    }
    *(undefined4 *)((int)local_8 + 0x20) = 2;
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (*(int *)((int)local_8 + 0x48c) == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_1c = 0;
      }
      else {
        local_4c = DAT_005b889c;
        local_1c = FUN_0041a280(DAT_005b889c,0x10);
      }
      if (local_1c != 0) {
        local_50 = (void *)((int)local_8 + 0x24);
        FUN_004bf040(local_50);
      }
      if (DAT_005b889c == (void *)0x0) {
        local_20 = 0;
      }
      else {
        local_54 = DAT_005b889c;
        local_20 = FUN_0041a280(DAT_005b889c,0x20);
      }
      if (local_20 != 0) {
        local_58 = (void *)((int)local_8 + 0x24);
        FUN_004bf020(local_58);
      }
      bVar2 = FUN_0045d030((int *)((int)local_8 + 0x24));
      if (CONCAT31(extraout_var,bVar2) != 0) {
        local_24 = 0;
        FUN_00426d70(&DAT_005ba830,10,0);
        iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        if (iVar4 < *(int *)((int)local_8 + 0x56d0)) {
          uVar5 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          *(undefined4 *)((int)local_8 + 0x56d0) = uVar5;
          local_24 = 1;
        }
        else {
          iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          if (*(int *)((int)local_8 + 0x56d0) + 10 <= iVar4) {
            iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
            *(int *)((int)local_8 + 0x56d0) = iVar4 + -9;
            local_24 = 1;
          }
        }
      }
    }
    else {
      if (DAT_005b889c == (void *)0x0) {
        local_28 = 0;
      }
      else {
        local_5c = DAT_005b889c;
        local_28 = FUN_0041a280(DAT_005b889c,0x10);
      }
      if (local_28 == 0) {
        if (DAT_005b889c == (void *)0x0) {
          local_2c = 0;
        }
        else {
          local_60 = DAT_005b889c;
          local_2c = FUN_0041a280(DAT_005b889c,0x20);
        }
        if (local_2c == 0) {
          if (DAT_005b889c == (void *)0x0) {
            local_30 = 0;
          }
          else {
            local_64 = DAT_005b889c;
            local_30 = FUN_0041a280(DAT_005b889c,0x106);
          }
          if (local_30 == 0) goto LAB_00520988;
        }
      }
      *(undefined4 *)((int)local_8 + 0x484) = *(undefined4 *)((int)local_8 + 0x488);
      *(undefined4 *)((int)local_8 + 0x480) = 0;
      FUN_004d9a70(0,(int)local_8 + *(int *)((int)local_8 + 0x484) * 0x40 + 0x490);
      FUN_004d9b50(0,0);
      *(undefined4 *)((int)local_8 + 0x48c) = 0;
    }
LAB_00520988:
    if (*(int *)((int)local_8 + 0x480) < 8) {
      *(int *)((int)local_8 + 0x480) = *(int *)((int)local_8 + 0x480) + 1;
    }
    if (DAT_005b889c == (void *)0x0) {
      local_34 = 0;
    }
    else {
      local_68 = DAT_005b889c;
      local_34 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_34 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_38 = 0;
      }
      else {
        local_7c = DAT_005b889c;
        local_38 = FUN_00419c00(DAT_005b889c,0x106);
      }
      if (local_38 != 0) {
        for (local_10 = 0; local_10 < *(int *)((int)local_8 + 0x47c); local_10 = local_10 + 1) {
          local_80 = (void *)((int)local_8 + 0x3d4);
          pvVar7 = (void *)FUN_00414580(local_80,local_10);
          FUN_00479040(pvVar7);
        }
        for (local_14 = 0; local_14 < 8; local_14 = local_14 + 1) {
          local_84 = (void *)((int)local_8 + 0x454);
          pvVar7 = (void *)FUN_00414580(local_84,local_14);
          FUN_00479040(pvVar7);
        }
        FUN_00426d70(&DAT_005ba830,9,0);
        FUN_0052ce50(local_8,3);
      }
    }
    else {
      for (local_c = 0; local_c < 8; local_c = local_c + 1) {
        local_6c = (void *)((int)local_8 + 0x454);
        local_70 = (void *)FUN_00414580(local_6c,local_c);
        FUN_0044ef90(local_70,3);
      }
      *(undefined4 *)((int)local_8 + 0x488) = *(undefined4 *)((int)local_8 + 0x484);
      uVar5 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      *(undefined4 *)((int)local_8 + 0x484) = uVar5;
      *(undefined4 *)((int)local_8 + 0x480) = 0;
      iVar4 = FUN_004640e0(DAT_005c6108);
      local_78 = (void *)(iVar4 + 0x36);
      local_74 = *(int *)((int)local_8 + 0x484);
      pcVar6 = (char *)FUN_00411c10(local_78,local_74);
      if ((*pcVar6 == '\0') && (*(int *)((int)local_8 + 0x48c) == 0)) {
        FUN_004d9bc0();
        *(undefined4 *)((int)local_8 + 0x48c) = 1;
      }
      else {
        iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        FUN_004d9a70(0,(int)local_8 + iVar4 * 0x40 + 0x490);
        FUN_004d9b50(0,0);
        *(undefined4 *)((int)local_8 + 0x48c) = 0;
      }
    }
    break;
  case 3:
    local_88 = (void *)((int)param_1 + 0x154);
    bVar2 = FUN_004235c0(local_88,10);
    if (bVar2) {
      FUN_0052cc30(local_8,0x28);
      FUN_00479040((void *)((int)local_8 + 0x390));
      FUN_004117a0((void *)((int)local_8 + 0x390),0);
      FUN_0052ced0(local_8,1);
      FUN_004d9a70(0,0x575098);
      FUN_004d9b50(0,0);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  return 0;
}

