/* Entry: 0x004decf0; symbol: FUN_004decf0; body bytes: 2374 */

void __fastcall FUN_004decf0(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined4 *puVar2;
  int iVar3;
  undefined1 auStack_170 [16];
  undefined4 uStack_160;
  undefined8 local_144;
  undefined8 local_13c;
  undefined8 local_134;
  undefined8 local_12c;
  undefined8 local_124;
  undefined1 *local_11c;
  undefined4 local_118;
  undefined1 *local_114;
  undefined1 *local_110;
  undefined1 *local_10c;
  undefined1 *local_108;
  undefined4 local_104;
  undefined4 *local_100;
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
  void *local_d4;
  undefined4 *local_d0;
  int local_cc;
  undefined4 local_c8;
  void *local_c4;
  int local_c0;
  undefined4 local_bc;
  void *local_b8;
  int local_b4;
  undefined4 local_b0;
  void *local_ac;
  int local_a8;
  undefined4 local_a4;
  int local_a0;
  void *local_9c;
  void *local_98;
  int local_94;
  void *local_90;
  void *local_8c;
  void *local_88;
  int local_84;
  undefined4 local_80;
  int local_7c;
  void *local_78;
  void *local_74;
  int local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  int local_60;
  undefined4 local_5c;
  int local_58;
  void *local_54;
  void *local_50;
  int local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  uint local_34;
  uint local_30;
  int *local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined1 local_1d;
  undefined1 local_1c;
  undefined1 local_1b;
  undefined1 local_1a;
  undefined1 local_19;
  void *local_18;
  undefined1 local_14 [12];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = param_1;
  FUN_00422dd0(local_14,*(undefined4 *)((int)param_1 + 0x94),0,0);
  local_24 = *(undefined4 *)((int)local_18 + 0x10);
  switch(local_24) {
  case 0:
    *(undefined4 *)((int)local_18 + 0x10) = 1;
    break;
  case 1:
    local_28 = *(undefined4 *)((int)local_18 + 0xa8);
    switch(local_28) {
    case 0:
      local_e8 = (void *)((int)local_18 + 0x24);
      FUN_0049c4e0(local_e8,99);
      FUN_004bed50((void *)((int)local_18 + 0x24),*(int *)((int)local_18 + 0x98));
      local_38 = (void *)((int)local_18 + 0x24);
      FUN_004bfbe0(local_38,1);
      local_3c = *(void **)((int)local_18 + 0x9c);
      puVar2 = (undefined4 *)FUN_004bfae0(local_3c,1);
      FUN_00449d80(puVar2);
      *(undefined4 *)((int)local_18 + 0xa8) = 1;
      FUN_00426d70(&DAT_005ba830,0x53,0);
LAB_004dedf8:
      *(undefined4 *)((int)local_18 + 0xa8) = 4;
      FUN_004dfb30(local_18,0);
      FUN_004dfb30(local_18,4);
      FUN_004dfb30(local_18,5);
      FUN_004dfb30(local_18,6);
      local_40 = (void *)((int)local_18 + 0x24);
      bVar1 = FUN_004dec80(local_40,1);
      if (CONCAT31(extraout_var,bVar1) == 0) {
LAB_004def1b:
        local_64 = (void *)((int)local_18 + 0x24);
        bVar1 = FUN_004dec80(local_64,9);
        if (CONCAT31(extraout_var_01,bVar1) != 0) {
          local_68 = (void *)((int)local_18 + 0x24);
          bVar1 = FUN_004dec50(local_68,0x10);
          if (CONCAT31(extraout_var_02,bVar1) != 0) {
            local_74 = (void *)FUN_0051b960();
            local_6c = (void *)((int)local_18 + 0x24);
            local_70 = FUN_004dec30(local_6c,9);
            local_7c = FUN_004dfad0(local_74,local_70);
            iVar3 = FUN_0040c300((undefined4 *)((int)local_18 + 0x24));
            local_80 = FUN_0044baf0((int)(&DAT_005c5b40 + iVar3 * 0x38));
            local_78 = (void *)((int)local_18 + 0x70);
            puVar2 = (undefined4 *)FUN_00414580(local_78,5);
            local_84 = FUN_0044ced0(puVar2);
            local_108 = &stack0xfffffe8c;
            FUN_004de670(&stack0xfffffe8c,&local_1a);
            FUN_0044add0(DAT_005c0028,&local_12c,local_84,0xffd080,0,7,0);
            goto LAB_004df1be;
          }
        }
        local_88 = (void *)((int)local_18 + 0x24);
        bVar1 = FUN_004dec80(local_88,0x11);
        if (CONCAT31(extraout_var_03,bVar1) != 0) {
          local_8c = (void *)((int)local_18 + 0x24);
          bVar1 = FUN_004dec50(local_8c,0x18);
          if (CONCAT31(extraout_var_04,bVar1) != 0) {
            local_98 = (void *)FUN_0051b960();
            local_90 = (void *)((int)local_18 + 0x24);
            local_94 = FUN_004dec30(local_90,0x11);
            local_a0 = FUN_004dfad0(local_98,local_94);
            iVar3 = FUN_0040c300((undefined4 *)((int)local_18 + 0x24));
            local_a4 = FUN_0044baf0((int)(&DAT_005c5b40 + iVar3 * 0x38));
            local_9c = (void *)((int)local_18 + 0x70);
            puVar2 = (undefined4 *)FUN_00414580(local_9c,5);
            local_a8 = FUN_0044ced0(puVar2);
            local_11c = &stack0xfffffe8c;
            FUN_004de6e0(&stack0xfffffe8c,&local_1b);
            FUN_0044add0(DAT_005c0028,&local_134,local_a8,0xffd080,0,7,0);
            goto LAB_004df1be;
          }
        }
        iVar3 = FUN_0040c300((undefined4 *)((int)local_18 + 0x24));
        local_b0 = FUN_0044baf0((int)(&DAT_005c5b40 + iVar3 * 0x38));
        local_ac = (void *)((int)local_18 + 0x70);
        puVar2 = (undefined4 *)FUN_00414580(local_ac,5);
        local_b4 = FUN_0044ced0(puVar2);
        local_110 = auStack_170;
        FUN_004de750(auStack_170,&local_1c);
        FUN_0044add0(DAT_005c0028,&local_13c,local_b4,0xffd080,0,7,0);
      }
      else {
        local_44 = (void *)((int)local_18 + 0x24);
        bVar1 = FUN_004dec50(local_44,8);
        if (CONCAT31(extraout_var_00,bVar1) == 0) goto LAB_004def1b;
        local_50 = (void *)FUN_0051b960();
        local_48 = (void *)((int)local_18 + 0x24);
        local_4c = FUN_004dec30(local_48,1);
        local_58 = FUN_004dfad0(local_50,local_4c);
        iVar3 = FUN_0040c300((undefined4 *)((int)local_18 + 0x24));
        local_5c = FUN_0044baf0((int)(&DAT_005c5b40 + iVar3 * 0x38));
        local_54 = (void *)((int)local_18 + 0x70);
        puVar2 = (undefined4 *)FUN_00414580(local_54,5);
        local_60 = FUN_0044ced0(puVar2);
        local_10c = &stack0xfffffe8c;
        FUN_004de600(&stack0xfffffe8c,&local_19);
        FUN_0044add0(DAT_005c0028,&local_124,local_60,0xffd080,0,7,0);
      }
LAB_004df1be:
      iVar3 = FUN_0040c300((undefined4 *)((int)local_18 + 0x24));
      local_bc = FUN_0044baf0((int)(&DAT_005c5b5c + iVar3 * 0x38));
      local_b8 = (void *)((int)local_18 + 0x70);
      puVar2 = (undefined4 *)FUN_00414580(local_b8,6);
      local_c0 = FUN_0044ced0(puVar2);
      local_114 = auStack_170;
      FUN_004de7c0(auStack_170,&local_1d);
      FUN_0044add0(DAT_005c0028,&local_144,local_c0,0xffd080,0,8,0);
      FUN_00423520((void *)((int)local_18 + 0x14),0);
      FUN_0040c300((undefined4 *)((int)local_18 + 0x24));
      FUN_004bee40((wchar_t *)((int)local_18 + 0xac),(wchar_t *)"notice_%.2d.png");
      break;
    case 3:
      local_c8 = *(undefined4 *)((int)local_18 + 300);
      local_cc = *(int *)((int)local_18 + 0xa4);
      local_c4 = *(void **)((int)local_18 + 0x9c);
      local_d0 = (undefined4 *)FUN_004bfae0(local_c4,1);
      uStack_160 = 0x4df2f0;
      FUN_0044dc20(local_d0,local_cc,local_c8,1,0,0);
      if (*(int *)((int)local_18 + 0xa4) != 0) {
        FUN_0041f670(*(LPVOID *)((int)local_18 + 0xa4));
        *(undefined4 *)((int)local_18 + 0xa4) = 0;
      }
      *(undefined4 *)((int)local_18 + 0xa4) = 0;
      local_d4 = *(void **)((int)local_18 + 0x9c);
      local_2c = (int *)FUN_004bfb00(local_d4,1);
      (**(code **)(*local_2c + 0x24))();
      FUN_004dfb30(local_18,0);
      FUN_004dfb30(local_18,4);
      *(undefined4 *)((int)local_18 + 0xa8) = 4;
      FUN_00423520((void *)((int)local_18 + 0x14),0);
    case 4:
      local_d8 = (void *)((int)local_18 + 0x14);
      bVar1 = FUN_00423560(local_d8,0x14);
      if (!bVar1) {
        if (DAT_005b88a4 == (void *)0x0) {
          local_30 = 0;
        }
        else {
          local_dc = DAT_005b88a4;
          local_30 = FUN_00419c00(DAT_005b88a4,0x80001);
        }
        if (local_30 != 0) {
          *(undefined4 *)((int)local_18 + 0x10) = 2;
          *(undefined4 *)((int)local_18 + 0xa8) = 0;
          FUN_00423520((void *)((int)local_18 + 0x14),0);
          FUN_00426d70(&DAT_005ba830,7,0);
          local_e0 = (void *)((int)local_18 + 0x24);
          FUN_004bf020(local_e0);
          FUN_004dfaf0(local_18,4);
          FUN_004dfaf0(local_18,0);
        }
      }
      break;
    case 5:
      local_e4 = (void *)((int)local_18 + 0x14);
      bVar1 = FUN_004235c0(local_e4,0x14);
      if (bVar1) goto LAB_004dedf8;
      break;
    case 6:
      local_104 = *(undefined4 *)((int)local_18 + 0xa0);
      puVar2 = FUN_00450c70(&local_118,(uint *)0x0,1,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_18 + 0x8c) = *puVar2;
      *(undefined4 *)((int)local_18 + 0xa8) = 7;
      FUN_00423520((void *)((int)local_18 + 0x14),0);
    case 7:
      local_ec = (void *)((int)local_18 + 0x14);
      bVar1 = FUN_00423560(local_ec,0xf0);
      if (!bVar1) {
        if (DAT_005b88a4 == (void *)0x0) {
          local_34 = 0;
        }
        else {
          local_f0 = DAT_005b88a4;
          local_34 = FUN_00419c00(DAT_005b88a4,0x80001);
        }
        if ((local_34 != 0) &&
           (iVar3 = FUN_0040c300((undefined4 *)((int)local_18 + 0x24)), 0 < iVar3)) {
          *(undefined4 *)((int)local_18 + 0x10) = 3;
          *(undefined4 *)((int)local_18 + 0xa8) = 0;
          FUN_00423520((void *)((int)local_18 + 0x14),0);
          FUN_00426d70(&DAT_005ba830,7,0);
          local_f4 = (void *)((int)local_18 + 0x24);
          FUN_004bf020(local_f4);
          FUN_00479040((void *)((int)local_18 + 0x8c));
        }
      }
    }
    break;
  case 2:
    local_f8 = (void *)((int)local_18 + 0x14);
    bVar1 = FUN_004235c0(local_f8,0x1e);
    if (bVar1) {
      *(undefined4 *)((int)local_18 + 0x90) = 1;
    }
    break;
  case 3:
    local_fc = (void *)((int)local_18 + 0x14);
    bVar1 = FUN_004235c0(local_fc,0xf0);
    if (bVar1) {
      *(undefined4 *)((int)local_18 + 0x90) = 1;
    }
  }
  local_100 = (undefined4 *)((int)local_18 + 0x14);
  FUN_00423540(local_100);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

