/* Entry: 0x004b2c90; symbol: FUN_004b2c90; body bytes: 6073 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004b2c90(int param_1)

{
  float fVar1;
  bool bVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  undefined3 extraout_var;
  int *piVar6;
  undefined4 *puVar7;
  undefined3 extraout_var_00;
  undefined4 uVar8;
  float *pfVar9;
  void *pvVar10;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  float *pfVar11;
  float10 fVar12;
  undefined4 local_1a4;
  undefined4 local_1a0;
  undefined4 local_19c;
  undefined4 local_198;
  undefined4 local_194;
  undefined4 local_190;
  undefined4 local_18c;
  undefined4 local_188;
  float local_184;
  void *local_180;
  undefined4 *local_17c;
  undefined4 *local_178;
  float local_174;
  void *local_170;
  void *local_16c;
  void *local_168;
  void *local_164;
  void *local_160;
  undefined4 local_15c;
  void *local_158;
  undefined4 local_154;
  void *local_150;
  void *local_14c;
  float local_148;
  undefined4 *local_144;
  float *local_140;
  float *local_13c;
  void *local_138;
  float local_134;
  float local_130;
  float *local_12c;
  float *local_128;
  void *local_124;
  float local_120;
  undefined4 local_11c;
  undefined4 local_118;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  undefined4 local_104;
  float local_100;
  void *local_fc;
  void *local_f8;
  void *local_f4;
  void *local_f0;
  int local_ec;
  void *local_e8;
  void *local_e4;
  int local_e0;
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
  void *local_b4;
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
  float local_68;
  float local_64;
  undefined4 *local_60;
  int local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  void *local_40;
  void *local_3c;
  int local_38;
  uint local_34;
  int local_30;
  float local_2c;
  float local_28;
  undefined4 local_24;
  undefined4 local_20 [2];
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_30 = param_1;
  if ((*(uint *)(param_1 + 0x1a4) >> 8 & 1) != 0) {
    local_144 = (undefined4 *)(param_1 + 0x1a8);
    FUN_00423540(local_144);
  }
  if ((*(uint *)(local_30 + 0x1a4) >> 0xb & 3) != 0) {
    local_17c = (undefined4 *)(local_30 + 0x1a8);
    FUN_00423540(local_17c);
    if ((*(uint *)(local_30 + 0x1a4) >> 0xb & 3) == 1) {
      local_180 = (void *)(local_30 + 0x1a8);
      bVar2 = FUN_004235c0(local_180,0x5a);
      if (bVar2) {
        if (*(float *)(local_30 + 0x120) <= _DAT_0056e0e8) {
          local_70 = (void *)(local_30 + 0x1a8);
          bVar2 = FUN_0047c620(local_70,0x5a);
          if (bVar2) {
            FUN_00426d70(&DAT_005ba830,0x2f,0);
          }
          *(undefined4 *)(local_30 + 0x120) = *(undefined4 *)(local_30 + 0x124);
          *(undefined4 *)(local_30 + 0x130) = 0;
          *(undefined4 *)(local_30 + 300) = *(undefined4 *)(local_30 + 0x128);
          *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xffffe7ff | 0x1000;
        }
        else {
          local_6c = (void *)(local_30 + 0x1a8);
          iVar4 = FUN_00472040(local_6c,4);
          if (iVar4 == 0) {
            FUN_00426d70(&DAT_005ba830,0x27,0);
          }
          *(float *)(local_30 + 0x120) = *(float *)(local_30 + 0x120) - DAT_0056c8cc;
          *(int *)(local_30 + 300) = *(int *)(local_30 + 300) + *(int *)(local_30 + 0x130);
        }
      }
    }
    local_74 = (void *)(local_30 + 0x1a8);
    bVar2 = FUN_004235c0(local_74,*(int *)(local_30 + 0x1b8));
    if (bVar2) {
      FUN_004b57d0(local_30);
      *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xffffe7ff;
    }
  }
  if ((*(int *)(local_30 + 0x138) != 0) &&
     (bVar2 = FUN_0047a340((undefined4 *)(local_30 + 0x114)), !bVar2)) {
    *(undefined4 *)(local_30 + 0x138) = 0;
  }
  iVar4 = FUN_00478060(0);
  if ((iVar4 != 0) && (-1 < *(int *)(local_30 + 0x1c4))) {
    local_78 = (void *)FUN_00478060(0);
    iVar4 = FUN_00485660(local_78,0);
    if (iVar4 != 0) {
      iVar4 = FUN_00478060(0);
      uVar5 = FUN_004b83f0(iVar4);
      if (((uVar5 == 0) && (bVar2 = FUN_00478160(local_30), CONCAT31(extraout_var,bVar2) == 0)) &&
         (uVar5 = FUN_004b8450(DAT_005ba828), uVar5 == 0)) {
        local_7c = (void *)(local_30 + 0x88);
        piVar6 = (int *)FUN_00414580(local_7c,0);
        FUN_0044fa70(*piVar6);
        local_80 = (void *)(local_30 + 0x88);
        piVar6 = (int *)FUN_00414580(local_80,1);
        FUN_0044fa70(*piVar6);
        if ((*(uint *)(local_30 + 0x1a4) >> 9 & 3) == 0) {
          iVar4 = FUN_00478ea0(0);
          uVar5 = FUN_004888a0(iVar4);
          if (uVar5 == 0) {
            iVar4 = FUN_00460830(0);
            iVar4 = FUN_00460850(iVar4);
            if (DAT_0056cd98 < *(float *)(iVar4 + 4) || DAT_0056cd98 == *(float *)(iVar4 + 4))
            goto LAB_004b2faa;
LAB_004b2fee:
            *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffff9ff | 0x200;
            local_84 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_84,0);
            FUN_004b8580((void *)*puVar7);
            local_88 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_88,1);
            FUN_004b8580((void *)*puVar7);
          }
          else {
LAB_004b2faa:
            iVar4 = FUN_00478ea0(0);
            uVar5 = FUN_004888a0(iVar4);
            if (uVar5 != 0) {
              iVar4 = FUN_00460830(0);
              iVar4 = FUN_00460850(iVar4);
              if (DAT_0056d7c0 - DAT_0056cd98 < *(float *)(iVar4 + 4)) goto LAB_004b2fee;
            }
          }
        }
        else if ((*(uint *)(local_30 + 0x1a4) >> 9 & 3) == 1) {
          iVar4 = FUN_00478ea0(0);
          uVar5 = FUN_004888a0(iVar4);
          if (uVar5 == 0) {
            iVar4 = FUN_00460830(0);
            iVar4 = FUN_00460850(iVar4);
            if (DAT_0056fb80 < *(float *)(iVar4 + 4) || DAT_0056fb80 == *(float *)(iVar4 + 4))
            goto LAB_004b309d;
LAB_004b30e1:
            local_8c = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_8c,0);
            FUN_004b85c0((void *)*puVar7);
            local_90 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_90,1);
            FUN_004b85c0((void *)*puVar7);
            *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffff9ff;
          }
          else {
LAB_004b309d:
            iVar4 = FUN_00478ea0(0);
            uVar5 = FUN_004888a0(iVar4);
            if (uVar5 != 0) {
              iVar4 = FUN_00460830(0);
              iVar4 = FUN_00460850(iVar4);
              if (DAT_0056d7c0 - DAT_0056fb80 < *(float *)(iVar4 + 4)) goto LAB_004b30e1;
            }
          }
        }
        else {
          iVar4 = FUN_00478ea0(0);
          bVar2 = FUN_004887a0(iVar4);
          if (bVar2) {
            local_94 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_94,0);
            FUN_004888e0((void *)*puVar7);
            local_98 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_98,1);
            FUN_004888e0((void *)*puVar7);
          }
          else {
            local_9c = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_9c,0);
            FUN_00488900((void *)*puVar7);
            local_a0 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_a0,1);
            FUN_00488900((void *)*puVar7);
          }
          local_a4 = (void *)(local_30 + 0x88);
          puVar7 = (undefined4 *)FUN_00414580(local_a4,0);
          FUN_004b85e0((void *)*puVar7);
          local_a8 = (void *)(local_30 + 0x88);
          puVar7 = (undefined4 *)FUN_00414580(local_a8,1);
          FUN_004b85e0((void *)*puVar7);
          *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffff9ff;
        }
        if (*(int *)(local_30 + 0x1c4) < *(int *)(local_30 + 0x1cc)) {
          if (*(int *)(local_30 + 0x1c4) < 2) {
            local_ac = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_ac,0);
            local_b0 = (void *)*puVar7;
            FUN_004388f0(local_b0,9);
            local_b4 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_b4,1);
            local_b8 = (void *)*puVar7;
            FUN_004388f0(local_b8,9);
            FUN_00426d70(&DAT_005ba830,0xc,0);
          }
          else if (*(int *)(local_30 + 0x1c4) < 5) {
            local_bc = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_bc,0);
            local_c0 = (void *)*puVar7;
            FUN_004388f0(local_c0,8);
            local_c4 = (void *)(local_30 + 0x88);
            puVar7 = (undefined4 *)FUN_00414580(local_c4,1);
            local_c8 = (void *)*puVar7;
            FUN_004388f0(local_c8,8);
            FUN_00426d70(&DAT_005ba830,0xb,0);
          }
        }
        else if (*(int *)(local_30 + 0x1cc) < *(int *)(local_30 + 0x1c4)) {
          local_cc = (void *)(local_30 + 0x88);
          puVar7 = (undefined4 *)FUN_00414580(local_cc,0);
          local_d0 = (void *)*puVar7;
          FUN_004388f0(local_d0,7);
          local_d4 = (void *)(local_30 + 0x88);
          puVar7 = (undefined4 *)FUN_00414580(local_d4,1);
          local_d8 = (void *)*puVar7;
          FUN_004388f0(local_d8,7);
        }
        if (*(int *)(local_30 + 0x1c4) != *(int *)(local_30 + 0x1cc)) {
          local_dc = (void *)(local_30 + 0x88);
          puVar7 = (undefined4 *)FUN_00414580(local_dc,0);
          local_e4 = (void *)*puVar7;
          local_e0 = *(int *)(local_30 + 0x1c4) / 10 + 0xef;
          FUN_0044bdc0(local_e4,local_e0);
          local_e8 = (void *)(local_30 + 0x88);
          puVar7 = (undefined4 *)FUN_00414580(local_e8,1);
          local_f0 = (void *)*puVar7;
          local_ec = *(int *)(local_30 + 0x1c4) % 10 + 0xef;
          FUN_0044bdc0(local_f0,local_ec);
        }
        *(undefined4 *)(local_30 + 0x1cc) = *(undefined4 *)(local_30 + 0x1c4);
        goto LAB_004b3505;
      }
    }
  }
  local_f4 = (void *)(local_30 + 0x88);
  piVar6 = (int *)FUN_00414580(local_f4,0);
  FUN_00450160(*piVar6);
  local_f8 = (void *)(local_30 + 0x88);
  piVar6 = (int *)FUN_00414580(local_f8,1);
  FUN_00450160(*piVar6);
  *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffff9ff | 0x400;
LAB_004b3505:
  iVar4 = FUN_00478060(0);
  if (iVar4 != 0) {
    iVar4 = FUN_00478060(0);
    uVar5 = FUN_004b83f0(iVar4);
    if (uVar5 == 0) {
      FUN_00422e10(&local_14);
      FUN_00422e10(&local_2c);
      FUN_00422e10(local_20);
      for (local_34 = 0; (int)local_34 < 2; local_34 = local_34 + 1) {
        local_fc = (void *)FUN_00478060(0);
        local_4c = FUN_00485660(local_fc,local_34);
        if (local_4c == 0) {
          *(undefined4 *)(local_30 + 0x1d0 + local_34 * 0x54) = 0;
          *(undefined4 *)(local_30 + local_34 * 0x54 + 0x1e0) = 0;
          *(undefined4 *)(local_30 + local_34 * 0x54 + 0x1e8) = 0;
          *(undefined4 *)(local_30 + local_34 * 0x54 + 0x1f0) = 0;
          *(undefined4 *)(local_30 + local_34 * 0x54 + 0x1f8) = 0;
          if (*(int *)(local_30 + 0x21c + local_34 * 0x54) != 0) {
            for (local_50 = 0; local_50 < 7; local_50 = local_50 + 1) {
              FUN_0044fcd0((undefined4 *)(local_30 + local_34 * 0x54 + 0x200 + local_50 * 4));
            }
            *(undefined4 *)(local_30 + 0x21c + local_34 * 0x54) = 0;
          }
          if (local_34 == 0) {
            FUN_0044fcd0((undefined4 *)(local_30 + 0xcc));
          }
        }
        else {
          puVar7 = (undefined4 *)FUN_004aabd0(local_4c);
          iVar4 = FUN_0040c300(puVar7);
          if (((iVar4 < 100000) && (cVar3 = FUN_0049c050(local_4c), cVar3 == '\0')) &&
             (bVar2 = FUN_00478160(local_30), CONCAT31(extraout_var_00,bVar2) == 0)) {
            local_60 = (undefined4 *)FUN_004aabd0(local_4c);
            uVar8 = FUN_0040c300(local_60);
            *(undefined4 *)(local_30 + 0x1d8 + local_34 * 0x54) = uVar8;
            local_100 = (float)*(int *)(local_30 + 0x1d8 + local_34 * 0x54);
            iVar4 = FUN_0040ff90((int)local_60);
            *(float *)(local_30 + 0x1d4 + local_34 * 0x54) = local_100 / (float)iVar4;
            fVar1 = *(float *)(local_30 + 0x1d4 + local_34 * 0x54);
            pfVar9 = (float *)(local_30 + 0x1d0 + local_34 * 0x54);
            if (*pfVar9 <= fVar1 && fVar1 != *pfVar9) {
              *(float *)(local_30 + 0x1d0 + local_34 * 0x54) =
                   *(float *)(local_30 + 0x1d0 + local_34 * 0x54) + _DAT_00570aa8;
            }
            fVar1 = *(float *)(local_30 + 0x1d0 + local_34 * 0x54);
            pfVar9 = (float *)(local_30 + 0x1d4 + local_34 * 0x54);
            if (*pfVar9 <= fVar1 && fVar1 != *pfVar9) {
              *(undefined4 *)(local_30 + 0x1d0 + local_34 * 0x54) =
                   *(undefined4 *)(local_30 + 0x1d4 + local_34 * 0x54);
            }
            if (*(int *)(local_30 + 0x21c + local_34 * 0x54) == 0) {
              local_104 = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_19c,(uint *)"front",0x176,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x200) = *puVar7;
              local_108 = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_1a0,(uint *)"front",0x177,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x204) = *puVar7;
              local_10c = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_1a4,(uint *)"front",0x178,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x208) = *puVar7;
              local_110 = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_188,(uint *)"front",0x179,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x20c) = *puVar7;
              local_114 = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_18c,(uint *)"front",0x179,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x210) = *puVar7;
              local_118 = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_190,(uint *)"front",0x179,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x214) = *puVar7;
              local_11c = FUN_004b8250(local_30);
              puVar7 = FUN_00450c70(&local_194,(uint *)"front",0x179,-1,(undefined4 *)0x0);
              *(undefined4 *)(local_30 + local_34 * 0x54 + 0x218) = *puVar7;
              *(undefined4 *)(local_30 + 0x21c + local_34 * 0x54) = 1;
            }
            FUN_004b8f60(local_30);
            local_40 = (void *)FUN_0044ced0((undefined4 *)(local_30 + local_34 * 0x54 + 0x200));
            FUN_00450460(local_40,(float)(*(uint *)(local_30 + 0x1d0 + local_34 * 0x54) ^
                                         _DAT_0056d7d0) * DAT_0056e0f0 * DAT_0056c8d0);
            pfVar9 = (float *)FUN_0047a2c0(local_4c);
            local_c = pfVar9[2];
            local_14 = *pfVar9 * DAT_0056c8d0;
            local_10 = pfVar9[1] * DAT_0056c8d0;
            FUN_0045dd60(local_40,&local_14);
            local_40 = (void *)FUN_0044ced0((undefined4 *)(local_30 + local_34 * 0x54 + 0x204));
            FUN_0045dd60(local_40,&local_14);
            local_40 = (void *)FUN_0044ced0((undefined4 *)(local_30 + local_34 * 0x54 + 0x208));
            FUN_0045dd60(local_40,&local_14);
            local_2c = 0.0;
            local_28 = DAT_0056fc04 * DAT_0056c8d0;
            local_24 = 0;
            local_18 = 0;
            for (local_48 = 0; local_48 < 4; local_48 = local_48 + 1) {
              local_40 = (void *)FUN_0044ced0((undefined4 *)
                                              (local_30 + local_34 * 0x54 + 0x20c + local_48 * 4));
              if ((*(float *)(local_30 + local_34 * 0x54 + 0x1e0 + local_48 * 8) == _DAT_0056e0e8)
                 || (*(float *)(local_30 + 0x1d0 + local_34 * 0x54) <=
                     *(float *)(local_30 + local_34 * 0x54 + 0x1e0 + local_48 * 8))) {
                FUN_00450160((int)local_40);
              }
              else {
                FUN_0044fa70((int)local_40);
                local_120 = *(float *)(local_30 + local_34 * 0x54 + 0x1e0 + local_48 * 8);
                fVar12 = FUN_00438540((float)((uint)(DAT_0056e0f0 * DAT_0056c8d0) ^ _DAT_0056d7d0) /
                                      DAT_0056c8d0 - DAT_0056e0f0 * DAT_0056c8d0 * local_120);
                local_64 = (float)fVar12;
                FUN_00450590(local_40,local_64);
                FUN_00458fa0(local_20,(float *)local_20,&local_2c,local_64);
                FUN_004296e0(local_20,&local_14);
                FUN_0045dd60(local_40,local_20);
              }
            }
            if (*(int *)(local_30 + 0x220 + local_34 * 0x54) == 0) {
              iVar4 = FUN_00460830(0);
              local_13c = (float *)FUN_00460850(iVar4);
              local_138 = (void *)FUN_00478060(0);
              iVar4 = FUN_00485660(local_138,local_34);
              local_140 = (float *)FUN_0047a2c0(iVar4);
              local_174 = DAT_0056fa30 * DAT_0056fa30;
              fVar12 = FUN_004560d0(local_140,local_13c);
              local_148 = (float)fVar12;
              if (local_148 < local_174) {
                for (local_5c = 0; local_5c < 7; local_5c = local_5c + 1) {
                  FUN_00486350((void *)(local_30 + local_34 * 0x54 + 0x200 + local_5c * 4));
                }
                *(undefined4 *)(local_30 + 0x220 + local_34 * 0x54) = 1;
              }
            }
            else {
              iVar4 = FUN_00460830(0);
              local_128 = (float *)FUN_00460850(iVar4);
              local_124 = (void *)FUN_00478060(0);
              iVar4 = FUN_00485660(local_124,local_34);
              local_12c = (float *)FUN_0047a2c0(iVar4);
              local_134 = DAT_0056fe7c * DAT_0056fe7c;
              fVar12 = FUN_004560d0(local_12c,local_128);
              local_130 = (float)fVar12;
              if (local_134 <= local_130) {
                for (local_58 = 0; local_58 < 7; local_58 = local_58 + 1) {
                  FUN_00486310((void *)(local_30 + local_34 * 0x54 + 0x200 + local_58 * 4));
                }
                *(undefined4 *)(local_30 + 0x220 + local_34 * 0x54) = 0;
              }
            }
          }
          else if (*(int *)(local_30 + 0x21c + local_34 * 0x54) != 0) {
            for (local_54 = 0; local_54 < 7; local_54 = local_54 + 1) {
              FUN_0044fcd0((undefined4 *)(local_30 + local_34 * 0x54 + 0x200 + local_54 * 4));
            }
            *(undefined4 *)(local_30 + 0x21c + local_34 * 0x54) = 0;
          }
        }
      }
    }
  }
  for (local_44 = 0; local_44 < 10; local_44 = local_44 + 1) {
    if (local_44 < *(int *)(local_30 + 0x180)) {
      local_14c = (void *)(local_30 + 0xd0);
      local_150 = (void *)FUN_00414580(local_14c,local_44);
      bVar2 = FUN_00449430(local_150,0);
      if (bVar2) {
        local_154 = *(undefined4 *)(local_30 + 0x2cc);
        puVar7 = FUN_00450c70(&local_198,(uint *)"front",local_44 + 0x3a,-1,(undefined4 *)0x0);
        local_15c = *puVar7;
        local_158 = (void *)(local_30 + 0xd0);
        puVar7 = (undefined4 *)FUN_00414580(local_158,local_44);
        *puVar7 = local_15c;
      }
    }
    else {
      local_160 = (void *)(local_30 + 0xd0);
      local_164 = (void *)FUN_00414580(local_160,local_44);
      bVar2 = FUN_0047c650(local_164,0);
      if (bVar2) {
        local_168 = (void *)(local_30 + 0xd0);
        pvVar10 = (void *)FUN_00414580(local_168,local_44);
        FUN_00479040(pvVar10);
        local_16c = (void *)(local_30 + 0xd0);
        uVar8 = 0;
        pvVar10 = (void *)FUN_00414580(local_16c,local_44);
        FUN_004117a0(pvVar10,uVar8);
      }
    }
  }
  bVar2 = FUN_00478160(local_30);
  if ((CONCAT31(extraout_var_01,bVar2) != 0) &&
     (bVar2 = FUN_004b4450(*(void **)(local_30 + 0x1bc)), CONCAT31(extraout_var_02,bVar2) != 0)) {
    FUN_004ae830(*(void **)(local_30 + 0x1bc));
    *(undefined4 *)(local_30 + 0x1bc) = 0;
  }
  iVar4 = FUN_00478060(0);
  if (iVar4 != 0) {
    local_170 = (void *)FUN_00478060(0);
    local_38 = FUN_00485660(local_170,0);
    if (((local_38 == 0) || (bVar2 = FUN_004b83b0(local_38), CONCAT31(extraout_var_03,bVar2) == 0))
       || (bVar2 = FUN_004b8470(local_38), CONCAT31(extraout_var_04,bVar2) == 0)) {
      FUN_00450120();
    }
    else {
      local_3c = (void *)FUN_0044ced0((undefined4 *)(local_30 + 0x90));
      FUN_0044fa70((int)local_3c);
      iVar4 = FUN_00478ea0(0);
      bVar2 = FUN_004887a0(iVar4);
      if (bVar2) {
        if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 0) {
          iVar4 = FUN_004aabd0(local_38);
          iVar4 = FUN_00412730(iVar4);
          if (iVar4 < 2000) {
            FUN_004388f0(local_3c,7);
            *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffffff9 | 2;
          }
        }
        else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 1) {
          iVar4 = FUN_004aabd0(local_38);
          iVar4 = FUN_00412730(iVar4);
          if (iVar4 < 1000) {
            FUN_004388f0(local_3c,8);
            *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffffff9 | 4;
          }
        }
        else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 2) {
          iVar4 = FUN_004aabd0(local_38);
          iVar4 = FUN_00412730(iVar4);
          if (iVar4 < 400) {
            FUN_004388f0(local_3c,9);
            *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) | 6;
          }
        }
        else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 3) {
          iVar4 = FUN_004aabd0(local_38);
          iVar4 = FUN_00412730(iVar4);
          if (400 < iVar4) {
            FUN_004388f0(local_3c,10);
            *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffffff9;
          }
        }
      }
      else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 0) {
        iVar4 = FUN_004aabd0(local_38);
        iVar4 = FUN_00412730(iVar4);
        if (iVar4 < 700) {
          FUN_004388f0(local_3c,7);
          *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffffff9 | 2;
        }
      }
      else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 1) {
        iVar4 = FUN_004aabd0(local_38);
        iVar4 = FUN_00412730(iVar4);
        if (iVar4 < 400) {
          FUN_004388f0(local_3c,8);
          *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffffff9 | 4;
        }
      }
      else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 2) {
        iVar4 = FUN_004aabd0(local_38);
        iVar4 = FUN_00412730(iVar4);
        if (iVar4 < 200) {
          FUN_004388f0(local_3c,9);
          *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) | 6;
        }
      }
      else if ((*(uint *)(local_30 + 0x1a4) >> 1 & 3) == 3) {
        iVar4 = FUN_004aabd0(local_38);
        iVar4 = FUN_00412730(iVar4);
        if (200 < iVar4) {
          FUN_004388f0(local_3c,10);
          *(uint *)(local_30 + 0x1a4) = *(uint *)(local_30 + 0x1a4) & 0xfffffff9;
        }
      }
      pfVar9 = (float *)FUN_0047a2c0(local_38);
      local_184 = (*pfVar9 + DAT_0056cd94 + DAT_0056e050) * DAT_0056c8d0;
      pfVar9 = (float *)FUN_0044cb20((int)local_3c);
      *pfVar9 = local_184;
      iVar4 = FUN_0044cb20((int)local_3c);
      *(undefined4 *)(iVar4 + 4) = DAT_00570af8;
      pfVar9 = (float *)FUN_0047a2c0(local_38);
      iVar4 = FUN_00460830(0);
      pfVar11 = (float *)FUN_00460850(iVar4);
      fVar12 = FUN_0047a630(*pfVar9 - *pfVar11);
      local_68 = (float)fVar12;
      if ((double)local_68 < _DAT_00570ab8) {
        FUN_00470b60(local_3c,(char)(int)((DAT_00570ae0 * local_68) / DAT_0056fa28) + '@');
      }
      else {
        FUN_00470b60(local_3c,0xff);
      }
      pfVar9 = (float *)FUN_0047a2c0(local_38);
      if ((*pfVar9 <= DAT_0056fa6c && DAT_0056fa6c != *pfVar9) ||
         (pfVar9 = (float *)FUN_0047a2c0(local_38), DAT_0056e050 < *pfVar9)) {
        FUN_00470b60(local_3c,0);
      }
    }
  }
  local_178 = (undefined4 *)(local_30 + 0x140);
  FUN_00423540(local_178);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}


