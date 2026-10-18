/* Entry: 0x004c25a0; symbol: FUN_004c25a0; body bytes: 4742 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c25a0(void *param_1)

{
  bool bVar1;
  char cVar2;
  byte bVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  undefined3 extraout_var;
  float *pfVar7;
  undefined3 extraout_var_00;
  void *pvVar8;
  undefined4 *puVar9;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int *unaff_FS_OFFSET;
  float10 fVar10;
  undefined1 local_1d4 [12];
  undefined1 local_1c8 [12];
  undefined1 local_1bc [12];
  undefined1 local_1b0 [12];
  undefined1 local_1a4 [12];
  void *local_198;
  float local_194;
  int *local_190;
  undefined4 *local_18c;
  void *local_188;
  undefined4 *local_184;
  float local_180;
  float local_17c;
  float local_178;
  float local_174;
  float *local_170;
  float *local_16c;
  float local_168;
  float local_164;
  float local_160;
  void *local_15c;
  void *local_158;
  void *local_154;
  void *local_150;
  void *local_14c;
  void *local_148;
  void *local_144;
  void *local_140;
  void *local_13c;
  void *local_138;
  void *local_134;
  void *local_130;
  void *local_12c;
  void *local_128;
  void *local_124;
  void *local_120;
  void *local_11c;
  void *local_118;
  void *local_114;
  float local_110;
  float local_10c;
  float local_108;
  float *local_104;
  float *local_100;
  float local_fc;
  float local_f8;
  float local_f4;
  float *local_f0;
  void *local_ec;
  float local_e8;
  void *local_e4;
  float local_e0;
  float local_dc;
  int local_d8;
  float *local_d4;
  float local_d0;
  void *local_cc;
  float local_c8;
  float *local_c4;
  void *local_c0;
  float local_bc;
  void *local_b8;
  float local_b4;
  float local_b0;
  int local_ac;
  float *local_a8;
  float local_a4;
  float local_a0;
  float local_9c;
  float local_98;
  float *local_94;
  void *local_90;
  float local_8c;
  float local_88;
  void *local_84;
  float local_80;
  float local_7c;
  float local_78;
  float *local_74;
  void *local_70;
  float local_6c;
  void *local_68;
  float local_64;
  float local_60;
  float *local_5c;
  float *local_58;
  float *local_54;
  float *local_50;
  int *local_4c;
  int *local_48;
  void *local_44;
  int local_40;
  void *local_3c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_38;
  float local_34;
  float local_30;
  float local_28;
  float local_24;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569a4d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_3c = param_1;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)((int)param_1 + 0x49c894));
  *(undefined4 *)((int)local_3c + 0x49c870) = 0;
  *(undefined4 *)((int)local_3c + 0x49c864) = 0;
  local_44 = (void *)((int)local_3c + 0x49c818);
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_44,local_1c);
  local_8 = 0;
  local_190 = (int *)FUN_00412540();
  do {
    bVar1 = FUN_00411bb0(local_1c,local_190);
    if (!bVar1) {
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
      if (*(float *)((int)local_3c + 0x49c860) <= DAT_0056c8cc &&
          DAT_0056c8cc != *(float *)((int)local_3c + 0x49c860)) {
        FUN_004c4f70(local_3c,*(float *)((int)local_3c + 0x49c860) + DAT_0056ef28);
      }
      *(undefined4 *)((int)local_3c + 0x49c884) = 0;
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    local_38 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_0040c300(local_1c);
    piVar4 = (int *)FUN_0040f160(local_38);
    if (*(int *)(*piVar4 + 0xc24) != 0) {
      piVar4 = (int *)FUN_0040f160(local_38);
      if (*(int *)(*piVar4 + 0xc24) == 5) {
        piVar4 = (int *)FUN_0040f160(local_38);
        local_48 = (int *)(*piVar4 + 0xc34);
        *local_48 = *local_48 + -1;
        piVar4 = (int *)FUN_0040f160(local_38);
        if (*(int *)(*piVar4 + 0xc34) < 0) {
          piVar4 = (int *)FUN_0040f160(local_38);
          FUN_004c4420(*piVar4);
        }
      }
      else {
        piVar4 = (int *)FUN_0040f160(local_38);
        if (*(int *)(*piVar4 + 0xc24) == 1) {
          piVar4 = (int *)FUN_0040f160(local_38);
          if (0 < *(int *)(*piVar4 + 0xc34)) {
            piVar4 = (int *)FUN_0040f160(local_38);
            local_4c = (int *)(*piVar4 + 0xc34);
            *local_4c = *local_4c + -1;
            piVar4 = (int *)FUN_0040f160(local_38);
            if (*(int *)(*piVar4 + 0xc34) < 1) {
              piVar4 = (int *)FUN_0040f160(local_38);
              FUN_004c42c0(*piVar4);
            }
            goto LAB_004c2637;
          }
          iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          iVar5 = FUN_00411700(iVar5);
          if (iVar5 == 2) {
LAB_004c27c0:
            local_198 = (void *)FUN_00424010(*(int *)((int)local_3c + 0x49c898));
            cVar2 = FUN_004c4f00(local_198,0x3c);
            if ((cVar2 == '\0') &&
               (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) == 0)) {
              piVar4 = (int *)FUN_0040f160(local_38);
              local_68 = (void *)(*piVar4 + 0xbf0);
              fVar10 = FUN_004292e0((float *)&DAT_005aefe4);
              local_64 = (float)fVar10;
              local_70 = FUN_004292f0(local_68,local_1a4,local_64);
              local_6c = *(float *)((int)local_3c + 0x49c860);
              pfVar7 = (float *)FUN_004292f0(local_70,local_1b0,local_6c);
              local_74 = pfVar7;
              piVar4 = (int *)FUN_0040f160(local_38);
              FUN_004296e0((void *)(*piVar4 + 0xbe4),pfVar7);
              fVar10 = FUN_004292e0((float *)&DAT_005aefe4);
              local_78 = (float)fVar10;
              local_80 = local_78 * _DAT_00570558 * *(float *)((int)local_3c + 0x49c860);
              piVar4 = (int *)FUN_0040f160(local_38);
              local_50 = (float *)(*piVar4 + 0xbf4);
              local_7c = *local_50;
              *local_50 = local_7c + local_80;
              piVar4 = (int *)FUN_0040f160(local_38);
              local_84 = (void *)(*piVar4 + 0xc04);
              FUN_004235c0(local_84,0x20);
              piVar4 = (int *)FUN_0040f160(local_38);
              if (_DAT_0056e0e8 <= *(float *)(*piVar4 + 0xbf4)) {
                piVar4 = (int *)FUN_0040f160(local_38);
                *(undefined4 *)(*piVar4 + 0xbf0) = 0;
              }
              piVar4 = (int *)FUN_0040f160(local_38);
              if (DAT_0056c8d0 < *(float *)(*piVar4 + 0xbf4)) {
                piVar4 = (int *)FUN_0040f160(local_38);
                *(float *)(*piVar4 + 0xbf4) = DAT_0056c8d0;
              }
              piVar4 = (int *)FUN_0040f160(local_38);
              if (*(float *)(*piVar4 + 0xbe8) <= DAT_0056cd90 + DAT_0056d7c0 + DAT_0056d7bc) {
                piVar4 = (int *)FUN_0040f160(local_38);
                fVar10 = FUN_0047a630(*(float *)(*piVar4 + 0xbe4));
                local_88 = (float)fVar10;
                if (local_88 < DAT_0056cda4 / DAT_0056c8d0 + DAT_0056d7bc) goto LAB_004c304a;
              }
              puVar9 = (undefined4 *)FUN_0040f160(local_38);
              FUN_004c3880((void *)*puVar9);
              goto LAB_004c2637;
            }
          }
          else {
            iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
            iVar5 = FUN_00411700(iVar5);
            if (iVar5 == 4) goto LAB_004c27c0;
            iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
            iVar5 = FUN_00460850(iVar5);
            iVar6 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
            fVar10 = FUN_004b8050(iVar6);
            local_194 = (float)fVar10;
            if (local_194 < *(float *)(iVar5 + 4) || local_194 == *(float *)(iVar5 + 4))
            goto LAB_004c27c0;
          }
          iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          fVar10 = FUN_004c4940(iVar5);
          local_60 = (float)fVar10;
          piVar4 = (int *)FUN_0040f160(local_38);
          *(float *)(*piVar4 + 0xc30) = local_60;
          piVar4 = (int *)FUN_0040f160(local_38);
          *(undefined4 *)(*piVar4 + 0xc24) = 3;
LAB_004c2bf3:
          local_ac = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          piVar4 = (int *)FUN_0040f160(local_38);
          local_a8 = (float *)(*piVar4 + 0xbe4);
          fVar10 = FUN_004ff350(local_ac,local_a8);
          local_b4 = (float)fVar10;
          piVar4 = (int *)FUN_0040f160(local_38);
          local_b0 = *(float *)(*piVar4 + 0xc30);
          piVar4 = (int *)FUN_0040f160(local_38);
          local_b8 = (void *)(*piVar4 + 0xbf0);
          FUN_00439330(local_b8,(float *)local_b8,local_b4,local_b0);
          piVar4 = (int *)FUN_0040f160(local_38);
          local_c0 = (void *)(*piVar4 + 0xbf0);
          fVar10 = FUN_004292e0((float *)&DAT_005aefe4);
          local_bc = (float)fVar10;
          pfVar7 = (float *)FUN_004292f0(local_c0,local_1c8,local_bc);
          local_c4 = pfVar7;
          piVar4 = (int *)FUN_0040f160(local_38);
          FUN_004296e0((void *)(*piVar4 + 0xbe4),pfVar7);
          piVar4 = (int *)FUN_0040f160(local_38);
          if (*(float *)(*piVar4 + 0xc30) <= DAT_0056f7a8 &&
              DAT_0056f7a8 != *(float *)(*piVar4 + 0xc30)) {
            piVar4 = (int *)FUN_0040f160(local_38);
            local_58 = (float *)(*piVar4 + 0xc30);
            *local_58 = *local_58 + DAT_0056fbf8;
          }
          iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          iVar5 = FUN_00411700(iVar5);
          if (iVar5 == 4) {
            piVar4 = (int *)FUN_0040f160(local_38);
            *(undefined4 *)(*piVar4 + 0xc24) = 1;
            piVar4 = (int *)FUN_0040f160(local_38);
            *(undefined4 *)(*piVar4 + 0xbf0) = 0;
            piVar4 = (int *)FUN_0040f160(local_38);
            *(undefined4 *)(*piVar4 + 0xbf4) = 0;
          }
        }
        else {
          piVar4 = (int *)FUN_0040f160(local_38);
          if (*(int *)(*piVar4 + 0xc24) != 2) {
            piVar4 = (int *)FUN_0040f160(local_38);
            if (*(int *)(*piVar4 + 0xc24) != 3) {
              piVar4 = (int *)FUN_0040f160(local_38);
              if (*(int *)(*piVar4 + 0xc24) != 4) goto LAB_004c304a;
              iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
              iVar5 = FUN_00411700(iVar5);
              if (iVar5 == 2) {
LAB_004c2e24:
                local_cc = (void *)FUN_00424010(*(int *)((int)local_3c + 0x49c898));
                cVar2 = FUN_004c4f00(local_cc,0x3c);
                if ((cVar2 == '\0') &&
                   (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var_00,bVar1) == 0)) {
                  local_d8 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
                  piVar4 = (int *)FUN_0040f160(local_38);
                  local_d4 = (float *)(*piVar4 + 0xbe4);
                  fVar10 = FUN_004ff350(local_d8,local_d4);
                  local_e0 = (float)fVar10;
                  piVar4 = (int *)FUN_0040f160(local_38);
                  local_dc = *(float *)(*piVar4 + 0xc30);
                  piVar4 = (int *)FUN_0040f160(local_38);
                  local_e4 = (void *)(*piVar4 + 0xbf0);
                  FUN_00439330(local_e4,(float *)local_e4,local_e0,local_dc);
                  piVar4 = (int *)FUN_0040f160(local_38);
                  local_ec = (void *)(*piVar4 + 0xbf0);
                  fVar10 = FUN_004292e0((float *)&DAT_005aefe4);
                  local_e8 = (float)fVar10;
                  pfVar7 = (float *)FUN_004292f0(local_ec,local_1d4,local_e8);
                  local_f0 = pfVar7;
                  piVar4 = (int *)FUN_0040f160(local_38);
                  FUN_004296e0((void *)(*piVar4 + 0xbe4),pfVar7);
                  piVar4 = (int *)FUN_0040f160(local_38);
                  if (*(float *)(*piVar4 + 0xc30) <= DAT_0056f7a8 &&
                      DAT_0056f7a8 != *(float *)(*piVar4 + 0xc30)) {
                    piVar4 = (int *)FUN_0040f160(local_38);
                    local_5c = (float *)(*piVar4 + 0xc30);
                    *local_5c = *local_5c + DAT_0056fbf8;
                  }
                  iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
                  iVar5 = FUN_00411700(iVar5);
                  if (iVar5 == 4) {
                    piVar4 = (int *)FUN_0040f160(local_38);
                    *(undefined4 *)(*piVar4 + 0xc24) = 1;
                    piVar4 = (int *)FUN_0040f160(local_38);
                    *(undefined4 *)(*piVar4 + 0xbf0) = 0;
                    piVar4 = (int *)FUN_0040f160(local_38);
                    *(undefined4 *)(*piVar4 + 0xbf4) = 0;
                  }
                  goto LAB_004c304a;
                }
              }
              else {
                iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
                iVar5 = FUN_00411700(iVar5);
                if (iVar5 == 4) goto LAB_004c2e24;
                iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
                iVar5 = FUN_00460850(iVar5);
                iVar6 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
                fVar10 = FUN_004b8050(iVar6);
                local_c8 = (float)fVar10;
                if (local_c8 < *(float *)(iVar5 + 4) || local_c8 == *(float *)(iVar5 + 4))
                goto LAB_004c2e24;
              }
              iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
              fVar10 = FUN_004c4940(iVar5);
              local_d0 = (float)fVar10;
              piVar4 = (int *)FUN_0040f160(local_38);
              *(float *)(*piVar4 + 0xc30) = local_d0;
              piVar4 = (int *)FUN_0040f160(local_38);
              *(undefined4 *)(*piVar4 + 0xc24) = 3;
            }
            goto LAB_004c2bf3;
          }
          piVar4 = (int *)FUN_0040f160(local_38);
          local_90 = (void *)(*piVar4 + 0xbf0);
          fVar10 = FUN_004292e0((float *)&DAT_005aefe4);
          local_8c = (float)fVar10;
          pfVar7 = (float *)FUN_004292f0(local_90,local_1bc,local_8c);
          local_94 = pfVar7;
          piVar4 = (int *)FUN_0040f160(local_38);
          FUN_004296e0((void *)(*piVar4 + 0xbe4),pfVar7);
          fVar10 = FUN_004292e0((float *)&DAT_005aefe4);
          local_98 = (float)fVar10;
          local_a0 = local_98 * _DAT_00570558;
          piVar4 = (int *)FUN_0040f160(local_38);
          local_54 = (float *)(*piVar4 + 0xbf4);
          local_9c = *local_54;
          *local_54 = local_9c + local_a0;
          piVar4 = (int *)FUN_0040f160(local_38);
          if (_DAT_0056e0e8 <= *(float *)(*piVar4 + 0xbf4)) {
            piVar4 = (int *)FUN_0040f160(local_38);
            *(undefined4 *)(*piVar4 + 0xc24) = 1;
          }
          piVar4 = (int *)FUN_0040f160(local_38);
          if (DAT_0056cd90 + DAT_0056d7c0 + DAT_0056d7bc < *(float *)(*piVar4 + 0xbe8)) {
LAB_004c2bc2:
            puVar9 = (undefined4 *)FUN_0040f160(local_38);
            FUN_004c3880((void *)*puVar9);
            goto LAB_004c2637;
          }
          piVar4 = (int *)FUN_0040f160(local_38);
          fVar10 = FUN_0047a630(*(float *)(*piVar4 + 0xbe4));
          local_a4 = (float)fVar10;
          if (DAT_0056cda4 / DAT_0056c8d0 + DAT_0056d7bc <= local_a4) goto LAB_004c2bc2;
        }
LAB_004c304a:
        iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
        iVar5 = FUN_00411700(iVar5);
        if (iVar5 != 2) {
          FUN_00422e10(&local_28);
          FUN_00422e10(&local_34);
          piVar4 = (int *)FUN_0040f160(local_38);
          local_28 = *(float *)(*piVar4 + 0xbe4) - 0.0 / DAT_0056c8d0;
          piVar4 = (int *)FUN_0040f160(local_38);
          local_24 = *(float *)(*piVar4 + 0xbe8) - 0.0 / DAT_0056c8d0;
          piVar4 = (int *)FUN_0040f160(local_38);
          local_34 = 0.0 / DAT_0056c8d0 + *(float *)(*piVar4 + 0xbe4);
          piVar4 = (int *)FUN_0040f160(local_38);
          local_30 = 0.0 / DAT_0056c8d0 + *(float *)(*piVar4 + 0xbe8);
          piVar4 = (int *)FUN_0040f160(local_38);
          local_100 = (float *)(*piVar4 + 0xbe4);
          iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          local_104 = (float *)FUN_00460850(iVar5);
          iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          fVar10 = FUN_004c4920(iVar5);
          local_fc = (float)fVar10;
          local_f4 = local_fc;
          iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
          fVar10 = FUN_004c4920(iVar5);
          local_f8 = (float)fVar10;
          local_108 = local_f8 * local_fc;
          fVar10 = FUN_004560d0(local_104,local_100);
          local_10c = (float)fVar10;
          if (local_10c < local_108) {
LAB_004c3215:
            piVar4 = (int *)FUN_0040f160(local_38);
            local_40 = *(int *)(*piVar4 + 0xc28) + -1;
            switch(local_40) {
            case 0:
              piVar4 = (int *)FUN_0040f160(local_38);
              FUN_004c4b90(*piVar4);
              break;
            case 1:
            case 0xe:
              piVar4 = (int *)FUN_0040f160(local_38);
              FUN_004c4960(*piVar4);
              break;
            case 2:
              piVar4 = (int *)FUN_0040f160(local_38);
              FUN_004c4da0(*piVar4);
              break;
            case 3:
              local_11c = (void *)FUN_00464080(0);
              FUN_004e1310(local_11c,1);
              break;
            case 4:
              pvVar8 = (void *)FUN_00464080(0);
              FUN_004e1510(pvVar8);
              break;
            case 5:
              local_114 = (void *)FUN_00464080(0);
              FUN_004e11a0(local_114,1);
              break;
            case 6:
              local_118 = (void *)FUN_00464080(0);
              FUN_004e10e0(local_118,1);
              break;
            case 7:
              piVar4 = (int *)FUN_0040f160(local_38);
              FUN_004c47c0(*piVar4);
              break;
            case 8:
            case 0xc:
              local_120 = (void *)FUN_00464230(0);
              FUN_00533780(local_120,1);
              local_124 = (void *)FUN_00464080(0);
              FUN_004a9fb0(local_124,1);
              iVar5 = FUN_00513dd0();
              cVar2 = FUN_00485a40(iVar5);
              if (cVar2 == '\0') {
                local_128 = (void *)FUN_00464080(0);
                FUN_00477f60(local_128,1);
                local_12c = (void *)FUN_00464080(0);
                FUN_004a9ec0(local_12c,10);
              }
              break;
            case 9:
              local_130 = (void *)FUN_00464230(0);
              FUN_00533780(local_130,1);
              local_134 = (void *)FUN_00464080(0);
              FUN_004a9fb0(local_134,1);
              iVar5 = FUN_00513dd0();
              cVar2 = FUN_00485a40(iVar5);
              if (cVar2 == '\0') {
                local_138 = (void *)FUN_00464080(0);
                FUN_00477f60(local_138,1);
                local_13c = (void *)FUN_00464080(0);
                FUN_004a9d80(local_13c,10);
              }
              break;
            case 10:
              local_140 = (void *)FUN_00464230(0);
              FUN_00533780(local_140,1);
              local_144 = (void *)FUN_00464080(0);
              FUN_004a9fb0(local_144,1);
              iVar5 = FUN_00513dd0();
              cVar2 = FUN_00485a40(iVar5);
              if (cVar2 == '\0') {
                local_148 = (void *)FUN_00464080(0);
                FUN_00477f60(local_148,1);
                local_14c = (void *)FUN_00464080(0);
                FUN_004a9f20(local_14c,10);
              }
              break;
            case 0xb:
              local_150 = (void *)FUN_00464230(0);
              FUN_00533780(local_150,1);
              local_154 = (void *)FUN_00464080(0);
              FUN_004a9fb0(local_154,1);
              iVar5 = FUN_00513dd0();
              cVar2 = FUN_00485a40(iVar5);
              if (cVar2 == '\0') {
                local_158 = (void *)FUN_00464080(0);
                FUN_00477f60(local_158,1);
                local_15c = (void *)FUN_00464080(0);
                FUN_004a9e60(local_15c,10);
              }
            }
            piVar4 = (int *)FUN_0040f160(local_38);
            FUN_00426eb0(&DAT_005ba830,0x25,*(float *)(*piVar4 + 0xbe4));
            puVar9 = (undefined4 *)FUN_0040f160(local_38);
            FUN_004c3880((void *)*puVar9);
            goto LAB_004c2637;
          }
          if (*(int *)((int)local_3c + 0x49c884) != 0) {
            piVar4 = (int *)FUN_0040f160(local_38);
            fVar10 = FUN_004560d0((float *)((int)local_3c + 0x49c888),(float *)(*piVar4 + 0xbe4));
            local_110 = (float)fVar10;
            if (local_110 < DAT_0056cda0) goto LAB_004c3215;
          }
          piVar4 = (int *)FUN_0040f160(local_38);
          if (((*(int *)(*piVar4 + 0xc24) != 4) &&
              (piVar4 = (int *)FUN_0040f160(local_38), *(int *)(*piVar4 + 0xc24) != 3)) &&
             (piVar4 = (int *)FUN_0040f160(local_38), *(int *)(*piVar4 + 0xc24) != 2)) {
            piVar4 = (int *)FUN_0040f160(local_38);
            local_16c = (float *)(*piVar4 + 0xbe4);
            iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
            local_170 = (float *)FUN_00460850(iVar5);
            iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
            fVar10 = FUN_004c4900(iVar5);
            local_168 = (float)fVar10;
            local_160 = local_168;
            iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
            fVar10 = FUN_004c4900(iVar5);
            local_164 = (float)fVar10;
            local_174 = local_164 * local_168;
            fVar10 = FUN_004560d0(local_170,local_16c);
            local_178 = (float)fVar10;
            if (local_178 < local_174) {
              iVar5 = FUN_0040ff90(*(int *)((int)local_3c + 0x49c898));
              fVar10 = FUN_004c4940(iVar5);
              local_17c = (float)fVar10;
              local_180 = local_17c / DAT_0056c8d8;
              piVar4 = (int *)FUN_0040f160(local_38);
              *(float *)(*piVar4 + 0xc30) = local_180;
              piVar4 = (int *)FUN_0040f160(local_38);
              *(undefined4 *)(*piVar4 + 0xc24) = 4;
            }
          }
        }
        piVar4 = (int *)FUN_0040f160(local_38);
        bVar3 = FUN_00445ab0(*piVar4 + 0x18);
        if (CONCAT31(extraout_var_01,bVar3) != 0) {
          piVar4 = (int *)FUN_0040f160(local_38);
          FUN_0042b5d0((void *)(*piVar4 + 0x18));
        }
        piVar4 = (int *)FUN_0040f160(local_38);
        bVar3 = FUN_00445ab0(*piVar4 + 0x5fc);
        if (CONCAT31(extraout_var_02,bVar3) != 0) {
          piVar4 = (int *)FUN_0040f160(local_38);
          FUN_0042b5d0((void *)(*piVar4 + 0x5fc));
        }
        piVar4 = (int *)FUN_0040f160(local_38);
        local_188 = (void *)(*piVar4 + 0xbe0);
        piVar4 = (int *)FUN_0040f160(local_38);
        local_184 = (undefined4 *)(*piVar4 + 0xbe4);
        FUN_004502c0(local_188,local_184);
        piVar4 = (int *)FUN_0040f160(local_38);
        local_18c = (undefined4 *)(*piVar4 + 0xc04);
        FUN_00423540(local_18c);
        *(int *)((int)local_3c + 0x49c864) = *(int *)((int)local_3c + 0x49c864) + 1;
      }
    }
LAB_004c2637:
    FUN_00411c30(local_1c);
  } while( true );
}

