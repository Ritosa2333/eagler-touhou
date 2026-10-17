/* Entry: 0x004a8d70; symbol: FUN_004a8d70; body bytes: 2424 */

/* WARNING: Variable defined which should be unmapped: param_2 */

undefined4 * __thiscall FUN_004a8d70(undefined4 *param_1,undefined4 *param_2)

{
  bool bVar1;
  undefined4 *puVar2;
  float *pfVar3;
  float *pfVar4;
  float10 fVar5;
  undefined1 local_1b8 [12];
  undefined1 local_1ac [12];
  undefined1 local_1a0 [12];
  undefined1 local_194 [12];
  undefined1 local_188 [12];
  undefined1 local_17c [12];
  undefined1 local_170 [12];
  undefined1 local_164 [12];
  undefined1 local_158 [12];
  undefined1 local_14c [12];
  undefined1 local_140 [12];
  undefined1 local_134 [12];
  undefined1 local_128 [12];
  undefined4 local_11c;
  undefined4 local_118;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  float local_104;
  undefined4 *local_100;
  float local_fc;
  undefined4 *local_f8;
  float local_f4;
  undefined4 *local_f0;
  undefined4 *local_ec;
  float local_e8;
  undefined4 *local_e4;
  float local_e0;
  float local_dc;
  float local_d8;
  float local_d4;
  undefined4 *local_d0;
  float local_cc;
  undefined4 *local_c8;
  float local_c4;
  undefined4 *local_c0;
  float local_bc;
  undefined4 *local_b8;
  float local_b4;
  undefined4 local_b0;
  undefined4 *local_ac;
  undefined4 *local_a8;
  float local_a4;
  undefined4 *local_a0;
  undefined4 *local_9c;
  undefined4 *local_98;
  float local_94;
  undefined4 *local_90;
  undefined4 *local_8c;
  undefined4 *local_88;
  undefined4 local_84;
  undefined4 *local_80;
  undefined4 *local_7c;
  float local_78;
  undefined4 *local_74;
  undefined4 *local_70;
  undefined4 *local_6c;
  void *local_68;
  void *local_64;
  float local_60;
  undefined4 *local_5c;
  void *local_58;
  float *local_54;
  undefined4 *local_50;
  float local_4c;
  void *local_48;
  float *local_44;
  undefined4 *local_40;
  float local_3c;
  void *local_38;
  float *local_34;
  undefined4 *local_30;
  float local_2c;
  undefined4 *local_28;
  float local_24;
  float local_20;
  undefined4 *local_1c;
  undefined4 *local_18;
  float local_14;
  float local_10;
  int local_c;
  undefined4 *local_8;
  
  local_8 = param_1;
  if ((int)param_1[0x13] < 1) {
    if (param_1[0x13] == 0) {
      if ((param_1[0x17] != 7) && (param_1[0x17] != 0x11)) {
        *param_2 = param_1[6];
        param_2[1] = param_1[7];
        param_2[2] = param_1[8];
        return param_2;
      }
      *param_2 = param_1[3];
      param_2[1] = param_1[4];
      param_2[2] = param_1[5];
      return param_2;
    }
  }
  else {
    FUN_00429400(param_1 + 0xf);
    local_18 = local_8 + 0xf;
    bVar1 = FUN_004235c0(local_18,local_8[0x13]);
    if (bVar1) {
      FUN_00423520(local_8 + 0xf,local_8[0x13]);
      FUN_00429990((int)local_8);
      if ((local_8[0x17] != 7) && (local_8[0x17] != 0x11)) {
        *param_2 = local_8[6];
        param_2[1] = local_8[7];
        param_2[2] = local_8[8];
        return param_2;
      }
      *param_2 = local_8[3];
      param_2[1] = local_8[4];
      param_2[2] = local_8[5];
      return param_2;
    }
  }
  if ((local_8[0x18] & 1) == 0) {
    if (local_8[0x17] == 7) {
      local_110 = local_8[3];
      local_10c = local_8[4];
      local_108 = local_8[5];
      puVar2 = (undefined4 *)FUN_00429570(&local_110,local_128,(float *)(local_8 + 6));
      local_8[3] = *puVar2;
      local_8[4] = puVar2[1];
      local_8[5] = puVar2[2];
      *local_8 = local_8[3];
      local_8[1] = local_8[4];
      local_8[2] = local_8[5];
    }
    else if (local_8[0x17] == 0x11) {
      local_11c = local_8[3];
      local_118 = local_8[4];
      local_114 = local_8[5];
      puVar2 = (undefined4 *)FUN_00429570(&local_11c,local_134,(float *)(local_8 + 0xc));
      local_8[3] = *puVar2;
      local_8[4] = puVar2[1];
      local_8[5] = puVar2[2];
      local_1c = local_8 + 0xc;
      puVar2 = (undefined4 *)FUN_00429570(local_1c,local_140,(float *)(local_8 + 6));
      local_8[0xc] = *puVar2;
      local_8[0xd] = puVar2[1];
      local_8[0xe] = puVar2[2];
      *local_8 = local_8[3];
      local_8[1] = local_8[4];
      local_8[2] = local_8[5];
    }
    else if (local_8[0x17] == 8) {
      fVar5 = FUN_00423bd0((int)(local_8 + 0xf));
      local_20 = (float)fVar5;
      local_10 = local_20 / (float)(int)local_8[0x13];
      local_24 = (local_10 - DAT_0056c8cc) * (local_10 - DAT_0056c8cc) *
                 (DAT_0056c8d0 * local_10 + DAT_0056c8cc);
      local_2c = local_10 * local_10 * (DAT_0056c8d8 - DAT_0056c8d0 * local_10);
      local_3c = (DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10) * local_10;
      local_4c = (local_10 - DAT_0056c8cc) * local_10 * local_10;
      local_28 = local_8 + 3;
      local_38 = FUN_004292f0(local_28,local_14c,local_24);
      local_30 = local_8 + 6;
      local_34 = (float *)FUN_004292f0(local_30,local_158,local_2c);
      local_48 = FUN_00429570(local_38,local_164,local_34);
      local_40 = local_8 + 9;
      local_44 = (float *)FUN_004292f0(local_40,local_170,local_3c);
      local_58 = FUN_00429570(local_48,local_17c,local_44);
      local_50 = local_8 + 0xc;
      local_54 = (float *)FUN_004292f0(local_50,local_188,local_4c);
      puVar2 = (undefined4 *)FUN_00429570(local_58,local_194,local_54);
      *local_8 = *puVar2;
      local_8[1] = puVar2[1];
      local_8[2] = puVar2[2];
    }
    else {
      fVar5 = (float10)FUN_004aab70((int)local_8);
      local_60 = (float)fVar5;
      local_5c = local_8 + 6;
      local_64 = FUN_00429440(local_5c,local_1a0,(float *)(local_8 + 3));
      local_68 = FUN_004292f0(local_64,local_1ac,local_60);
      puVar2 = (undefined4 *)FUN_00429570(local_68,local_1b8,(float *)(local_8 + 3));
      *local_8 = *puVar2;
      local_8[1] = puVar2[1];
      local_8[2] = puVar2[2];
    }
    *param_2 = *local_8;
    param_2[1] = local_8[1];
    param_2[2] = local_8[2];
  }
  else {
    for (local_c = 0; local_c < 3; local_c = local_c + 1) {
      if (local_8[local_c + 0x14] == 7) {
        local_70 = local_8 + 3;
        local_6c = local_8 + 6;
        pfVar3 = (float *)FUN_00414580(local_6c,local_c);
        pfVar4 = (float *)FUN_00414580(local_70,local_c);
        local_78 = *pfVar4 + *pfVar3;
        local_74 = local_8 + 3;
        pfVar3 = (float *)FUN_00414580(local_74,local_c);
        *pfVar3 = local_78;
        local_7c = local_8 + 3;
        puVar2 = (undefined4 *)FUN_00414580(local_7c,local_c);
        local_84 = *puVar2;
        local_80 = local_8;
        puVar2 = (undefined4 *)FUN_00414580(local_8,local_c);
        *puVar2 = local_84;
      }
      else if (local_8[local_c + 0x14] == 0x11) {
        local_8c = local_8 + 3;
        local_88 = local_8 + 0xc;
        pfVar3 = (float *)FUN_00414580(local_88,local_c);
        pfVar4 = (float *)FUN_00414580(local_8c,local_c);
        local_94 = *pfVar4 + *pfVar3;
        local_90 = local_8 + 3;
        pfVar3 = (float *)FUN_00414580(local_90,local_c);
        *pfVar3 = local_94;
        local_9c = local_8 + 0xc;
        local_98 = local_8 + 6;
        pfVar3 = (float *)FUN_00414580(local_98,local_c);
        pfVar4 = (float *)FUN_00414580(local_9c,local_c);
        local_a4 = *pfVar4 + *pfVar3;
        local_a0 = local_8 + 0xc;
        pfVar3 = (float *)FUN_00414580(local_a0,local_c);
        *pfVar3 = local_a4;
        local_a8 = local_8 + 3;
        puVar2 = (undefined4 *)FUN_00414580(local_a8,local_c);
        local_b0 = *puVar2;
        local_ac = local_8;
        puVar2 = (undefined4 *)FUN_00414580(local_8,local_c);
        *puVar2 = local_b0;
      }
      else if (local_8[local_c + 0x14] == 8) {
        fVar5 = FUN_00423bd0((int)(local_8 + 0xf));
        local_b4 = (float)fVar5;
        local_14 = local_b4 / (float)(int)local_8[0x13];
        local_d4 = (local_14 - DAT_0056c8cc) * (local_14 - DAT_0056c8cc) *
                   (DAT_0056c8d0 * local_14 + DAT_0056c8cc);
        local_cc = local_14 * local_14 * (DAT_0056c8d8 - DAT_0056c8d0 * local_14);
        local_c4 = (DAT_0056c8cc - local_14) * (DAT_0056c8cc - local_14) * local_14;
        local_bc = (local_14 - DAT_0056c8cc) * local_14 * local_14;
        local_d0 = local_8 + 3;
        local_c8 = local_8 + 6;
        local_c0 = local_8 + 9;
        local_b8 = local_8 + 0xc;
        pfVar3 = (float *)FUN_00414580(local_b8,local_c);
        local_e0 = *pfVar3 * local_bc;
        pfVar3 = (float *)FUN_00414580(local_c0,local_c);
        local_dc = *pfVar3 * local_c4;
        pfVar3 = (float *)FUN_00414580(local_c8,local_c);
        local_d8 = *pfVar3 * local_cc;
        pfVar3 = (float *)FUN_00414580(local_d0,local_c);
        local_e8 = *pfVar3 * local_d4 + local_d8 + local_dc + local_e0;
        local_e4 = local_8;
        pfVar3 = (float *)FUN_00414580(local_8,local_c);
        *pfVar3 = local_e8;
      }
      else {
        fVar5 = (float10)FUN_004aab10(local_8,local_c);
        local_f4 = (float)fVar5;
        local_f0 = local_8 + 6;
        local_ec = local_8 + 3;
        local_f8 = local_8 + 3;
        pfVar3 = (float *)FUN_00414580(local_ec,local_c);
        pfVar4 = (float *)FUN_00414580(local_f0,local_c);
        local_fc = (*pfVar4 - *pfVar3) * local_f4;
        pfVar3 = (float *)FUN_00414580(local_f8,local_c);
        local_104 = local_fc + *pfVar3;
        local_100 = local_8;
        pfVar3 = (float *)FUN_00414580(local_8,local_c);
        *pfVar3 = local_104;
      }
    }
    *param_2 = *local_8;
    param_2[1] = local_8[1];
    param_2[2] = local_8[2];
  }
  return param_2;
}

