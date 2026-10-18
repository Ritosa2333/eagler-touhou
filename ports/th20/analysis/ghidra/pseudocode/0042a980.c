/* Entry: 0x0042a980; symbol: FUN_0042a980; body bytes: 1018 */

float * __thiscall FUN_0042a980(void *this,float *param_1)

{
  float fVar1;
  bool bVar2;
  float *pfVar3;
  float10 fVar4;
  undefined1 local_d8 [8];
  undefined1 local_d0 [8];
  undefined1 local_c8 [8];
  undefined1 local_c0 [8];
  undefined1 local_b8 [8];
  undefined1 local_b0 [8];
  undefined1 local_a8 [8];
  undefined1 local_a0 [8];
  undefined1 local_98 [8];
  undefined1 local_90 [8];
  undefined1 local_88 [8];
  undefined1 local_80 [8];
  undefined1 local_78 [8];
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
  if (*(int *)((int)this + 0x38) < 1) {
    if (*(int *)((int)this + 0x38) == 0) {
      if ((*(int *)((int)this + 0x3c) != 7) && (*(int *)((int)this + 0x3c) != 0x11)) {
        fVar1 = *(float *)((int)this + 0xc);
        *param_1 = *(float *)((int)this + 8);
        param_1[1] = fVar1;
        return param_1;
      }
                    /* WARNING: Load size is inaccurate */
      fVar1 = *(float *)((int)this + 4);
      *param_1 = *this;
      param_1[1] = fVar1;
      return param_1;
    }
  }
  else {
    FUN_00429400((undefined4 *)((int)this + 0x28));
    local_10 = local_8 + 10;
    bVar2 = FUN_004235c0(local_10,(int)local_8[0xe]);
    if (bVar2) {
      FUN_00423520(local_8 + 10,(int)local_8[0xe]);
      FUN_004299b0((int)local_8);
      if ((local_8[0xf] != 9.80909e-45) && (local_8[0xf] != 2.38221e-44)) {
        fVar1 = local_8[3];
        *param_1 = local_8[2];
        param_1[1] = fVar1;
        return param_1;
      }
      fVar1 = local_8[1];
      *param_1 = *local_8;
      param_1[1] = fVar1;
      return param_1;
    }
  }
  if (local_8[0xf] == 9.80909e-45) {
    local_68 = *local_8;
    local_64 = local_8[1];
    pfVar3 = (float *)FUN_00429640(&local_68,local_78,local_8 + 2);
    fVar1 = pfVar3[1];
    *local_8 = *pfVar3;
    local_8[1] = fVar1;
    local_8[8] = *local_8;
    local_8[9] = local_8[1];
  }
  else if (local_8[0xf] == 2.38221e-44) {
    local_70 = *local_8;
    local_6c = local_8[1];
    pfVar3 = (float *)FUN_00429640(&local_70,local_80,local_8 + 6);
    fVar1 = pfVar3[1];
    *local_8 = *pfVar3;
    local_8[1] = fVar1;
    local_14 = local_8 + 6;
    pfVar3 = (float *)FUN_00429640(local_14,local_88,local_8 + 2);
    fVar1 = pfVar3[1];
    local_8[6] = *pfVar3;
    local_8[7] = fVar1;
    local_8[8] = *local_8;
    local_8[9] = local_8[1];
  }
  else if (local_8[0xf] == 1.12104e-44) {
    fVar4 = FUN_00423bd0((int)(local_8 + 10));
    local_18 = (float)fVar4;
    local_c = local_18 / (float)(int)local_8[0xe];
    local_1c = (local_c - DAT_0056c8cc) * (local_c - DAT_0056c8cc) *
               (DAT_0056c8d0 * local_c + DAT_0056c8cc);
    local_24 = local_c * local_c * (DAT_0056c8d8 - DAT_0056c8d0 * local_c);
    local_34 = (DAT_0056c8cc - local_c) * (DAT_0056c8cc - local_c) * local_c;
    local_44 = (local_c - DAT_0056c8cc) * local_c * local_c;
    local_20 = local_8;
    local_30 = FUN_004293c0(local_8,local_90,local_1c);
    local_28 = local_8 + 2;
    local_2c = (float *)FUN_004293c0(local_28,local_98,local_24);
    local_40 = FUN_00429640(local_30,local_a0,local_2c);
    local_38 = local_8 + 4;
    local_3c = (float *)FUN_004293c0(local_38,local_a8,local_34);
    local_50 = FUN_00429640(local_40,local_b0,local_3c);
    local_48 = local_8 + 6;
    local_4c = (float *)FUN_004293c0(local_48,local_b8,local_44);
    pfVar3 = (float *)FUN_00429640(local_50,local_c0,local_4c);
    fVar1 = pfVar3[1];
    local_8[8] = *pfVar3;
    local_8[9] = fVar1;
  }
  else {
    fVar4 = (float10)FUN_00437600((int)local_8);
    local_58 = (float)fVar4;
    local_54 = local_8 + 2;
    local_5c = FUN_00429520(local_54,local_c8,local_8);
    local_60 = FUN_004293c0(local_5c,local_d0,local_58);
    pfVar3 = (float *)FUN_00429640(local_60,local_d8,local_8);
    fVar1 = pfVar3[1];
    local_8[8] = *pfVar3;
    local_8[9] = fVar1;
  }
  fVar1 = local_8[9];
  *param_1 = local_8[8];
  param_1[1] = fVar1;
  return param_1;
}

