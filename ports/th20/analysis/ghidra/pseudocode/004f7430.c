/* Entry: 0x004f7430; symbol: FUN_004f7430; body bytes: 4031 */

void __fastcall FUN_004f7430(void *param_1)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  float10 fVar6;
  undefined4 uVar7;
  undefined4 local_18c;
  undefined4 local_188;
  undefined4 local_184;
  int local_180 [8];
  undefined4 local_160;
  undefined4 local_15c;
  undefined4 local_158;
  undefined4 local_154;
  void *local_150;
  void *local_14c;
  undefined4 *local_148;
  undefined4 *local_144;
  void *local_140;
  void *local_13c;
  void *local_138;
  void *local_134;
  float local_130;
  void *local_12c;
  void *local_128;
  void *local_124;
  void *local_120;
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
  undefined4 *local_f4;
  undefined4 *local_f0;
  void *local_ec;
  void *local_e8;
  float *local_e4;
  void *local_e0;
  void *local_dc;
  int local_d8;
  float *local_d4;
  float local_d0;
  float local_cc;
  void *local_c8;
  int local_c4;
  void *local_c0;
  int local_bc;
  void *local_b8;
  int local_b4;
  void *local_b0;
  int local_ac;
  void *local_a8;
  void *local_a4;
  void *local_a0;
  void *local_9c;
  void *local_98;
  void *local_94;
  void *local_90;
  void *local_8c;
  void *local_88;
  undefined4 *local_84;
  void *local_80;
  undefined4 *local_7c;
  void *local_78;
  undefined4 *local_74;
  void *local_70;
  float *local_6c;
  void *local_68;
  default_delete<class_std::_Facet_base> *local_64;
  default_delete<class_std::_Facet_base> *local_60;
  float local_5c;
  int local_58;
  uint local_54;
  uint local_50;
  float local_4c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_48;
  undefined4 local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  default_delete<class_std::_Facet_base> *local_34;
  int local_30;
  int local_2c;
  int local_28;
  void *local_24;
  float local_20 [3];
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = param_1;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)((int)param_1 + 0x14854));
  local_28 = FUN_00498fb0(DAT_005b8898,*(int *)((int)local_24 + 0x14854));
  local_44 = FUN_00411700((int)local_24);
  switch(local_44) {
  case 0:
    local_14c = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_00423590(local_14c,1);
    if (bVar2) {
      local_150 = *(void **)((int)local_24 + 0x1c);
      FUN_00438380(local_150,(void *)((int)local_24 + 0x28),*(int *)((int)local_24 + 0x22a0));
    }
    iVar4 = FUN_0040ff90((int)local_24 + 0x644);
    local_58 = (iVar4 * -0x2800) / 0x3c + 0xf000;
    iVar4 = FUN_004ff690((int)local_24);
    *(int *)(iVar4 + 4) = local_58;
    iVar4 = FUN_004ff690((int)local_24);
    local_5c = (float)*(int *)(iVar4 + 4) / DAT_0056cd98;
    iVar4 = FUN_00460850((int)local_24);
    *(float *)(iVar4 + 4) = local_5c;
    FUN_004fb290(local_24,1);
    local_48 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x20fc);
    local_34 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_48);
    local_60 = (default_delete<class_std::_Facet_base> *)FUN_004fce40((int)local_48);
    for (; local_34 != local_60; local_34 = local_34 + 8) {
      local_64 = local_34;
      puVar3 = (undefined4 *)FUN_004ff690((int)local_24);
      uVar7 = puVar3[1];
      *(undefined4 *)local_64 = *puVar3;
      *(undefined4 *)(local_64 + 4) = uVar7;
    }
    local_68 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004235c0(local_68,0x1e);
    if (bVar2) {
      local_70 = (void *)FUN_0040c300(*(undefined4 **)((int)local_24 + 0x14858));
      local_6c = (float *)FUN_00460850((int)local_24);
      FUN_0047cf60(local_70,local_6c,DAT_0056cdac,0,99999,0);
      local_78 = (void *)FUN_0041ca90(*(int *)((int)local_24 + 0x14858));
      local_74 = (undefined4 *)FUN_00460850((int)local_24);
      FUN_004caad0(local_78,local_74,DAT_0056cdac,0,0);
      local_80 = (void *)FUN_00412710(*(int *)((int)local_24 + 0x14858));
      local_7c = (undefined4 *)FUN_00460850((int)local_24);
      local_84 = FUN_004c1b80(local_80,&local_188,local_7c,DAT_00570ad0,0,1,10);
      FUN_004c1c10(local_84,&local_18c);
    }
    else {
      iVar4 = FUN_0040ff90((int)local_24 + 0x644);
      local_4c = ((float)iVar4 * DAT_0056f114) / DAT_0056f280 + DAT_0056fa28;
      local_88 = (void *)FUN_0041ca90(*(int *)((int)local_24 + 0x14858));
      FUN_004caad0(local_88,(undefined4 *)((int)local_24 + 0x638),local_4c,0,1);
      local_8c = (void *)FUN_0041ca90(*(int *)((int)local_24 + 0x14858));
      FUN_004caad0(local_8c,(undefined4 *)((int)local_24 + 0x638),local_4c / DAT_0056c8e0,0,0);
    }
    local_90 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004235c0(local_90,0x3c);
    if (!bVar2) break;
    FUN_004ff8f0((int)local_24);
    FUN_00412da0(local_24,1);
    FUN_00423520((void *)((int)local_24 + 0x644),0);
  case 1:
    local_94 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_00423590(local_94,1);
    if (bVar2) {
      local_98 = *(void **)((int)local_24 + 0x1c);
      FUN_00438380(local_98,(void *)((int)local_24 + 0x28),*(int *)((int)local_24 + 0x22a0));
    }
    uVar5 = FUN_004ab2e0(DAT_005ba828);
    if ((uVar5 == 0) && (iVar4 = FUN_00424010(*(int *)((int)local_24 + 0x14858)), iVar4 != 0)) {
      iVar4 = FUN_00424010(*(int *)((int)local_24 + 0x14858));
      cVar1 = FUN_004780a0(iVar4);
      if (cVar1 != '\0') {
        if ((local_28 < 0) || ((&DAT_005b889c)[local_28] == 0)) {
          local_50 = 0;
        }
        else {
          local_9c = (void *)(&DAT_005b889c)[local_28];
          local_50 = FUN_004b58e0(local_9c,4);
        }
        if (local_50 != 0) {
          iVar4 = FUN_00424010(*(int *)((int)local_24 + 0x14858));
          FUN_00477e30(iVar4);
        }
      }
    }
    FUN_004f9c30(local_24);
    break;
  case 2:
    goto switchD_004f748b_caseD_2;
  case 3:
    local_100 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004785e0(local_100,4);
    if (!bVar2) {
      local_104 = (void *)((int)local_24 + 0x644);
      bVar2 = FUN_004785e0(local_104,0xf);
      if (bVar2) {
        local_108 = (void *)FUN_0041ca90(*(int *)((int)local_24 + 0x14858));
        FUN_004c9490(local_108,1,0);
      }
      else {
        local_10c = (void *)((int)local_24 + 0x644);
        FUN_004785e0(local_10c,0x1e);
      }
    }
    break;
  case 4:
    local_a0 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004235c0(local_a0,*(int *)((int)local_24 + 0x2204));
    if (!bVar2) {
      iVar4 = FUN_00424010(*(int *)((int)local_24 + 0x14858));
      if (iVar4 != 0) {
        if ((local_28 < 0) || ((&DAT_005b889c)[local_28] == 0)) {
          local_54 = 0;
        }
        else {
          local_a4 = (void *)(&DAT_005b889c)[local_28];
          local_54 = FUN_004b58e0(local_a4,4);
        }
        if (local_54 != 0) {
          iVar4 = FUN_00424010(*(int *)((int)local_24 + 0x14858));
          cVar1 = FUN_004780a0(iVar4);
          if (cVar1 != '\0') {
            iVar4 = FUN_00424010(*(int *)((int)local_24 + 0x14858));
            FUN_00477e30(iVar4);
            FUN_004f86c0(local_24);
          }
        }
      }
      break;
    }
    FUN_004f8420(local_24);
