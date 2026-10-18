/* Entry: 0x005223e0; symbol: FUN_005223e0; body bytes: 2604 */

undefined4 __fastcall FUN_005223e0(void *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined3 extraout_var;
  size_t sVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 local_c0;
  void *local_bc;
  void *local_b8;
  int local_b4;
  int local_b0;
  int local_ac;
  int local_a8;
  void *local_a4;
  void *local_a0;
  void *local_9c;
  int local_98;
  undefined *local_94;
  int local_90;
  undefined *local_8c;
  void *local_88;
  void *local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  void *local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  int local_54;
  int local_50;
  void *local_4c;
  int local_48;
  void *local_44;
  undefined4 local_40;
  void *local_3c;
  void *local_38;
  uint local_34;
  uint local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  undefined4 local_18;
  int local_14;
  size_t local_10;
  undefined1 local_a;
  undefined1 local_9;
  void *local_8;
  
  local_18 = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_18) {
  case 0:
    local_38 = (void *)((int)param_1 + 0x24);
    FUN_0049c4e0(local_38,0x1e);
    FUN_004d9a70(0,0x572628);
    FUN_004d9b50(0,0x11);
    local_3c = (void *)((int)local_8 + 0x390);
    bVar1 = FUN_00449430(local_3c,0);
    if (bVar1) {
      local_40 = FUN_00437950(DAT_005c0698);
      puVar2 = FUN_00450c70(&local_c0,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar2;
    }
    FUN_0052cf80(local_8,0x29);
    FUN_0052ce50(local_8,1);
    local_44 = (void *)FUN_0051b960();
    FUN_0051a0b0(local_44,2);
    local_4c = (void *)FUN_0051b960();
    iVar4 = FUN_00464080(0);
    local_48 = FUN_0041cab0(iVar4);
    FUN_0052ce80(local_4c,local_48);
    FUN_0052cf80(local_8,0x48);
    iVar4 = FUN_00464080(0);
    iVar4 = FUN_00412730(iVar4);
    iVar5 = FUN_00464080(0);
    iVar5 = FUN_0041cab0(iVar5);
    FUN_0052cd10(local_8,0x48,iVar5 / 2 + 0x25 + iVar4 * 4);
    iVar4 = FUN_004640a0(0x5ba568);
    FUN_0052cf80(local_8,iVar4 + 0x40);
    bVar1 = FUN_0052c750(local_8,0);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_0052cf80(local_8,0);
      FUN_0052cc90(local_8,0);
    }
    local_50 = FUN_004b8340(0x5ba568);
    local_54 = FUN_004b7f70(0x5ba568);
    FUN_004bd460(DAT_005c6108,local_54,local_50);
    local_1c = FUN_0050f170();
    FUN_004be360(&DAT_005ba568,0);
    if (local_1c < 0) {
      FUN_004bed50((void *)((int)local_8 + 0x24),-1);
      *(undefined4 *)((int)local_8 + 0x56e4) = 1;
    }
    else {
      FUN_00423520((void *)((int)local_8 + 0x154),0);
      local_58 = (void *)((int)local_8 + 0x24);
      FUN_004bfbe0(local_58,1);
      FUN_004bed50((void *)((int)local_8 + 0x24),local_1c);
      FUN_004bed50((void *)((int)local_8 + 0x56e8),0);
      local_5c = (void *)((int)local_8 + 0x56e8);
      sVar3 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
      FUN_0049c4e0(local_5c,sVar3);
      local_60 = (void *)((int)local_8 + 0x56e8);
      FUN_004bfbe0(local_60,1);
      iVar4 = FUN_004640e0((int)DAT_005c6108);
      FUN_00548610((char *)((int)local_8 + 0x56d4),10,iVar4 + 0xc);
      iVar4 = _strcmp((char *)((int)local_8 + 0x56d4),"        ");
      if (iVar4 != 0) {
        local_64 = (void *)((int)local_8 + 0x56e8);
        FUN_004bf040(local_64);
      }
      for (local_14 = 8; (0 < local_14 && (*(char *)((int)local_8 + local_14 + 0x56d3) == ' '));
          local_14 = local_14 + -1) {
      }
      *(int *)((int)local_8 + 0x56e0) = local_14;
      *(undefined4 *)((int)local_8 + 0x56e4) = 0;
    }
  case 1:
    local_68 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_00461070(local_68,6);
    if (bVar1) {
      FUN_0052ce50(local_8,2);
    }
    break;
  case 2:
    if (*(int *)((int)param_1 + 0x56e4) == 0) {
      FUN_004bfc00((undefined4 *)((int)param_1 + 0x56e8));
      if (DAT_005b889c == (void *)0x0) {
        local_20 = 0;
      }
      else {
        local_6c = DAT_005b889c;
        local_20 = FUN_0041a280(DAT_005b889c,0x10);
      }
      if (local_20 != 0) {
        FUN_004e2020((void *)((int)local_8 + 0x56e8),0xd);
      }
      if (DAT_005b889c == (void *)0x0) {
        local_24 = 0;
      }
      else {
        local_70 = DAT_005b889c;
        local_24 = FUN_0041a280(DAT_005b889c,0x20);
      }
      if (local_24 != 0) {
        FUN_004e2000((void *)((int)local_8 + 0x56e8),0xd);
      }
      if (DAT_005b889c == (void *)0x0) {
        local_28 = 0;
      }
      else {
        local_74 = DAT_005b889c;
        local_28 = FUN_0041a280(DAT_005b889c,0x40);
      }
      if (local_28 != 0) {
        iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x56e8));
        if (iVar4 % 0xd == 0) {
          FUN_004e2000((void *)((int)local_8 + 0x56e8),0xc);
        }
        else {
          local_78 = (void *)((int)local_8 + 0x56e8);
          FUN_004bf040(local_78);
        }
      }
      if (DAT_005b889c == (void *)0x0) {
        local_2c = 0;
      }
      else {
        local_7c = DAT_005b889c;
        local_2c = FUN_0041a280(DAT_005b889c,0x80);
      }
      if (local_2c != 0) {
        iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x56e8));
        if (iVar4 % 0xd == 0xc) {
          FUN_004e2020((void *)((int)local_8 + 0x56e8),0xc);
        }
        else {
          local_80 = (void *)((int)local_8 + 0x56e8);
          FUN_004bf020(local_80);
        }
      }
      bVar1 = FUN_0045d030((int *)((int)local_8 + 0x56e8));
      if (CONCAT31(extraout_var_00,bVar1) != 0) {
        FUN_00426d70(&DAT_005ba830,10,0);
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_30 = 0;
    }
    else {
      local_84 = DAT_005b889c;
      local_30 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_30 != 0) {
      if (*(int *)((int)local_8 + 0x56e4) == 0) {
        local_10 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
        local_88 = (void *)((int)local_8 + 0x56e8);
        bVar1 = FUN_004e1fd0(local_88,local_10 - 3);
        if (CONCAT31(extraout_var_01,bVar1) == 0) {
          local_9c = (void *)((int)local_8 + 0x56e8);
          bVar1 = FUN_0046b1b0(local_9c,local_10 - 3);
          if (CONCAT31(extraout_var_02,bVar1) == 0) {
            local_a0 = (void *)((int)local_8 + 0x56e8);
            bVar1 = FUN_0046b1b0(local_a0,local_10 - 2);
            if (CONCAT31(extraout_var_03,bVar1) == 0) {
              local_a4 = (void *)((int)local_8 + 0x56e8);
              bVar1 = FUN_0046b1b0(local_a4,local_10 - 1);
              if (CONCAT31(extraout_var_04,bVar1) != 0) {
                local_a8 = FUN_0050fc50(DAT_005c6108);
                local_a8 = local_a8 + 0x18;
                local_ac = FUN_004640a0(0x5ba568);
                local_ac = local_ac * 400;
                local_b4 = local_a8 + local_ac;
                local_b0 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
                local_b0 = local_b0 * 0x28;
                FUN_00548610((char *)(local_b4 + 10 + local_b0),10,(int)local_8 + 0x56d4);
                iVar5 = (int)local_8 + 0x56d4;
                iVar6 = 10;
                iVar4 = FUN_004640e0((int)DAT_005c6108);
                FUN_00548610((char *)(iVar4 + 0xc),iVar6,iVar5);
                FUN_0052ce50(local_8,3);
              }
            }
            else {
              if (*(int *)((int)local_8 + 0x56e0) == 0) {
                return 1;
              }
              *(int *)((int)local_8 + 0x56e0) = *(int *)((int)local_8 + 0x56e0) + -1;
              *(undefined1 *)((int)local_8 + *(int *)((int)local_8 + 0x56e0) + 0x56d4) = 0x20;
            }
          }
          else if (*(int *)((int)local_8 + 0x56e0) < 8) {
            *(undefined1 *)((int)local_8 + *(int *)((int)local_8 + 0x56e0) + 0x56d4) = 0x20;
            *(int *)((int)local_8 + 0x56e0) = *(int *)((int)local_8 + 0x56e0) + 1;
            if (7 < *(int *)((int)local_8 + 0x56e0)) {
              FUN_004bed50((void *)((int)local_8 + 0x56e8),local_10 - 1);
            }
          }
          else {
            *(undefined1 *)((int)local_8 + *(int *)((int)local_8 + 0x56e0) + 0x56d3) = 0x20;
          }
        }
        else if (*(int *)((int)local_8 + 0x56e0) < 8) {
          local_8c = PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c;
          local_90 = FUN_0040c300((undefined4 *)((int)local_8 + 0x56e8));
          local_9 = local_8c[local_90];
          *(undefined1 *)((int)local_8 + *(int *)((int)local_8 + 0x56e0) + 0x56d4) = local_9;
          *(int *)((int)local_8 + 0x56e0) = *(int *)((int)local_8 + 0x56e0) + 1;
          if (7 < *(int *)((int)local_8 + 0x56e0)) {
            FUN_004bed50((void *)((int)local_8 + 0x56e8),local_10 - 1);
          }
        }
        else {
          local_94 = PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c;
          local_98 = FUN_0040c300((undefined4 *)((int)local_8 + 0x56e8));
          local_a = local_94[local_98];
          *(undefined1 *)((int)local_8 + *(int *)((int)local_8 + 0x56e0) + 0x56d3) = local_a;
        }
        FUN_00426d70(&DAT_005ba830,7,0);
      }
      else {
        FUN_0052ce50(local_8,3);
        FUN_00426d70(&DAT_005ba830,7,0);
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_34 = 0;
    }
    else {
      local_b8 = DAT_005b889c;
      local_34 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_34 != 0) {
      if (*(int *)((int)local_8 + 0x56e4) == 0) {
        if (*(int *)((int)local_8 + 0x56e0) != 0) {
          FUN_00426d70(&DAT_005ba830,9,0);
          *(int *)((int)local_8 + 0x56e0) = *(int *)((int)local_8 + 0x56e0) + -1;
          *(undefined1 *)((int)local_8 + *(int *)((int)local_8 + 0x56e0) + 0x56d4) = 0x20;
        }
      }
      else {
        FUN_0052ce50(local_8,3);
        FUN_00426d70(&DAT_005ba830,7,0);
      }
    }
    break;
  case 3:
    local_bc = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_bc,6);
    if (bVar1) {
      iVar4 = FUN_0051b960();
      FUN_00519840(iVar4);
      FUN_0052cc30(local_8,0x29);
      iVar4 = FUN_004b7f40(0x5ba568);
      FUN_0052cc30(local_8,iVar4 + 0x3c);
      iVar4 = FUN_004640a0(0x5ba568);
      FUN_0052cc30(local_8,iVar4 + 0x40);
      FUN_0052cc30(local_8,0x48);
      iVar4 = FUN_004b7fa0(0x5ba568);
      if (iVar4 == 0) {
        FUN_0052ced0(local_8,0x10);
      }
      else {
        FUN_004217c0(DAT_005c60fc);
        FUN_00479040((void *)((int)local_8 + 0x390));
        FUN_004117a0((void *)((int)local_8 + 0x390),0);
        FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
        FUN_0052ced0(local_8,1);
        FUN_004d9a70(0,0x575098);
        FUN_004d9b50(0,0);
      }
    }
  }
  return 1;
}

