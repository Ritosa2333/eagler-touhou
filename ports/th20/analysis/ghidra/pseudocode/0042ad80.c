/* Entry: 0x0042ad80; symbol: FUN_0042ad80; body bytes: 1125 */

float * __thiscall FUN_0042ad80(void *this,float *param_1)

{
  bool bVar1;
  float *pfVar2;
  float10 fVar3;
  undefined1 local_114 [12];
  undefined1 local_108 [12];
  undefined1 local_fc [12];
  undefined1 local_f0 [12];
  undefined1 local_e4 [12];
  undefined1 local_d8 [12];
  undefined1 local_cc [12];
  undefined1 local_c0 [12];
  undefined1 local_b4 [12];
  undefined1 local_a8 [12];
  undefined1 local_9c [12];
  undefined1 local_90 [12];
  undefined1 local_84 [12];
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  void *local_60;
  void *local_5c;
  float local_58;
  float *local_54;
  void *local_50;
  float *local_4c;
  float *local_48;
  float local_44;
  void *local_40;
  float *local_3c;
  float *local_38;
  float local_34;
  void *local_30;
  float *local_2c;
  float *local_28;
  float local_24;
  float *local_20;
  float local_1c;
  float local_18;
  float *local_14;
  float *local_10;
  float local_c;
  float *local_8;
  
  local_8 = (float *)this;
  if (*(int *)((int)this + 0x4c) < 1) {
    if (*(int *)((int)this + 0x4c) == 0) {
      if ((*(int *)((int)this + 0x50) != 7) && (*(int *)((int)this + 0x50) != 0x11)) {
        *param_1 = *(float *)((int)this + 0xc);
        param_1[1] = *(float *)((int)this + 0x10);
        param_1[2] = *(float *)((int)this + 0x14);
        return param_1;
      }
                    /* WARNING: Load size is inaccurate */
      *param_1 = *this;
      param_1[1] = *(float *)((int)this + 4);
      param_1[2] = *(float *)((int)this + 8);
      return param_1;
    }
  }
  else {
    FUN_00429400((undefined4 *)((int)this + 0x3c));
    local_10 = local_8 + 0xf;
    bVar1 = FUN_004235c0(local_10,(int)local_8[0x13]);
    if (bVar1) {
      FUN_00423520(local_8 + 0xf,(int)local_8[0x13]);
      FUN_00429990((int)local_8);
      if ((local_8[0x14] != 9.80909e-45) && (local_8[0x14] != 2.38221e-44)) {
        *param_1 = local_8[3];
        param_1[1] = local_8[4];
        param_1[2] = local_8[5];
        return param_1;
      }
      *param_1 = *local_8;
      param_1[1] = local_8[1];
      param_1[2] = local_8[2];
      return param_1;
    }
  }
  if (local_8[0x14] == 9.80909e-45) {
    local_6c = *local_8;
    local_68 = local_8[1];
    local_64 = local_8[2];
    pfVar2 = (float *)FUN_00429570(&local_6c,local_84,local_8 + 3);
    *local_8 = *pfVar2;
    local_8[1] = pfVar2[1];
    local_8[2] = pfVar2[2];
    local_8[0xc] = *local_8;
    local_8[0xd] = local_8[1];
    local_8[0xe] = local_8[2];
  }
  else if (local_8[0x14] == 2.38221e-44) {
    local_78 = *local_8;
    local_74 = local_8[1];
    local_70 = local_8[2];
    pfVar2 = (float *)FUN_00429570(&local_78,local_90,local_8 + 9);
    *local_8 = *pfVar2;
    local_8[1] = pfVar2[1];
    local_8[2] = pfVar2[2];
    local_14 = local_8 + 9;
    pfVar2 = (float *)FUN_00429570(local_14,local_9c,local_8 + 3);
    local_8[9] = *pfVar2;
    local_8[10] = pfVar2[1];
    local_8[0xb] = pfVar2[2];
    local_8[0xc] = *local_8;
    local_8[0xd] = local_8[1];
    local_8[0xe] = local_8[2];
  }
  else if (local_8[0x14] == 1.12104e-44) {
    fVar3 = FUN_00423bd0((int)(local_8 + 0xf));
    local_18 = (float)fVar3;
    local_c = local_18 / (float)(int)local_8[0x13];
    local_1c = (local_c - DAT_0056c8cc) * (local_c - DAT_0056c8cc) *
               (DAT_0056c8d0 * local_c + DAT_0056c8cc);
    local_24 = local_c * local_c * (DAT_0056c8d8 - DAT_0056c8d0 * local_c);
    local_34 = (DAT_0056c8cc - local_c) * (DAT_0056c8cc - local_c) * local_c;
    local_44 = (local_c - DAT_0056c8cc) * local_c * local_c;
    local_20 = local_8;
    local_30 = FUN_004292f0(local_8,local_a8,local_1c);
    local_28 = local_8 + 3;
    local_2c = (float *)FUN_004292f0(local_28,local_b4,local_24);
    local_40 = FUN_00429570(local_30,local_c0,local_2c);
    local_38 = local_8 + 6;
    local_3c = (float *)FUN_004292f0(local_38,local_cc,local_34);
    local_50 = FUN_00429570(local_40,local_d8,local_3c);
    local_48 = local_8 + 9;
    local_4c = (float *)FUN_004292f0(local_48,local_e4,local_44);
    pfVar2 = (float *)FUN_00429570(local_50,local_f0,local_4c);
    local_8[0xc] = *pfVar2;
    local_8[0xd] = pfVar2[1];
    local_8[0xe] = pfVar2[2];
  }
  else {
    fVar3 = (float10)FUN_004375a0((int)local_8);
    local_58 = (float)fVar3;
    local_54 = local_8 + 3;
    local_5c = FUN_00429440(local_54,local_fc,local_8);
    local_60 = FUN_004292f0(local_5c,local_108,local_58);
    pfVar2 = (float *)FUN_00429570(local_60,local_114,local_8);
    local_8[0xc] = *pfVar2;
    local_8[0xd] = pfVar2[1];
    local_8[0xe] = pfVar2[2];
  }
  *param_1 = local_8[0xc];
  param_1[1] = local_8[0xd];
  param_1[2] = local_8[0xe];
  return param_1;
}

