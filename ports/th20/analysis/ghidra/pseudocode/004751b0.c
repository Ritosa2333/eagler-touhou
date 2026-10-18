/* Entry: 0x004751b0; symbol: FUN_004751b0; body bytes: 8557 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004751b0(undefined4 *param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  float *pfVar5;
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *extraout_ECX_01;
  void *extraout_ECX_02;
  void *extraout_ECX_03;
  void *extraout_ECX_04;
  void *extraout_ECX_05;
  void *extraout_ECX_06;
  void *extraout_ECX_07;
  void *extraout_ECX_08;
  void *extraout_ECX_09;
  void *pvVar6;
  void *extraout_ECX_10;
  void *extraout_ECX_11;
  void *extraout_ECX_12;
  void *extraout_ECX_13;
  void *extraout_ECX_14;
  void *extraout_ECX_15;
  void *extraout_ECX_16;
  void *extraout_ECX_17;
  void *extraout_ECX_18;
  void *extraout_ECX_19;
  void *extraout_ECX_20;
  void *extraout_ECX_21;
  void *extraout_ECX_22;
  void *extraout_ECX_23;
  void *extraout_ECX_24;
  void *extraout_ECX_25;
  void *extraout_ECX_26;
  void *extraout_ECX_27;
  void *extraout_ECX_28;
  float *pfVar7;
  float10 fVar8;
  float local_300 [7];
  float local_2e4 [3];
  float local_2d8 [3];
  float local_2cc [3];
  undefined1 local_2c0 [12];
  undefined4 local_2b4 [3];
  undefined4 local_2a8 [3];
  undefined4 local_29c [3];
  undefined4 local_290 [3];
  undefined4 local_284 [3];
  undefined4 local_278 [3];
  undefined4 local_26c [3];
  undefined4 local_260 [3];
  undefined4 local_254 [3];
  undefined4 *local_248;
  undefined4 *local_244;
  undefined4 *local_240;
  float local_23c;
  float local_238;
  float local_234;
  float local_230;
  float local_22c;
  float local_228;
  undefined4 *local_224;
  undefined4 *local_220;
  float local_21c;
  float local_218;
  float local_214;
  float local_210;
  float local_20c;
  undefined4 *local_208;
  undefined4 *local_204;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f4;
  float local_1f0;
  float local_1ec;
  float local_1e8;
  undefined4 *local_1e4;
  undefined4 *local_1e0;
  float local_1dc;
  float local_1d8;
  float local_1d4;
  undefined4 *local_1d0;
  undefined4 *local_1cc;
  float local_1c8;
  float local_1c4;
  float local_1c0;
  undefined4 *local_1bc;
  undefined4 *local_1b8;
  float local_1b4;
  float local_1b0;
  float local_1ac;
  float local_1a8;
  undefined4 *local_1a4;
  undefined4 *local_1a0;
  float local_19c;
  float local_198;
  float local_194;
  undefined4 *local_190;
  undefined4 *local_18c;
  undefined4 *local_188;
  float local_184;
  float local_180;
  float local_17c;
  float local_178;
  undefined4 *local_174;
  undefined4 *local_170;
  undefined4 *local_16c;
  undefined4 *local_168;
  float local_164;
  float local_160;
  float local_15c;
  undefined4 *local_158;
  undefined4 *local_154;
  undefined4 *local_150;
  undefined4 *local_14c;
  float local_148;
  float local_144;
  float local_140;
  float local_13c;
  undefined4 *local_138;
  undefined4 *local_134;
  undefined4 *local_130;
  undefined4 *local_12c;
  float local_128;
  float local_124;
  float local_120;
  undefined4 *local_11c;
  undefined4 *local_118;
  float local_114;
  float local_110;
  float local_10c;
  float local_108;
  float local_104;
  undefined4 *local_100;
  undefined4 *local_fc;
  undefined4 *local_f8;
  undefined4 *local_f4;
  undefined4 *local_f0;
  undefined4 *local_ec;
  void *local_e8;
  void *local_e4;
  undefined4 *local_e0;
  undefined4 *local_dc;
  undefined4 *local_d8;
  undefined4 *local_d4;
  undefined4 *local_d0;
  undefined4 *local_cc;
  undefined4 *local_c8;
  undefined4 *local_c4;
  undefined4 *local_c0;
  undefined4 *local_bc;
  undefined4 *local_b8;
  undefined4 *local_b4;
  undefined4 *local_b0;
  undefined4 *local_ac;
  undefined4 *local_a8;
  undefined4 *local_a4;
  undefined4 *local_a0;
  undefined4 *local_9c;
  undefined4 *local_98;
  undefined4 *local_94;
  undefined4 *local_90;
  undefined4 *local_8c;
  undefined4 *local_88;
  undefined4 *local_84;
  undefined4 *local_80;
  undefined4 *local_7c;
  undefined4 *local_78;
  undefined4 *local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  int local_50;
  float local_4c;
  float local_48;
  float local_44;
  int local_40;
  float local_3c;
  int local_38;
  int *local_34;
  undefined4 *local_30;
  undefined4 local_2c [7];
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_30 = param_1;
  local_34 = (int *)FUN_00471160(*(int *)(param_1[0xdc] + 0x3330),param_1[4]);
LAB_004751ea:
  do {
    iVar2 = FUN_0040ff90((int)local_30);
    if (iVar2 < *local_34) {
      local_ec = local_30;
      FUN_00423540(local_30);
LAB_004760cb:
      iVar2 = FUN_00438500((int)(local_30 + 0xe));
      if (iVar2 != 0) {
        local_f0 = local_30 + 0xe;
        pfVar5 = FUN_0042ad80(local_f0,local_2cc);
        local_30[0x84] = *pfVar5;
        local_30[0x85] = pfVar5[1];
        local_30[0x86] = pfVar5[2];
      }
      iVar2 = FUN_00438500((int)(local_30 + 0x23));
      if (iVar2 != 0) {
        local_f4 = local_30 + 0x23;
        pfVar5 = FUN_0042ad80(local_f4,local_2d8);
        local_30[0x81] = *pfVar5;
        local_30[0x82] = pfVar5[1];
        local_30[0x83] = pfVar5[2];
      }
      iVar2 = FUN_00474e20((int)(local_30 + 0x4d));
      if (iVar2 != 0) {
        local_f8 = local_30 + 0x4d;
        pfVar5 = FUN_00473590(local_f8,local_300);
        pfVar7 = (float *)(local_30 + 0xd5);
        for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
          *pfVar7 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar7 = pfVar7 + 1;
        }
      }
      iVar2 = FUN_00438500((int)(local_30 + 0x38));
      if (iVar2 != 0) {
        local_fc = local_30 + 0x38;
        pfVar5 = FUN_0042ad80(local_fc,local_2e4);
        local_30[0x87] = *pfVar5;
        local_30[0x88] = pfVar5[1];
        local_30[0x89] = pfVar5[2];
      }
      iVar2 = FUN_0041caf0((int)(local_30 + 0x76));
      if (iVar2 != 0) {
        fVar8 = FUN_0042a110((float *)(local_30 + 0x76));
        local_30[0x96] = (float)fVar8;
      }
      if (*(char *)(local_30 + 5) != '\0') {
        local_40 = *(byte *)(local_30 + 5) - 1;
        switch(local_40) {
        case 0:
          local_100 = local_30 + 6;
          bVar1 = FUN_004235c0(local_100,0x200);
          if (bVar1) {
            local_54 = DAT_0056c8cc;
          }
          else {
            fVar8 = FUN_00423bd0((int)(local_30 + 6));
            local_104 = (float)fVar8;
            local_54 = local_104 / DAT_0056f114;
          }
          local_48 = local_54;
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_108 = (float)fVar8;
          fVar8 = FUN_00438540((local_108 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056cdb4);
          local_44 = (float)fVar8;
          fVar8 = FUN_00439820(extraout_ECX,local_44);
          local_10c = (float)fVar8;
          local_30[0x90] = local_10c * _DAT_0056fa54 * local_48;
          fVar8 = FUN_00438540(local_44 * DAT_0056c8d0);
          fVar8 = FUN_00439820(extraout_ECX_00,(float)fVar8);
          local_110 = (float)fVar8;
          local_30[0x92] = local_110 * _DAT_0056fa54 * local_48;
          fVar8 = FUN_00439820(extraout_ECX_01,local_44);
          local_114 = (float)fVar8;
          local_30[0x87] = (float)((uint)local_114 ^ _DAT_0056d7d0) * DAT_0056e1a0 * local_48;
          local_30[0x93] = (float)(local_30[0x90] ^ _DAT_0056d7d0) / DAT_0056c8d0;
          local_30[0x95] = (float)(local_30[0x92] ^ _DAT_0056d7d0) / DAT_0056c8d0;
          local_118 = local_30 + 6;
          FUN_00423540(local_118);
          local_11c = local_30 + 6;
          bVar1 = FUN_004235c0(local_11c,0x800);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0x400);
          }
          break;
        case 1:
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_120 = (float)fVar8;
          fVar8 = FUN_00438540((local_120 * DAT_0056e0f0 * DAT_0056c8d0) /
                               (DAT_0056f114 * DAT_0056f00c));
          local_58 = (float)fVar8;
          fVar8 = FUN_00439820(extraout_ECX_02,local_58);
          local_124 = (float)fVar8;
          local_30[0x87] = (float)((uint)local_124 ^ _DAT_0056d7d0) * DAT_0056c8cc;
          fVar8 = FUN_004397c0(extraout_ECX_03,local_58);
          local_128 = (float)fVar8;
          local_30[0x89] = local_128 * DAT_0056c8cc;
          local_12c = local_30 + 6;
          FUN_00423540(local_12c);
          local_130 = local_30 + 6;
          bVar1 = FUN_004235c0(local_130,0xc00);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 2:
          local_134 = local_30 + 6;
          iVar2 = FUN_00472040(local_134,0x400);
          fVar8 = FUN_00438540(((float)iVar2 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056cdb4);
          local_140 = (float)fVar8;
          local_138 = local_30 + 6;
          bVar1 = FUN_004235c0(local_138,0x400);
          if (bVar1) {
            local_5c = DAT_0056fa30;
            pvVar6 = extraout_ECX_04;
          }
          else {
            fVar8 = FUN_00423bd0((int)(local_30 + 6));
            local_13c = (float)fVar8;
            local_5c = (local_13c * DAT_0056fa30) / _DAT_0056cdb4;
            pvVar6 = extraout_ECX_05;
          }
          local_148 = local_5c;
          fVar8 = FUN_00439820(pvVar6,local_140);
          local_144 = (float)fVar8;
          local_30[0x90] = local_144 * local_148;
          local_30[0x91] = 0;
          local_30[0x92] = 0;
          local_30[0x93] = (float)(local_30[0x90] ^ _DAT_0056d7d0) / DAT_0056e734;
          local_14c = local_30 + 6;
          FUN_00423540(local_14c);
          local_150 = local_30 + 6;
          bVar1 = FUN_004235c0(local_150,0x800);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0x400);
          }
          break;
        case 3:
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_194 = (float)fVar8;
          fVar8 = FUN_00438540((local_194 * DAT_0056e0f0 * DAT_0056c8d0) /
                               (DAT_0056f114 * DAT_0056f00c));
          local_68 = (float)fVar8;
          fVar8 = FUN_00439820(extraout_ECX_10,local_68);
          local_198 = (float)fVar8;
          local_30[0x87] = (float)((uint)local_198 ^ _DAT_0056d7d0) * DAT_0056c8cc;
          fVar8 = FUN_004397c0(local_30,local_68);
          local_19c = (float)fVar8;
          local_30[0x89] = (float)((uint)local_19c ^ _DAT_0056d7d0) * DAT_0056c8cc;
          local_1a0 = local_30 + 6;
          FUN_00423540(local_1a0);
          local_1a4 = local_30 + 6;
          bVar1 = FUN_004235c0(local_1a4,0xc00);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 4:
          local_60 = DAT_0056c8cc;
          local_154 = local_30 + 6;
          iVar2 = FUN_00472040(local_154,0x400);
          fVar8 = FUN_00438540(((float)iVar2 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056cdb4);
          local_160 = (float)fVar8;
          local_158 = local_30 + 6;
          bVar1 = FUN_00423560(local_158,0x200);
          pvVar6 = extraout_ECX_06;
          if (bVar1) {
            fVar8 = FUN_00423bd0((int)(local_30 + 6));
            local_15c = (float)fVar8;
            local_60 = local_15c / DAT_0056f114;
            pvVar6 = extraout_ECX_07;
          }
          fVar8 = FUN_00439820(pvVar6,local_160);
          local_164 = (float)fVar8;
          local_30[0x92] = local_164 * _DAT_0056fa68 * local_60;
          local_168 = local_30 + 6;
          FUN_00423540(local_168);
          local_16c = local_30 + 6;
          bVar1 = FUN_004235c0(local_16c,0x800);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0x400);
          }
          break;
        case 5:
          local_170 = local_30 + 6;
          iVar2 = FUN_00472040(local_170,0x400);
          fVar8 = FUN_00438540(((float)iVar2 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056cdb4);
          local_17c = (float)fVar8;
          local_174 = local_30 + 10;
          bVar1 = FUN_004235c0(local_174,0x200);
          if (bVar1) {
            local_64 = 0.0;
            pvVar6 = extraout_ECX_08;
          }
          else {
            fVar8 = FUN_00423bd0((int)(local_30 + 10));
            local_178 = (float)fVar8;
            local_64 = (DAT_0056c8cc - local_178 / DAT_0056f114) * DAT_0056fa30;
            pvVar6 = extraout_ECX_09;
          }
          local_184 = local_64;
          fVar8 = FUN_00439820(pvVar6,local_17c);
          local_180 = (float)fVar8;
          local_30[0x90] = local_180 * local_184;
          local_30[0x91] = 0;
          local_30[0x92] = 0;
          local_30[0x93] = (float)(local_30[0x90] ^ _DAT_0056d7d0) / DAT_0056e734;
          local_188 = local_30 + 6;
          FUN_00423540(local_188);
          local_18c = local_30 + 10;
          FUN_00423540(local_18c);
          local_190 = local_30 + 6;
          bVar1 = FUN_004235c0(local_190,0x800);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0x400);
          }
          break;
        case 6:
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_1e8 = (float)fVar8;
          fVar8 = FUN_00438540((local_1e8 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056fa50);
          local_3c = (float)fVar8;
          fVar8 = FUN_00439820(extraout_ECX_17,local_3c);
          local_1ec = (float)fVar8;
          local_30[0x90] = local_1ec * _DAT_0056fa40;
          fVar8 = FUN_004397c0(local_30,local_3c);
          local_1f0 = (float)fVar8;
          local_30[0x91] = (float)((uint)local_1f0 ^ _DAT_0056d7d0) * _DAT_0056fa40;
          fVar8 = FUN_004397c0(extraout_ECX_18,local_3c);
          local_1f4 = (float)fVar8;
          local_30[0x92] = local_1f4 * _DAT_0056fa64;
          fVar8 = FUN_00439820(extraout_ECX_19,local_3c);
          local_1f8 = (float)fVar8;
          local_30[0x93] = local_1f8 * _DAT_0056cdc0;
          fVar8 = FUN_004397c0(local_30,local_3c);
          local_1fc = (float)fVar8;
          local_30[0x94] = (float)((uint)local_1fc ^ _DAT_0056d7d0) * _DAT_0056cdc0;
          fVar8 = FUN_00438540(local_3c * DAT_0056c8d0);
          fVar8 = FUN_004397c0(extraout_ECX_20,(float)fVar8);
          local_200 = (float)fVar8;
          local_30[0x95] = local_200 * _DAT_0056fa34 + DAT_0056e04c;
          local_204 = local_30 + 6;
          FUN_00423540(local_204);
          local_208 = local_30 + 6;
          bVar1 = FUN_004235c0(local_208,0x12c0);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 7:
          FUN_00447de0(&local_10);
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_20c = (float)fVar8;
          FUN_00452f20(&local_10,
                       (local_20c * DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056fa3c - DAT_0056e0f0);
          fVar8 = FUN_004292e0(&local_10);
          fVar8 = FUN_00439820(extraout_ECX_21,(float)fVar8);
          local_210 = (float)fVar8;
          local_30[0x90] = local_210 * _DAT_0056fa60;
          fVar8 = FUN_004292e0(&local_10);
          fVar8 = FUN_00439820(extraout_ECX_22,(float)fVar8);
          local_214 = (float)fVar8;
          local_30[0x91] = local_214 * _DAT_0056fa58;
          fVar8 = FUN_004292e0(&local_10);
          fVar8 = FUN_00439820(extraout_ECX_23,(float)fVar8);
          local_218 = (float)fVar8;
          local_30[0x92] = local_218 * _DAT_0056fa5c;
          fVar8 = FUN_004292e0(&local_10);
          fVar8 = FUN_00439820(extraout_ECX_24,(float)fVar8);
          local_21c = (float)fVar8;
          local_30[0x87] = (float)((uint)local_21c ^ _DAT_0056d7d0) * DAT_0056ef28;
          local_220 = local_30 + 6;
          FUN_00423540(local_220);
          local_224 = local_30 + 6;
          bVar1 = FUN_004235c0(local_224,800);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 8:
          FUN_00447de0(&local_c);
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_228 = (float)fVar8;
          FUN_00452f20(&local_c,(local_228 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056fa44);
          fVar8 = FUN_004292e0(&local_c);
          local_22c = (float)fVar8;
          if (DAT_0056e0f0 <= local_22c) {
            FUN_00472060(&local_c,DAT_0056e0f0 * DAT_0056c8d0);
          }
          fVar8 = FUN_004292e0(&local_c);
          fVar8 = FUN_00439820(extraout_ECX_25,(float)fVar8);
          local_230 = (float)fVar8;
          local_30[0x90] = local_230 * DAT_0056edd4;
          fVar8 = FUN_004292e0(&local_c);
          fVar8 = FUN_00439820(extraout_ECX_26,(float)fVar8);
          local_234 = (float)fVar8;
          local_30[0x91] = local_234 * _DAT_0056fa58;
          fVar8 = FUN_004292e0(&local_c);
          fVar8 = FUN_00439820(extraout_ECX_27,(float)fVar8);
          local_238 = (float)fVar8;
          local_30[0x87] = (float)((uint)local_238 ^ _DAT_0056d7d0) * DAT_0056c8cc;
          fVar8 = FUN_004292e0(&local_c);
          fVar8 = FUN_004397c0(extraout_ECX_28,(float)fVar8);
          local_23c = (float)fVar8;
          local_30[0x88] = local_23c * DAT_0056c8cc;
          local_240 = local_30 + 6;
          FUN_00423540(local_240);
          local_244 = local_30 + 6;
          bVar1 = FUN_004235c0(local_244,2000);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 10:
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_1a8 = (float)fVar8;
          local_4c = (local_1a8 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056fa48 - DAT_0056e0f0;
          fVar8 = FUN_00439820(extraout_ECX_11,local_4c);
          local_1ac = (float)fVar8;
          local_30[0x90] = local_1ac * DAT_0056fa2c;
          fVar8 = FUN_00438540(local_4c * DAT_0056c8d0);
          fVar8 = FUN_00439820(extraout_ECX_12,(float)fVar8);
          local_1b0 = (float)fVar8;
          local_30[0x92] = local_1b0 * DAT_0056ec98;
          fVar8 = FUN_00439820(local_30,local_4c);
          local_1b4 = (float)fVar8;
          local_30[0x87] = (float)((uint)local_1b4 ^ _DAT_0056d7d0) * DAT_0056ef28;
          local_1b8 = local_30 + 6;
          FUN_00423540(local_1b8);
          local_1bc = local_30 + 6;
          bVar1 = FUN_004235c0(local_1bc,0x800);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 0xb:
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_1c0 = (float)fVar8;
          local_6c = (local_1c0 * DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056cdb4 - DAT_0056e0f0;
          fVar8 = FUN_00439820(extraout_ECX_13,local_6c);
          local_1c4 = (float)fVar8;
          local_30[0x90] = local_1c4 * _DAT_0056fa68;
          fVar8 = FUN_00439820(extraout_ECX_14,local_6c);
          local_1c8 = (float)fVar8;
          local_30[0x87] = (float)((uint)local_1c8 ^ _DAT_0056d7d0) * DAT_0056ef28;
          local_1cc = local_30 + 6;
          FUN_00423540(local_1cc);
          local_1d0 = local_30 + 6;
          bVar1 = FUN_004235c0(local_1d0,0x400);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
          break;
        case 0xc:
          fVar8 = FUN_00423bd0((int)(local_30 + 6));
          local_1d4 = (float)fVar8;
          local_70 = (local_1d4 * DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056f114 - DAT_0056e0f0;
          fVar8 = FUN_00439820(extraout_ECX_15,local_70);
          local_1d8 = (float)fVar8;
          local_30[0x90] = local_1d8 * _DAT_0056fa58;
          fVar8 = FUN_00439820(extraout_ECX_16,local_70);
          local_1dc = (float)fVar8;
          local_30[0x87] = (float)((uint)local_1dc ^ _DAT_0056d7d0) * DAT_0056e1a0;
          local_1e0 = local_30 + 6;
          FUN_00423540(local_1e0);
          local_1e4 = local_30 + 6;
          bVar1 = FUN_004235c0(local_1e4,0x200);
          if (bVar1) {
            FUN_00423520(local_30 + 6,0);
          }
        }
      }
      FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
      return;
    }
    local_50 = (int)(short)local_34[1];
    switch(local_50) {
    case 0:
      goto LAB_004760cb;
    case 1:
      FUN_00423520(local_30,local_34[3]);
      local_30[4] = local_34[2];
      local_34 = (int *)FUN_00471160(*(int *)(local_30[0xdc] + 0x3330),local_30[4]);
      goto LAB_004751ea;
    case 2:
      local_30[0xd2] = local_30[0x81];
      local_30[0xd3] = local_30[0x82];
      local_30[0xd4] = local_30[0x83];
      local_30[0x81] = local_34[2];
      local_30[0x82] = local_34[3];
      local_30[0x83] = local_34[4];
      local_248 = local_30 + 0x81;
      puVar3 = (undefined4 *)FUN_00429440(local_248,local_2c0,(float *)(local_30 + 0xd2));
      local_30[0xd2] = *puVar3;
      local_30[0xd3] = puVar3[1];
      local_30[0xd4] = puVar3[2];
      break;
    case 3:
      local_74 = local_30 + 0x23;
      FUN_00438e70(local_74,local_34[2]);
      local_78 = local_30 + 0x23;
      FUN_004394f0(local_78,local_34[3]);
      local_7c = local_30 + 0x23;
      FUN_004393f0(local_7c,local_30 + 0x81);
      local_80 = local_30 + 0x23;
      FUN_00422dd0(local_2b4,local_34[4],local_34[5],local_34[6]);
      FUN_00438a90(local_80,local_2b4);
      FUN_004396a0((int)(local_30 + 0x23));
      break;
    case 4:
      local_30[0x84] = local_34[2];
      local_30[0x85] = local_34[3];
      local_30[0x86] = local_34[4];
      break;
    case 5:
      local_84 = local_30 + 0xe;
      FUN_00438e70(local_84,local_34[2]);
      local_88 = local_30 + 0xe;
      FUN_004394f0(local_88,local_34[3]);
      local_8c = local_30 + 0xe;
      FUN_004393f0(local_8c,local_30 + 0x84);
      local_90 = local_30 + 0xe;
      FUN_00422dd0(local_254,local_34[4],local_34[5],local_34[6]);
      FUN_00438a90(local_90,local_254);
      FUN_004396a0((int)(local_30 + 0xe));
      break;
    case 6:
      local_30[0x87] = local_34[2];
      local_30[0x88] = local_34[3];
      local_30[0x89] = local_34[4];
      break;
    case 7:
      local_30[0x96] = local_34[2];
      break;
    case 8:
      local_30[0xdb] = local_34[2];
      local_30[0xd7] = (float)*(byte *)(local_30 + 0xdb);
      local_30[0xd8] = (float)*(byte *)((int)local_30 + 0x36d);
      local_30[0xd9] = (float)*(byte *)((int)local_30 + 0x36e);
      local_30[0xda] = (float)*(byte *)((int)local_30 + 0x36f);
      local_30[0xd5] = local_34[3];
      local_30[0xd6] = local_34[4];
      break;
    case 9:
      FUN_004718d0(local_2c,local_34[5],local_34[6],(float)*(byte *)(local_34 + 4),
                   (float)*(byte *)((int)local_34 + 0x11),(float)*(byte *)((int)local_34 + 0x12),
                   (float)*(byte *)((int)local_34 + 0x13));
      FUN_004776c0(local_30,local_34[2],local_34[3],local_2c);
      break;
    case 10:
      local_b4 = local_30 + 0x23;
      FUN_00438e70(local_b4,local_34[2]);
      local_b8 = local_30 + 0x23;
      FUN_004394f0(local_b8,8);
      local_bc = local_30 + 0x23;
      FUN_004393f0(local_bc,local_30 + 0x81);
      local_c0 = local_30 + 0x23;
      FUN_00422dd0(local_26c,local_34[4],local_34[5],local_34[6]);
      FUN_00439480(local_c0,local_26c);
      local_c4 = local_30 + 0x23;
      FUN_00422dd0(local_278,local_34[7],local_34[8],local_34[9]);
      FUN_00438a90(local_c4,local_278);
      local_c8 = local_30 + 0x23;
      FUN_00422dd0(local_284,local_34[10],local_34[0xb],local_34[0xc]);
      FUN_00438b20(local_c8,local_284);
      FUN_004396a0((int)(local_30 + 0x23));
      break;
    case 0xb:
      local_cc = local_30 + 0xe;
      FUN_00438e70(local_cc,local_34[2]);
      local_d0 = local_30 + 0xe;
      FUN_004394f0(local_d0,8);
      local_d4 = local_30 + 0xe;
      FUN_004393f0(local_d4,local_30 + 0x84);
      local_d8 = local_30 + 0xe;
      FUN_00422dd0(local_290,local_34[4],local_34[5],local_34[6]);
      FUN_00439480(local_d8,local_290);
      local_dc = local_30 + 0xe;
      FUN_00422dd0(local_29c,local_34[7],local_34[8],local_34[9]);
      FUN_00438a90(local_dc,local_29c);
      local_e0 = local_30 + 0xe;
      FUN_00422dd0(local_2a8,local_34[10],local_34[0xb],local_34[0xc]);
      FUN_00438b20(local_e0,local_2a8);
      FUN_004396a0((int)(local_30 + 0xe));
      break;
    case 0xc:
      *(char *)(local_30 + 5) = (char)local_34[2];
      if (*(char *)(local_30 + 5) == '\0') {
        local_30[0x90] = 0;
        local_30[0x91] = 0;
        local_30[0x92] = 0;
      }
      if (*(char *)(local_30 + 5) == '\x06') {
        FUN_00423520(local_30 + 10,0);
      }
      else {
        FUN_00423520(local_30 + 6,0);
      }
      if (*(char *)(local_30 + 5) == '\x02') {
        FUN_00423520(local_30 + 6,0x200);
      }
      break;
    case 0xd:
      DAT_005c5b20 = local_34[2];
      break;
    case 0xe:
      if (local_34[3] < 0) {
        if (local_34[3] == -2) {
          FUN_00472a10((int)(local_30 + local_34[2] * 0x179 + 0xdd));
        }
        else if (local_34[3] == -1) {
          FUN_0044fd00((int)(local_30 + local_34[2] * 0x179 + 0xdd));
        }
      }
      else {
        FUN_0044c240((int)(local_30 + local_34[2] * 0x179 + 0xdd));
        local_e4 = *(void **)(local_30[0xdc] + 0x3334);
        FUN_00438380(local_e4,local_30 + local_34[2] * 0x179 + 0xdd,local_34[3]);
      }
      local_30[local_34[2] + 0xca5] = local_34[4];
      break;
    case 0x11:
      local_30[0xcc2] = local_34[2];
      for (local_38 = 0; local_38 < 2; local_38 = local_38 + 1) {
        FUN_00471210(local_30 + local_38 + 0xcae);
        local_30[local_38 + 0xcae] = 0;
        local_30[local_38 + 0xcb0] = DAT_0056f090;
        local_30[local_38 + 0xcb2] = DAT_0056e050;
        local_30[local_38 + 0xcb4] = 0xffffffff;
        FUN_00452f20(local_30 + local_38 + 0xcbe,0.0);
        FUN_00452f20(local_30 + local_38 + 0xcc0,0.0);
        FUN_00423520(local_30 + local_38 * 4 + 0xcb6,0);
        FUN_004776a0(DAT_005c0028,local_38);
        if (_DAT_0056e0e8 < (float)local_30[local_38 + 0xcb0]) {
          if (local_30[0xcc2] == 1) {
            piVar4 = FUN_004712d0();
            local_30[local_38 + 0xcae] = piVar4;
          }
          else {
            piVar4 = FUN_004712d0();
            local_30[local_38 + 0xcae] = piVar4;
          }
        }
      }
      break;
    case 0x12:
      local_94 = local_30 + 0x38;
      FUN_00438e70(local_94,local_34[2]);
      local_98 = local_30 + 0x38;
      FUN_004394f0(local_98,local_34[3]);
      local_9c = local_30 + 0x38;
      FUN_004393f0(local_9c,local_30 + 0x87);
      local_a0 = local_30 + 0x38;
      FUN_00422dd0(local_260,local_34[4],local_34[5],local_34[6]);
      FUN_00438a90(local_a0,local_260);
      FUN_004396a0((int)(local_30 + 0x38));
      break;
    case 0x13:
      local_e8 = (void *)local_30[0xdc];
      FUN_00477480(local_e8,local_34[2] + 7);
      break;
    case 0x14:
      local_30[0xcad] = (float)local_34[2] * (float)local_34[2];
      break;
    case 0x15:
      local_a4 = local_30 + 0x76;
      FUN_0041df50(local_a4,local_34[2]);
      local_a8 = local_30 + 0x76;
      FUN_004394d0(local_a8,local_34[3]);
      local_ac = local_30 + 0x76;
      FUN_004393d0(local_ac,local_30 + 0x96);
      local_b0 = local_30 + 0x76;
      FUN_00438a70(local_b0,local_34 + 4);
      FUN_004395b0((int)(local_30 + 0x76));
      break;
    case 0x16:
      FUN_00477790(DAT_005c069c,local_34[2]);
    }
    local_30[4] = (int)*(short *)((int)local_34 + 6) + local_30[4];
    local_34 = (int *)FUN_00471160(*(int *)(local_30[0xdc] + 0x3330),local_30[4]);
  } while( true );
}