switchD_004f748b_caseD_2:
    local_a8 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004785e0(local_a8,3);
    if (bVar2) {
      iVar4 = FUN_00464080(0);
      iVar4 = FUN_004b81a0(iVar4);
      if (iVar4 < 4) {
        iVar4 = FUN_00464080(0);
        iVar4 = FUN_004b81a0(iVar4);
        if (iVar4 < 3) {
          iVar4 = FUN_00464080(0);
          iVar4 = FUN_004b81a0(iVar4);
          if (iVar4 < 2) {
            local_c8 = (void *)FUN_00464080(0);
            iVar4 = FUN_00464080(0);
            iVar4 = FUN_004b81d0(iVar4);
            local_c4 = (iVar4 * 0x28) / 100;
            FUN_004e1680(local_c8,local_c4);
          }
          else {
            local_c0 = (void *)FUN_00464080(0);
            iVar4 = FUN_00464080(0);
            iVar4 = FUN_004b81d0(iVar4);
            local_bc = (iVar4 * 0x32) / 100;
            FUN_004e1680(local_c0,local_bc);
          }
        }
        else {
          local_b8 = (void *)FUN_00464080(0);
          iVar4 = FUN_00464080(0);
          iVar4 = FUN_004b81d0(iVar4);
          local_b4 = (iVar4 * 0x3c) / 100;
          FUN_004e1680(local_b8,local_b4);
        }
      }
      else {
        local_b0 = (void *)FUN_00464080(0);
        iVar4 = FUN_00464080(0);
        iVar4 = FUN_004b81d0(iVar4);
        local_ac = (iVar4 * 0x50) / 100;
        FUN_004e1680(local_b0,local_ac);
      }
      uVar7 = 0;
      iVar4 = FUN_00460850((int)local_24);
      FUN_00422dd0(local_20,0,*(float *)(iVar4 + 4) - DAT_0056ed14,uVar7);
      fVar6 = FUN_004ff3f0((int)local_24,local_20);
      local_cc = (float)fVar6;
      local_180[0] = 1;
      local_180[1] = 1;
      local_180[2] = 1;
      local_180[3] = 1;
      local_180[4] = 1;
      local_180[5] = 1;
      local_180[6] = 1;
      for (local_2c = 0; local_2c < 7; local_2c = local_2c + 1) {
        local_d0 = (((float)local_2c * DAT_0056e0f0) / DAT_0056fd48 + local_cc) -
                   (DAT_0056e0f0 * DAT_0056c8dc) / DAT_0056fd48;
        local_dc = (void *)FUN_00498fd0(0);
        local_d4 = (float *)FUN_00460850((int)local_24);
        local_d8 = local_180[local_2c];
        FUN_004c3c90(local_dc,local_d8,local_d4,0xffffffff,local_d0,DAT_0056c8d8,0,0,0xffffffff);
      }
      FUN_004faca0(local_24,-1);
    }
    local_e0 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004235c0(local_e0,0x1e);
    if (bVar2) {
      if ((*(uint *)((int)local_24 + 0x14) >> 10 & 1) != 0) {
        local_e8 = (void *)FUN_0040c300(*(undefined4 **)((int)local_24 + 0x14858));
        local_e4 = (float *)FUN_00460850((int)local_24);
        FUN_0047d100(local_e8,local_e4,DAT_0056fa30,0);
      }
      iVar4 = FUN_00464080(0);
      iVar4 = FUN_004b80b0(iVar4);
      if (iVar4 < 0) {
        local_ec = (void *)((int)local_24 + 0x644);
        bVar2 = FUN_004785e0(local_ec,0x1e);
        if (bVar2) {
          bVar2 = FUN_00488800(DAT_005c60fc);
          if (CONCAT31(extraout_var,bVar2) == 0) {
            FUN_004e5bd0(DAT_005c60bc);
          }
          local_f0 = (undefined4 *)((int)local_24 + 0x644);
          FUN_00423540(local_f0);
          break;
        }
      }
      FUN_00412da0(local_24,0);
      FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
      local_f8 = (void *)FUN_00478ec0(0);
      local_f4 = (undefined4 *)FUN_00460850((int)local_24);
      FUN_004c1b80(local_f8,&local_184,local_f4,DAT_0056cd94,DAT_0056cd90,0x1e,0x96);
      local_30 = 0;
      iVar4 = FUN_00464080(0);
      iVar4 = FUN_00477ff0(iVar4);
      if (iVar4 < 2) {
        iVar4 = FUN_00464080(0);
        iVar4 = FUN_004bd410(iVar4);
        local_30 = iVar4 + local_30;
      }
      else {
        iVar4 = FUN_00464080(0);
        iVar4 = FUN_00477ff0(iVar4);
        local_30 = iVar4 + local_30;
      }
      local_fc = (void *)FUN_00464080(0);
      FUN_004e15e0(local_fc,local_30);
      puVar3 = (undefined4 *)FUN_00460850((int)local_24);
      *(undefined4 *)((int)local_24 + 0x638) = *puVar3;
      *(undefined4 *)((int)local_24 + 0x63c) = puVar3[1];
      *(undefined4 *)((int)local_24 + 0x640) = puVar3[2];
      FUN_004ffd80(local_24,0.0,DAT_0056cda8);
      FUN_00423520((void *)((int)local_24 + 0x2050),0x118);
      FUN_00423520((void *)((int)local_24 + 0x644),0);
    }
    break;
  case 5:
    FUN_00422e10(&local_14);
    local_110 = (void *)((int)local_24 + 0x18);
    FUN_004ff710(local_110,&local_14,(float *)0x0,DAT_0056c8cc);
    iVar4 = FUN_0040ff90((int)local_24 + 0x644);
    FUN_004532c0(&local_14,&local_14,DAT_0056c8d0 - ((float)iVar4 * DAT_0056c8d0) / DAT_00570560);
    FUN_004ffa10(local_24,(int)(local_14 * DAT_0056cd98),(int)(local_10 * DAT_0056cd98));
    local_114 = (void *)((int)local_24 + 0x644);
    bVar2 = FUN_004235c0(local_114,0x28);
    if (bVar2) {
      *(undefined4 *)((int)local_24 + 0x10) = 1;
      FUN_00423520((void *)((int)local_24 + 0x644),0);
    }
    break;
  case 6:
    FUN_00423520((void *)((int)local_24 + 0x644),0);
    FUN_00422dd0(local_180 + 7,0,DAT_0056cda8,0);
    *(int *)((int)local_24 + 0x614) = local_180[7];
    *(undefined4 *)((int)local_24 + 0x618) = local_160;
    *(undefined4 *)((int)local_24 + 0x61c) = local_15c;
    FUN_0040de00(&local_158,0,0x1e0);
    *(undefined4 *)((int)local_24 + 0x620) = local_158;
    *(undefined4 *)((int)local_24 + 0x624) = local_154;
  }
  local_118 = (void *)((int)local_24 + 0x2060);
  bVar2 = FUN_00461070(local_118,0);
  if (bVar2) {
    local_11c = (void *)((int)local_24 + 0x2060);
    FUN_00429420(local_11c);
  }
  local_120 = (void *)((int)local_24 + 0x2050);
  bVar2 = FUN_00461070(local_120,0);
  if (bVar2) {
    local_124 = (void *)((int)local_24 + 0x2050);
    FUN_00429420(local_124);
    local_38 = (void *)((int)local_24 + 0x28);
    local_128 = (void *)((int)local_24 + 0x644);
    iVar4 = FUN_00478e00(local_128,3);
    if (iVar4 == 0) {
      *(byte *)((int)local_24 + 0x4c9) = *(byte *)((int)local_24 + 0x4c9) & 0xe3;
    }
    else {
      FUN_004ab610(local_38,0xff0000ff);
      *(byte *)((int)local_38 + 0x4a1) = *(byte *)((int)local_38 + 0x4a1) & 0xe3 | 4;
    }
  }
  else {
    *(byte *)((int)local_24 + 0x4c9) = *(byte *)((int)local_24 + 0x4c9) & 0xe3;
    uVar5 = FUN_004ff820((int)local_24);
    if (uVar5 == 0) {
      fVar6 = FUN_004ff610((int)local_24);
      local_130 = (float)fVar6;
      if (DAT_0056e730 < local_130) {
        local_40 = (void *)((int)local_24 + 0x28);
        local_134 = (void *)((int)local_24 + 0x644);
        iVar4 = FUN_00472040(local_134,8);
        if (iVar4 < 4) {
          FUN_004ab610(local_40,0xffffff00);
          *(byte *)((int)local_40 + 0x4a1) = *(byte *)((int)local_40 + 0x4a1) & 0xe3 | 4;
        }
      }
    }
    else {
      local_3c = (void *)((int)local_24 + 0x28);
      local_12c = (void *)((int)local_24 + 0x644);
      iVar4 = FUN_00472040(local_12c,8);
      if (iVar4 < 4) {
        FUN_004ab610(local_3c,0xffff0000);
        *(byte *)((int)local_3c + 0x4a1) = *(byte *)((int)local_3c + 0x4a1) & 0xe3 | 4;
      }
    }
  }
  *(float *)((int)local_24 + 0x20ec) = DAT_0056c8cc;
  FUN_004ffee0(local_24,(undefined4 *)&DAT_00572664);
  FUN_0042b5d0((void *)((int)local_24 + 0x28));
  uVar5 = FUN_004ff800((int)local_24);
  if (uVar5 == 0) {
    local_140 = (void *)((int)local_24 + 0x28);
    FUN_00470e50(local_140,DAT_0056c8cc,DAT_0056c8cc);
  }
  else {
    iVar4 = FUN_0041caf0((int)local_24 + 0x220c);
    if (iVar4 != 0) {
      fVar6 = FUN_0042a110((float *)((int)local_24 + 0x220c));
      *(float *)((int)local_24 + 0x2238) = (float)fVar6;
    }
    iVar4 = FUN_0041caf0((int)local_24 + 0x220c);
    if ((iVar4 == 0) || (iVar4 = FUN_0040ff90((int)local_24 + 0x644), iVar4 % 3 != 0)) {
      local_138 = (void *)((int)local_24 + 0x28);
      FUN_00470e50(local_138,*(undefined4 *)((int)local_24 + 0x2238),
                   *(undefined4 *)((int)local_24 + 0x2238));
    }
    else {
      local_13c = (void *)((int)local_24 + 0x28);
      FUN_00470e50(local_13c,DAT_0056c8cc,DAT_0056c8cc);
    }
  }
  local_144 = (undefined4 *)((int)local_24 + 0x644);
  FUN_00423540(local_144);
  local_148 = (undefined4 *)((int)local_24 + 0x654);
  FUN_00423540(local_148);
  FUN_00504210((void *)((int)local_24 + 0x22b4));
  FUN_004f7210((void *)((int)local_24 + 0x2244));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

