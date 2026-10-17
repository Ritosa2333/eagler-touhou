/* Entry: 0x0049f3d0; symbol: FUN_0049f3d0; body bytes: 4107 */

void __fastcall FUN_0049f3d0(uint *param_1)

{
  int iVar1;
  uint *puVar2;
  char *pcVar3;
  byte *pbVar4;
  void *pvVar5;
  undefined4 *puVar6;
  uint uVar7;
  int *unaff_FS_OFFSET;
  undefined1 auStack_200 [12];
  undefined4 uStack_1f4;
  char cVar8;
  undefined1 local_1d0 [44];
  undefined1 local_1a4 [36];
  undefined1 *local_180;
  undefined1 *local_17c;
  undefined4 local_178;
  undefined4 local_174;
  undefined4 local_170;
  undefined4 local_16c;
  uint *local_168;
  uint *local_164;
  uint *local_160;
  uint *local_15c;
  default_delete<class_std::_Facet_base> *local_158;
  uint *local_154;
  uint *local_150;
  undefined4 local_14c;
  uint *local_148;
  undefined4 local_144;
  int local_140;
  uint *local_13c;
  uint *local_138;
  undefined4 local_134;
  uint *local_130;
  undefined4 local_12c;
  int local_128;
  uint *local_124;
  uint *local_120;
  undefined4 local_11c;
  uint *local_118;
  undefined4 local_114;
  int local_110;
  uint *local_10c;
  uint *local_108;
  undefined4 local_104;
  uint *local_100;
  undefined4 local_fc;
  int local_f8;
  uint *local_f4;
  uint *local_f0;
  uint *local_ec;
  uint *local_e8;
  uint *local_e4;
  void *local_e0;
  void *local_dc;
  void *local_d8;
  uint *local_d4;
  uint *local_d0;
  void *local_cc;
  void *local_c8;
  void *local_c4;
  uint *local_c0;
  uint *local_bc;
  uint *local_b8;
  undefined4 *local_b4;
  undefined4 *local_b0;
  uint *local_ac;
  undefined4 *local_a8;
  undefined4 *local_a4;
  uint *local_a0;
  uint *local_9c;
  undefined4 local_98;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_94;
  uint local_90;
  uint local_8c;
  uint local_88;
  uint local_84;
  uint local_80;
  uint local_7c;
  uint local_78;
  default_delete<class_std::_Facet_base> *local_74;
  int local_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  uint *local_5c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_58 [28];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_3c [28];
  undefined4 local_20;
  uint local_1c;
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005695bb;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_5c = param_1;
  if ((param_1[0x1f] >> 2 & 1) == 0) {
LAB_0049f414:
    iVar1 = FUN_0040ff90((int)(local_5c + 5));
    if (iVar1 < (int)(uint)*(ushort *)local_5c[0x17]) goto LAB_004a03a5;
    local_78 = (uint)*(byte *)(local_5c[0x17] + 2);
    switch(local_78) {
    default:
      goto LAB_004a03c1;
    case 3:
      FUN_0040c080(local_3c,0x1c);
      puVar2 = (uint *)FUN_004b7c90((byte *)(local_5c[0x17] + 4));
      FUN_0046a830(local_3c,puVar2);
      local_8 = 0;
      if (local_5c[0x20] == 0) {
        for (local_64 = 0; local_64 < 5; local_64 = local_64 + 1) {
          local_15c = local_5c + 0xd;
          pvVar5 = (void *)FUN_00414580(local_15c,local_64);
          FUN_004a0f90(pvVar5);
          local_164 = local_5c + 0x12;
          pvVar5 = (void *)FUN_00414580(local_164,local_64);
          FUN_004a0f90(pvVar5);
        }
      }
      local_168 = local_5c + 0xd;
      pvVar5 = (void *)FUN_00414580(local_168,local_5c[0x20]);
      FUN_004a0f90(pvVar5);
      local_9c = local_5c + 0x12;
      pvVar5 = (void *)FUN_00414580(local_9c,local_5c[0x20]);
      FUN_004a0f90(pvVar5);
      pcVar3 = (char *)FUN_0049f1c0(local_3c,0);
      if (*pcVar3 == '|') {
        pbVar4 = (byte *)FUN_0049f1c0(local_3c,1);
        local_18 = FUN_005510cc(pbVar4);
        cVar8 = ',';
        puVar2 = (uint *)FUN_0049f1c0(local_3c,1);
        local_70 = FUN_004a1070(puVar2,cVar8);
        local_1c = FUN_005510cc((byte *)(local_70 + 1));
        local_70 = FUN_004a1070((uint *)(local_70 + 1),',');
        FUN_0040c080(local_58,0x1c);
        FUN_0046a830(local_58,(uint *)(local_70 + 1));
        local_8._0_1_ = 1;
        local_a0 = local_5c + 0x12;
        local_17c = auStack_200;
        puVar6 = (undefined4 *)FUN_00414580(local_a0,local_5c[0x20]);
        iVar1 = FUN_0044ced0(puVar6);
        local_a8 = FUN_0049eb40(local_1d0,local_5c,local_58,&local_18,&local_1c,iVar1);
        local_8._0_1_ = 2;
        local_a4 = local_a8;
        FUN_0049e370(auStack_200,local_a8);
        FUN_00470180(DAT_005c0698);
        local_8._0_1_ = 1;
        FUN_0046ade0((int)local_1d0);
        local_8 = (uint)local_8._1_3_ << 8;
        FUN_00449240(local_58);
      }
      else {
        local_ac = local_5c + 0xd;
        local_180 = auStack_200;
        puVar6 = (undefined4 *)FUN_00414580(local_ac,local_5c[0x20]);
        iVar1 = FUN_0044ced0(puVar6);
        local_b4 = FUN_00486b70(local_1a4,local_5c,local_3c,iVar1);
        local_8._0_1_ = 3;
        local_b0 = local_b4;
        FUN_0049e430(auStack_200,local_b4);
        FUN_00470180(DAT_005c0698);
        local_8 = (uint)local_8._1_3_ << 8;
        FUN_0046ade0((int)local_1a4);
        local_5c[0x20] = local_5c[0x20] + 1;
        if (4 < (int)local_5c[0x20]) {
          local_5c[0x20] = 0;
        }
      }
      local_8 = 0xffffffff;
      FUN_00449240(local_3c);
      break;
    case 4:
      for (local_68 = 0; local_68 < 5; local_68 = local_68 + 1) {
        local_b8 = local_5c + 0xd;
        pvVar5 = (void *)FUN_00414580(local_b8,local_68);
        FUN_00486350(pvVar5);
        local_bc = local_5c + 0x12;
        pvVar5 = (void *)FUN_00414580(local_bc,local_68);
        FUN_00486350(pvVar5);
      }
      break;
    case 5:
      iVar1 = FUN_0040ff90((int)(local_5c + 9));
      if (iVar1 < 1) {
        FUN_00423520(local_5c + 9,*(int *)(local_5c[0x17] + 4));
      }
      local_c0 = local_5c + 9;
      FUN_00429420(local_c0);
      if (*(int *)(local_5c[0x17] + 4) < 0) {
        FUN_00423520(local_5c + 9,999);
      }
      if (DAT_005b889c == (void *)0x0) {
        local_7c = 0;
      }
      else {
        local_c4 = DAT_005b889c;
        local_7c = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if ((local_7c == 0) && (iVar1 = FUN_0040ff90((int)(local_5c + 9)), 0 < iVar1)) {
        uVar7 = FUN_004a0e90((int)DAT_005c49e8);
        if (uVar7 != 0) goto LAB_004a03c1;
        if (DAT_005b889c == (void *)0x0) {
          local_80 = 0;
        }
        else {
          local_c8 = DAT_005b889c;
          local_80 = FUN_004495a0(DAT_005b889c,0x200);
        }
        if (local_80 == 0) {
          if (DAT_005b889c == (void *)0x0) {
            local_84 = 0;
          }
          else {
            local_cc = DAT_005b889c;
            local_84 = FUN_004a0a20(DAT_005b889c,0);
          }
          if (local_84 < 0x14) goto LAB_004a03c1;
        }
        local_d0 = local_5c + 9;
        iVar1 = FUN_00472040(local_d0,6);
        if (iVar1 != 0) goto LAB_004a03c1;
        FUN_00423520(local_5c + 9,0);
      }
      else {
        FUN_00426d70(&DAT_005ba830,0,0);
        FUN_00423520(local_5c + 9,0);
      }
      break;
    case 6:
      iVar1 = FUN_0040ff90((int)(local_5c + 9));
      if (iVar1 < 1) {
        FUN_00423520(local_5c + 9,*(int *)(local_5c[0x17] + 4));
      }
      local_d4 = local_5c + 9;
      FUN_00429420(local_d4);
      if (DAT_005b889c == (void *)0x0) {
        local_88 = 0;
      }
      else {
        local_d8 = DAT_005b889c;
        local_88 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if ((local_88 == 0) && (iVar1 = FUN_0040ff90((int)(local_5c + 9)), 0 < iVar1)) {
        uVar7 = FUN_004a0e90((int)DAT_005c49e8);
        if (uVar7 != 0) goto LAB_004a03c1;
        if (DAT_005b889c == (void *)0x0) {
          local_8c = 0;
        }
        else {
          local_dc = DAT_005b889c;
          local_8c = FUN_004495a0(DAT_005b889c,0x200);
        }
        if (local_8c == 0) {
          if (DAT_005b889c == (void *)0x0) {
            local_90 = 0;
          }
          else {
            local_e0 = DAT_005b889c;
            local_90 = FUN_004a0a20(DAT_005b889c,0);
          }
          if (local_90 < 0x14) goto LAB_004a03c1;
        }
        local_e4 = local_5c + 9;
        iVar1 = FUN_00472040(local_e4,6);
        if (iVar1 != 0) goto LAB_004a03c1;
        FUN_00423520(local_5c + 9,0);
        local_5c[0x20] = 0;
        DAT_005ba518 = 0;
      }
      else {
        FUN_00426d70(&DAT_005ba830,0,0);
        FUN_00423520(local_5c + 9,0);
        local_5c[0x20] = 0;
        DAT_005ba518 = 0;
      }
      break;
    case 7:
      FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
      FUN_0044c430(DAT_005c0028,*(int *)(local_5c[0x17] + 4) + 0xf);
      local_5c[0x1f] = local_5c[0x1f] | 4;
      local_5c[0x1e] = local_5c[0x17] + 8;
      local_5c[0x3b] = *(uint *)(local_5c[0x17] + 4);
      local_ec = local_5c + 0x37;
      local_e8 = local_5c;
      FUN_0040b1d0(FUN_004a0a00,&local_e8);
      local_5c[0x17] = local_5c[0x17] + 4 + (uint)*(byte *)(local_5c[0x17] + 3);
      goto LAB_004a03c1;
    case 8:
      local_f0 = local_5c + 0x27;
      puVar6 = (undefined4 *)FUN_00414580(local_f0,*(int *)(local_5c[0x17] + 4));
      FUN_0044fcd0(puVar6);
      local_f4 = local_5c + 0x23;
      puVar6 = (undefined4 *)FUN_00414580(local_f4,*(int *)(local_5c[0x17] + 8));
      local_fc = *puVar6;
      local_f8 = *(int *)(local_5c[0x17] + 0xc);
      puVar6 = FUN_00450c70(&local_16c,(uint *)0x0,local_f8,-1,(undefined4 *)0x0);
      local_104 = *puVar6;
      local_100 = local_5c + 0x27;
      puVar6 = (undefined4 *)FUN_00414580(local_100,*(int *)(local_5c[0x17] + 4));
      *puVar6 = local_104;
      break;
    case 9:
      local_5c[0x21] = *(uint *)(local_5c[0x17] + 4);
      local_5c[0x22] = *(uint *)(local_5c[0x17] + 8);
      break;
    case 10:
      FUN_004d9a70(0,local_5c[0x17] + 4);
      iVar1 = _strcmp((char *)(local_5c[0x17] + 4),"bgm/th20_15");
      if (iVar1 == 0) {
        FUN_004d9b50(0,0xf);
      }
      else {
        iVar1 = _strcmp((char *)(local_5c[0x17] + 4),"bgm/th20_14");
        if (iVar1 == 0) {
          FUN_004d9b50(0,0x10);
        }
        else {
          FUN_004d9b50(0,0x11);
        }
      }
      break;
    case 0xb:
      FUN_004d99d0(DAT_0056c8d8);
      local_5c[0x1f] = local_5c[0x1f] & 0xfffffffe;
      break;
    case 0xc:
      uVar7 = FUN_004a0f10((int)DAT_005c49e8);
      if (uVar7 != 0) goto LAB_004a03c1;
      FUN_0052f900(*local_5c);
      for (local_6c = 0; local_6c < 5; local_6c = local_6c + 1) {
        local_150 = local_5c + 0xd;
        puVar6 = (undefined4 *)FUN_00414580(local_150,local_6c);
        FUN_0044fcd0(puVar6);
        local_154 = local_5c + 0x12;
        puVar6 = (undefined4 *)FUN_00414580(local_154,local_6c);
        FUN_0044fcd0(puVar6);
      }
      local_94 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)(local_5c + 0x27);
      local_74 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first(local_94);
      local_158 = (default_delete<class_std::_Facet_base> *)FUN_00497c00((int)local_94);
      for (; local_74 != local_158; local_74 = local_74 + 4) {
        local_20 = *(undefined4 *)local_74;
        FUN_0044fcd0(&local_20);
      }
      if ((*local_5c != 0x10) && (*local_5c != 0x11)) {
        local_98 = FUN_004640a0(0x5ba568);
        switch(local_98) {
        default:
          local_60 = FUN_004a0f30(DAT_005c49e8,(int)PTR_s_staff1_msg_005afce8);
          break;
        case 1:
          local_60 = FUN_004a0f30(DAT_005c49e8,(int)PTR_s_staff2_msg_005afcec);
          goto joined_r0x004a01dc;
        case 2:
          local_60 = FUN_004a0f30(DAT_005c49e8,(int)PTR_s_staff3_msg_005afcf0);
          break;
        case 3:
          local_60 = FUN_004a0f30(DAT_005c49e8,(int)PTR_s_staff4_msg_005afcf4);
        }
        if (local_60 != 0) goto LAB_004a02c6;
        goto LAB_004a03c1;
      }
      local_60 = FUN_004a0f30(DAT_005c49e8,(int)PTR_s_staff5_msg_005afcf8);
joined_r0x004a01dc:
      if (local_60 == 0) goto LAB_004a03c1;
LAB_004a02c6:
      local_5c[0x17] = *(int *)(local_60 + 4) + local_60;
      FUN_00423520(local_5c + 1,0);
      FUN_00423520(local_5c + 5,0);
      FUN_00423520(local_5c + 9,0);
      local_5c[0x21] = 0xffffff;
      local_5c[0x1f] = local_5c[0x1f] | 2;
      goto LAB_0049f414;
    case 0xd:
      uStack_1f4 = 0x4a0352;
      FUN_004250c0(0,*(undefined4 *)(local_5c[0x17] + 4),0,0,0,0x6d);
      break;
    case 0xe:
      uStack_1f4 = 0x4a0379;
      FUN_004250c0(5,*(undefined4 *)(local_5c[0x17] + 4),0,0,0,0x6d);
      break;
    case 0xf:
      iVar1 = FUN_004640a0(0x5ba568);
      if (iVar1 == 1) {
        local_108 = local_5c + 0x27;
        puVar6 = (undefined4 *)FUN_00414580(local_108,*(int *)(local_5c[0x17] + 4));
        FUN_0044fcd0(puVar6);
        local_10c = local_5c + 0x23;
        puVar6 = (undefined4 *)FUN_00414580(local_10c,*(int *)(local_5c[0x17] + 8));
        local_114 = *puVar6;
        local_110 = *(int *)(local_5c[0x17] + 0xc);
        puVar6 = FUN_00450c70(&local_170,(uint *)0x0,local_110,-1,(undefined4 *)0x0);
        local_11c = *puVar6;
        local_118 = local_5c + 0x27;
        puVar6 = (undefined4 *)FUN_00414580(local_118,*(int *)(local_5c[0x17] + 4));
        *puVar6 = local_11c;
      }
      break;
    case 0x10:
      iVar1 = FUN_004640a0(0x5ba568);
      if (iVar1 == 2) {
        local_120 = local_5c + 0x27;
        puVar6 = (undefined4 *)FUN_00414580(local_120,*(int *)(local_5c[0x17] + 4));
        FUN_0044fcd0(puVar6);
        local_124 = local_5c + 0x23;
        puVar6 = (undefined4 *)FUN_00414580(local_124,*(int *)(local_5c[0x17] + 8));
        local_12c = *puVar6;
        local_128 = *(int *)(local_5c[0x17] + 0xc);
        puVar6 = FUN_00450c70(&local_174,(uint *)0x0,local_128,-1,(undefined4 *)0x0);
        local_134 = *puVar6;
        local_130 = local_5c + 0x27;
        puVar6 = (undefined4 *)FUN_00414580(local_130,*(int *)(local_5c[0x17] + 4));
        *puVar6 = local_134;
      }
      break;
    case 0x11:
      iVar1 = FUN_004640a0(0x5ba568);
      if (iVar1 == 3) {
        local_138 = local_5c + 0x27;
        puVar6 = (undefined4 *)FUN_00414580(local_138,*(int *)(local_5c[0x17] + 4));
        FUN_0044fcd0(puVar6);
        local_13c = local_5c + 0x23;
        puVar6 = (undefined4 *)FUN_00414580(local_13c,*(int *)(local_5c[0x17] + 8));
        local_144 = *puVar6;
        local_140 = *(int *)(local_5c[0x17] + 0xc);
        puVar6 = FUN_00450c70(&local_178,(uint *)0x0,local_140,-1,(undefined4 *)0x0);
        local_14c = *puVar6;
        local_148 = local_5c + 0x27;
        puVar6 = (undefined4 *)FUN_00414580(local_148,*(int *)(local_5c[0x17] + 4));
        *puVar6 = local_14c;
      }
    }
    local_5c[0x17] = local_5c[0x17] + 4 + (uint)*(byte *)(local_5c[0x17] + 3);
    goto LAB_0049f414;
  }
LAB_004a03c1:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
LAB_004a03a5:
  local_160 = local_5c + 5;
  FUN_00423540(local_160);
  goto LAB_004a03c1;
}

