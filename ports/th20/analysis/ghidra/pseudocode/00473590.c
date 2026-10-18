/* Entry: 0x00473590; symbol: FUN_00473590; body bytes: 1070 */

float * __thiscall FUN_00473590(void *this,float *param_1)

{
  bool bVar1;
  float *pfVar2;
  int iVar3;
  float *pfVar4;
  float10 fVar5;
  undefined1 local_204 [28];
  undefined1 local_1e8 [28];
  undefined1 local_1cc [28];
  undefined1 local_1b0 [28];
  undefined1 local_194 [28];
  undefined1 local_178 [28];
  undefined1 local_15c [28];
  undefined1 local_140 [28];
  undefined1 local_124 [28];
  undefined1 local_108 [28];
  undefined1 local_ec [28];
  undefined1 local_d0 [28];
  undefined1 local_b4 [28];
  float local_98 [7];
  float local_7c [7];
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
  if (*(int *)((int)this + 0x9c) < 1) {
    if (*(int *)((int)this + 0x9c) == 0) {
      if ((*(int *)((int)this + 0xa0) != 7) && (*(int *)((int)this + 0xa0) != 0x11)) {
        pfVar2 = (float *)((int)this + 0x1c);
        pfVar4 = param_1;
        for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
          *pfVar4 = *pfVar2;
          pfVar2 = pfVar2 + 1;
          pfVar4 = pfVar4 + 1;
        }
        return param_1;
      }
      pfVar2 = param_1;
                    /* WARNING: Load size is inaccurate */
      for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
        *pfVar2 = *this;
        this = (float *)((int)this + 4);
        pfVar2 = pfVar2 + 1;
      }
      return param_1;
    }
  }
  else {
    FUN_00429400((undefined4 *)((int)this + 0x8c));
    local_10 = local_8 + 0x23;
    bVar1 = FUN_004235c0(local_10,(int)local_8[0x27]);
    if (bVar1) {
      FUN_00423520(local_8 + 0x23,(int)local_8[0x27]);
      FUN_00473140((int)local_8);
      if ((local_8[0x28] != 9.80909e-45) && (local_8[0x28] != 2.38221e-44)) {
        pfVar2 = local_8 + 7;
        pfVar4 = param_1;
        for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
          *pfVar4 = *pfVar2;
          pfVar2 = pfVar2 + 1;
          pfVar4 = pfVar4 + 1;
        }
        return param_1;
      }
      pfVar2 = param_1;
      for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
        *pfVar2 = *local_8;
        local_8 = local_8 + 1;
        pfVar2 = pfVar2 + 1;
      }
      return param_1;
    }
  }
  if (local_8[0x28] == 9.80909e-45) {
    pfVar2 = local_8;
    pfVar4 = local_7c;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
    pfVar2 = (float *)FUN_00471f50(local_7c,local_b4,local_8 + 7);
    pfVar4 = local_8;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
    pfVar2 = local_8;
    pfVar4 = local_8 + 0x1c;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
  }
  else if (local_8[0x28] == 2.38221e-44) {
    pfVar2 = local_8;
    pfVar4 = local_98;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
    pfVar2 = (float *)FUN_00471f50(local_98,local_d0,local_8 + 0x15);
    pfVar4 = local_8;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
    local_14 = local_8 + 0x15;
    pfVar2 = (float *)FUN_00471f50(local_14,local_ec,local_8 + 7);
    pfVar4 = local_8 + 0x15;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
    pfVar2 = local_8;
    pfVar4 = local_8 + 0x1c;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
  }
  else if (local_8[0x28] == 1.12104e-44) {
    fVar5 = FUN_00423bd0((int)(local_8 + 0x23));
    local_18 = (float)fVar5;
    local_c = local_18 / (float)(int)local_8[0x27];
    local_1c = (local_c - DAT_0056c8cc) * (local_c - DAT_0056c8cc) *
               (DAT_0056c8d0 * local_c + DAT_0056c8cc);
    local_24 = local_c * local_c * (DAT_0056c8d8 - DAT_0056c8d0 * local_c);
    local_34 = (DAT_0056c8cc - local_c) * (DAT_0056c8cc - local_c) * local_c;
    local_44 = (local_c - DAT_0056c8cc) * local_c * local_c;
    local_20 = local_8;
    local_30 = FUN_00471db0(local_8,local_108,local_1c);
    local_28 = local_8 + 7;
    local_2c = (float *)FUN_00471db0(local_28,local_124,local_24);
    local_40 = FUN_00471f50(local_30,local_140,local_2c);
    local_38 = local_8 + 0xe;
    local_3c = (float *)FUN_00471db0(local_38,local_15c,local_34);
    local_50 = FUN_00471f50(local_40,local_178,local_3c);
    local_48 = local_8 + 0x15;
    local_4c = (float *)FUN_00471db0(local_48,local_194,local_44);
    pfVar2 = (float *)FUN_00471f50(local_50,local_1b0,local_4c);
    pfVar4 = local_8 + 0x1c;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
  }
  else {
    fVar5 = (float10)FUN_00474ce0((int)local_8);
    local_58 = (float)fVar5;
    local_54 = local_8 + 7;
    local_5c = FUN_00471e60(local_54,local_1cc,local_8);
    local_60 = FUN_00471db0(local_5c,local_1e8,local_58);
    pfVar2 = (float *)FUN_00471f50(local_60,local_204,local_8);
    pfVar4 = local_8 + 0x1c;
    for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pfVar4 = *pfVar2;
      pfVar2 = pfVar2 + 1;
      pfVar4 = pfVar4 + 1;
    }
  }
  pfVar2 = local_8 + 0x1c;
  pfVar4 = param_1;
  for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pfVar4 = *pfVar2;
    pfVar2 = pfVar2 + 1;
    pfVar4 = pfVar4 + 1;
  }
  return param_1;
}

