/* Entry: 0x00526a90; symbol: FUN_00526a90; body bytes: 3394 */

void __fastcall FUN_00526a90(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 uVar2;
  size_t sVar3;
  int iVar4;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined4 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  uint local_154;
  void *local_150;
  void *local_14c;
  void *local_148;
  undefined *local_144;
  void *local_140;
  void *local_13c;
  void *local_138;
  void *local_134;
  void *local_130;
  int local_12c;
  void *local_128;
  int local_124;
  undefined *local_120;
  void *local_11c;
  void *local_118;
  void *local_114;
  void *local_110;
  void *local_10c;
  void *local_108;
  void *local_104;
  void *local_100;
  void *local_fc;
  void *local_f8;
  void *local_f4;
  void *local_f0;
  void *local_ec;
  void *local_e8;
  void *local_e4;
  void *local_e0;
  void *local_dc;
  void *local_d8;
  undefined4 *local_d4;
  void *local_d0;
  uint local_cc;
  uint local_c8;
  int local_c4;
  int local_c0;
  int local_bc;
  int local_b8;
  uint local_b4;
  uint local_b0;
  int local_ac;
  int local_a8;
  undefined4 local_a4;
  int local_a0;
  int local_9c;
  int local_98;
  size_t local_94;
  undefined1 local_8e;
  undefined1 local_8d;
  void *local_8c;
  wchar_t local_88 [32];
  wchar_t local_48 [32];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_a4 = *(undefined4 *)((int)param_1 + 0x20);
  local_8c = param_1;
  switch(local_a4) {
  case 0:
    local_128 = (void *)((int)param_1 + 0x24);
    FUN_0049c4e0(local_128,0x19);
    FUN_004bfbe0((void *)((int)local_8c + 0x24),1);
    FUN_004bed50((void *)((int)local_8c + 0x24),0);
    FUN_004be360(&DAT_005ba568,8);
    for (local_98 = 1; local_98 < 0x1a; local_98 = local_98 + 1) {
      FID_conflict__swprintf(local_88,(wchar_t *)0x40,"th20_%.2d.rpy",local_98);
      local_d4 = FUN_0050a8e0((undefined1 (*) [16])local_88);
      local_d0 = (void *)((int)local_8c + 0x5740);
      puVar6 = (undefined4 *)FUN_00414580(local_d0,local_98 + -1);
      *puVar6 = local_d4;
    }
    bVar1 = FUN_0052c750(local_8c,0);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_0052cf80(local_8c,0);
      FUN_0052cc90(local_8c,0);
    }
    FUN_0052cf80(local_8c,0x2a);
    FUN_0052ce50(local_8c,1);
  case 1:
    local_d8 = (void *)((int)local_8c + 0x154);
    bVar1 = FUN_00461070(local_d8,6);
    if (bVar1) {
      FUN_0052ce50(local_8c,2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_a8 = 0;
    }
    else {
      local_dc = DAT_005b889c;
      local_a8 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_a8 != 0) {
      local_e0 = (void *)((int)local_8c + 0x24);
      FUN_004bf040(local_e0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_ac = 0;
    }
    else {
      local_e4 = DAT_005b889c;
      local_ac = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_ac != 0) {
      local_e8 = (void *)((int)local_8c + 0x24);
      FUN_004bf020(local_e8);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8c + 0x24));
    if (CONCAT31(extraout_var_00,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_b0 = 0;
    }
    else {
      local_ec = DAT_005b889c;
      local_b0 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_b0 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_b4 = 0;
      }
      else {
        local_f0 = DAT_005b889c;
        local_b4 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_b4 != 0) {
        uVar2 = FUN_0040c300((undefined4 *)((int)local_8c + 0x24));
        *(undefined4 *)((int)local_8c + 0x5738) = uVar2;
        FUN_004bed50((void *)((int)local_8c + 0x56e8),0);
        local_f4 = (void *)((int)local_8c + 0x56e8);
        sVar3 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
        FUN_0049c4e0(local_f4,sVar3);
        local_f8 = (void *)((int)local_8c + 0x56e8);
        FUN_004bfbe0(local_f8,1);
        FUN_00508310(DAT_005c60fc,1);
        iVar4 = FUN_004640e0(DAT_005c6108);
        FUN_00548610((char *)((int)local_8c + 0x56d4),10,iVar4 + 0xc);
        *(undefined4 *)((int)local_8c + 0x56e0) = 0;
        iVar4 = _strcmp((char *)((int)local_8c + 0x56d4),"        ");
        if (iVar4 != 0) {
          local_fc = (void *)((int)local_8c + 0x56e8);
          FUN_004bf040(local_fc);
        }
        for (local_9c = 8; (0 < local_9c && (*(char *)((int)local_8c + local_9c + 0x56d3) == ' '));
            local_9c = local_9c + -1) {
        }
        *(int *)((int)local_8c + 0x56e0) = local_9c;
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_0052ce50(local_8c,3);
      }
    }
    else {
      FUN_0052ce50(local_8c,4);
      FUN_00426d70(&DAT_005ba830,9,0);
    }
    break;
  case 3:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x56e8));
    if (DAT_005b889c == (void *)0x0) {
      local_b8 = 0;
    }
    else {
      local_100 = DAT_005b889c;
      local_b8 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_b8 != 0) {
      FUN_004e2020((void *)((int)local_8c + 0x56e8),0xd);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_bc = 0;
    }
    else {
      local_104 = DAT_005b889c;
      local_bc = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_bc != 0) {
      FUN_004e2000((void *)((int)local_8c + 0x56e8),0xd);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_c0 = 0;
    }
    else {
      local_108 = DAT_005b889c;
      local_c0 = FUN_0041a280(DAT_005b889c,0x40);
    }
    if (local_c0 != 0) {
      iVar4 = FUN_0040c300((undefined4 *)((int)local_8c + 0x56e8));
      if (iVar4 % 0xd == 0) {
        FUN_004e2000((void *)((int)local_8c + 0x56e8),0xc);
      }
      else {
        local_10c = (void *)((int)local_8c + 0x56e8);
        FUN_004bf040(local_10c);
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_c4 = 0;
    }
    else {
      local_110 = DAT_005b889c;
      local_c4 = FUN_0041a280(DAT_005b889c,0x80);
    }
    if (local_c4 != 0) {
      iVar4 = FUN_0040c300((undefined4 *)((int)local_8c + 0x56e8));
      if (iVar4 % 0xd == 0xc) {
        FUN_004e2020((void *)((int)local_8c + 0x56e8),0xc);
      }
      else {
        local_114 = (void *)((int)local_8c + 0x56e8);
        FUN_004bf020(local_114);
      }
    }
    bVar1 = FUN_0045d030((int *)((int)local_8c + 0x56e8));
    if (CONCAT31(extraout_var_01,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_c8 = 0;
    }
    else {
      local_118 = DAT_005b889c;
      local_c8 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_c8 != 0) {
      local_94 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
      local_11c = (void *)((int)local_8c + 0x56e8);
      bVar1 = FUN_004e1fd0(local_11c,local_94 - 3);
      if (CONCAT31(extraout_var_02,bVar1) == 0) {
        local_130 = (void *)((int)local_8c + 0x56e8);
        bVar1 = FUN_0046b1b0(local_130,local_94 - 3);
        if (CONCAT31(extraout_var_03,bVar1) == 0) {
          local_134 = (void *)((int)local_8c + 0x56e8);
          bVar1 = FUN_0046b1b0(local_134,local_94 - 2);
          if (CONCAT31(extraout_var_04,bVar1) == 0) {
            local_138 = (void *)((int)local_8c + 0x56e8);
            bVar1 = FUN_0046b1b0(local_138,local_94 - 1);
            if (CONCAT31(extraout_var_05,bVar1) != 0) {
              FUN_00426d70(&DAT_005ba830,0x11,0);
              iVar4 = FUN_0040c300((undefined4 *)((int)local_8c + 0x24));
              FID_conflict__swprintf(local_48,(wchar_t *)0x40,"th20_%.2d.rpy",iVar4 + 1);
              local_13c = (void *)((int)local_8c + 0x5740);
              iVar4 = FUN_0040c300((undefined4 *)((int)local_8c + 0x24));
              puVar6 = (undefined4 *)FUN_00414580(local_13c,iVar4);
              FUN_004217c0((undefined4 *)*puVar6);
              FUN_00509280(DAT_005c60fc,local_48,(char *)((int)local_8c + 0x56d4),1,0);
              puVar6 = FUN_0050a8e0((undefined1 (*) [16])local_48);
              local_140 = (void *)((int)local_8c + 0x5740);
              iVar4 = FUN_0040c300((undefined4 *)((int)local_8c + 0x24));
              puVar5 = (undefined4 *)FUN_00414580(local_140,iVar4);
              *puVar5 = puVar6;
              iVar7 = (int)local_8c + 0x56d4;
              iVar8 = 10;
              iVar4 = FUN_004640e0(DAT_005c6108);
              FUN_00548610((char *)(iVar4 + 0xc),iVar8,iVar7);
              FUN_0052ce50(local_8c,2);
            }
          }
          else {
            if (*(int *)((int)local_8c + 0x56e0) == 0) break;
            *(int *)((int)local_8c + 0x56e0) = *(int *)((int)local_8c + 0x56e0) + -1;
            *(undefined1 *)((int)local_8c + *(int *)((int)local_8c + 0x56e0) + 0x56d4) = 0x20;
          }
        }
        else if (*(int *)((int)local_8c + 0x56e0) < 8) {
          *(undefined1 *)((int)local_8c + *(int *)((int)local_8c + 0x56e0) + 0x56d4) = 0x20;
          *(int *)((int)local_8c + 0x56e0) = *(int *)((int)local_8c + 0x56e0) + 1;
          if (7 < *(int *)((int)local_8c + 0x56e0)) {
            FUN_004bed50((void *)((int)local_8c + 0x56e8),local_94 - 1);
          }
        }
        else {
          *(undefined1 *)((int)local_8c + *(int *)((int)local_8c + 0x56e0) + 0x56d3) = 0x20;
        }
      }
      else if (*(int *)((int)local_8c + 0x56e0) < 8) {
        local_120 = PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c;
        local_124 = FUN_0040c300((undefined4 *)((int)local_8c + 0x56e8));
        local_8d = local_120[local_124];
        *(undefined1 *)((int)local_8c + *(int *)((int)local_8c + 0x56e0) + 0x56d4) = local_8d;
        *(int *)((int)local_8c + 0x56e0) = *(int *)((int)local_8c + 0x56e0) + 1;
        if (7 < *(int *)((int)local_8c + 0x56e0)) {
          FUN_004bed50((void *)((int)local_8c + 0x56e8),local_94 - 1);
        }
      }
      else {
        local_144 = PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c;
        local_12c = FUN_0040c300((undefined4 *)((int)local_8c + 0x56e8));
        local_8e = local_144[local_12c];
        *(undefined1 *)((int)local_8c + *(int *)((int)local_8c + 0x56e0) + 0x56d3) = local_8e;
      }
      FUN_00426d70(&DAT_005ba830,7,0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_cc = 0;
    }
    else {
      local_148 = DAT_005b889c;
      local_cc = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_cc != 0) {
      if (*(int *)((int)local_8c + 0x56e0) == 0) {
        FUN_0052ce50(local_8c,2);
      }
      else {
        FUN_00426d70(&DAT_005ba830,9,0);
        *(int *)((int)local_8c + 0x56e0) = *(int *)((int)local_8c + 0x56e0) + -1;
        *(undefined1 *)((int)local_8c + *(int *)((int)local_8c + 0x56e0) + 0x56d4) = 0x20;
      }
    }
    break;
  case 4:
    local_14c = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_14c,6);
    if (bVar1) {
      FUN_0052cc30(local_8c,0x2a);
      FUN_00479040((void *)((int)local_8c + 0x390));
      FUN_004117a0((void *)((int)local_8c + 0x390),0);
      FUN_0052ced0(local_8c,1);
      FUN_004c6010((undefined4 *)((int)local_8c + 0x24));
      FUN_004217c0(DAT_005c60fc);
      FUN_004d9a70(0,0x575098);
      FUN_004d9b50(0,0);
      for (local_a0 = 0; local_a0 < 0x19; local_a0 = local_a0 + 1) {
        local_150 = (void *)((int)local_8c + 0x5740);
        puVar6 = (undefined4 *)FUN_00414580(local_150,local_a0);
        FUN_004217c0((undefined4 *)*puVar6);
      }
      local_154 = 0;
      FUN_0052c220((void *)((int)local_8c + 0x5740),&local_154);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

