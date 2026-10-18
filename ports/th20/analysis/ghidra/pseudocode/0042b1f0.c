/* Entry: 0x0042b1f0; symbol: FUN_0042b1f0; body bytes: 919 */

float * __thiscall FUN_0042b1f0(void *this,float *param_1)

{
  bool bVar1;
  float *pfVar2;
  float10 fVar3;
  undefined1 local_9c [4];
  undefined1 local_98 [4];
  undefined1 local_94 [4];
  undefined1 local_90 [4];
  undefined1 local_8c [4];
  undefined1 local_88 [4];
  undefined1 local_84 [4];
  undefined1 local_80 [4];
  undefined1 local_7c [4];
  undefined1 local_78 [4];
  undefined1 local_74 [4];
  undefined1 local_70 [4];
  undefined1 local_6c [4];
  void *local_68;
  void *local_64;
  float local_60;
  float *local_5c;
  void *local_58;
  float *local_54;
  float *local_50;
  float local_4c;
  void *local_48;
  float *local_44;
  float *local_40;
  float local_3c;
  void *local_38;
  float *local_34;
  float *local_30;
  float local_2c;
  float *local_28;
  float local_24;
  float local_20;
  float *local_1c;
  float local_18;
  float local_14;
  float *local_10;
  float local_c;
  float *local_8;
  
  local_8 = (float *)this;
  if (*(int *)((int)this + 0x24) < 1) {
    if (*(int *)((int)this + 0x24) == 0) {
      if ((*(int *)((int)this + 0x28) != 7) && (*(int *)((int)this + 0x28) != 0x11)) {
        *param_1 = *(float *)((int)this + 4);
        return param_1;
      }
                    /* WARNING: Load size is inaccurate */
      *param_1 = *this;
      return param_1;
    }
  }
  else {
    FUN_00429400((undefined4 *)((int)this + 0x14));
    local_10 = local_8 + 5;
    bVar1 = FUN_004235c0(local_10,(int)local_8[9]);
    if (bVar1) {
      FUN_00423520(local_8 + 5,(int)local_8[9]);
      FUN_00429970((int)local_8);
      if ((local_8[10] != 9.80909e-45) && (local_8[10] != 2.38221e-44)) {
        *param_1 = local_8[1];
        return param_1;
      }
      *param_1 = *local_8;
      return param_1;
    }
  }
  if (local_8[10] == 9.80909e-45) {
    local_14 = *local_8;
    pfVar2 = (float *)FUN_00429610(&local_14,local_6c,local_8 + 1);
    *local_8 = *pfVar2;
    local_8[4] = *local_8;
  }
  else if (local_8[10] == 2.38221e-44) {
    local_18 = *local_8;
    pfVar2 = (float *)FUN_00429610(&local_18,local_70,local_8 + 3);
    *local_8 = *pfVar2;
    local_1c = local_8 + 3;
    pfVar2 = (float *)FUN_00429610(local_1c,local_74,local_8 + 1);
    local_8[3] = *pfVar2;
    local_8[4] = *local_8;
  }
  else if (local_8[10] == 1.12104e-44) {
    fVar3 = FUN_00423bd0((int)(local_8 + 5));
    local_20 = (float)fVar3;
    local_c = local_20 / (float)(int)local_8[9];
    local_24 = (local_c - DAT_0056c8cc) * (local_c - DAT_0056c8cc) *
               (DAT_0056c8d0 * local_c + DAT_0056c8cc);
    local_2c = local_c * local_c * (DAT_0056c8d8 - DAT_0056c8d0 * local_c);
    local_3c = (DAT_0056c8cc - local_c) * (DAT_0056c8cc - local_c) * local_c;
    local_4c = (local_c - DAT_0056c8cc) * local_c * local_c;
    local_28 = local_8;
    local_38 = FUN_00429390(local_8,local_78,local_24);
    local_30 = local_8 + 1;
    local_34 = (float *)FUN_00429390(local_30,local_7c,local_2c);
    local_48 = FUN_00429610(local_38,local_80,local_34);
    local_40 = local_8 + 2;
    local_44 = (float *)FUN_00429390(local_40,local_84,local_3c);
    local_58 = FUN_00429610(local_48,local_88,local_44);
    local_50 = local_8 + 3;
    local_54 = (float *)FUN_00429390(local_50,local_8c,local_4c);
    pfVar2 = (float *)FUN_00429610(local_58,local_90,local_54);
    local_8[4] = *pfVar2;
  }
  else {
    fVar3 = (float10)FUN_00437540((int)local_8);
    local_60 = (float)fVar3;
    local_5c = local_8 + 1;
    local_64 = FUN_004294e0(local_5c,local_94,local_8);
    local_68 = FUN_00429390(local_64,local_98,local_60);
    pfVar2 = (float *)FUN_00429610(local_68,local_9c,local_8);
    local_8[4] = *pfVar2;
  }
  *param_1 = local_8[4];
  return param_1;
}

