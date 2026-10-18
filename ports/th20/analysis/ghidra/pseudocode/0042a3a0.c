/* Entry: 0x0042a3a0; symbol: FUN_0042a3a0; body bytes: 1485 */

int * __thiscall FUN_0042a3a0(void *this,int *param_1)

{
  bool bVar1;
  int *piVar2;
  float10 fVar3;
  undefined1 local_174 [12];
  undefined1 local_168 [12];
  undefined1 local_15c [12];
  undefined1 local_150 [12];
  undefined1 local_144 [12];
  undefined1 local_138 [12];
  undefined1 local_12c [12];
  undefined1 local_120 [12];
  undefined1 local_114 [12];
  undefined1 local_108 [12];
  undefined1 local_fc [12];
  undefined1 local_f0 [12];
  undefined1 local_e4 [12];
  int local_d8;
  int local_d4;
  int local_d0;
  int local_cc;
  int local_c8;
  int local_c4;
  int local_c0;
  int local_bc;
  int local_b8;
  int local_b0;
  int local_ac;
  int local_a8;
  int local_a0;
  int local_9c;
  int local_98;
  int local_90;
  int local_8c;
  int local_88;
  int local_80;
  int local_7c;
  int local_78;
  int local_70;
  int local_6c;
  int local_68;
  void *local_60;
  void *local_5c;
  float local_58;
  int *local_54;
  void *local_50;
  int *local_4c;
  float local_48;
  void *local_44;
  int *local_40;
  float local_3c;
  void *local_38;
  int *local_34;
  float local_30;
  int *local_2c;
  float local_28;
  float local_24;
  int *local_20;
  int *local_1c;
  float local_18;
  int *local_14;
  
  local_14 = (int *)this;
  if (*(int *)((int)this + 0x4c) < 1) {
    if (*(int *)((int)this + 0x4c) == 0) {
      if ((*(int *)((int)this + 0x50) != 7) && (*(int *)((int)this + 0x50) != 0x11)) {
        *param_1 = *(int *)((int)this + 0xc);
        param_1[1] = *(int *)((int)this + 0x10);
        param_1[2] = *(int *)((int)this + 0x14);
        return param_1;
      }
                    /* WARNING: Load size is inaccurate */
      *param_1 = *this;
      param_1[1] = *(int *)((int)this + 4);
      param_1[2] = *(int *)((int)this + 8);
      return param_1;
    }
  }
  else {
    FUN_00429400((undefined4 *)((int)this + 0x3c));
    local_1c = local_14 + 0xf;
    bVar1 = FUN_004235c0(local_1c,local_14[0x13]);
    if (bVar1) {
      FUN_00423520(local_14 + 0xf,local_14[0x13]);
      FUN_00429990((int)local_14);
      if ((local_14[0x14] != 7) && (local_14[0x14] != 0x11)) {
        *param_1 = local_14[3];
        param_1[1] = local_14[4];
        param_1[2] = local_14[5];
        return param_1;
      }
      *param_1 = *local_14;
      param_1[1] = local_14[1];
      param_1[2] = local_14[2];
      return param_1;
    }
  }
  if (local_14[0x14] == 7) {
    local_cc = *local_14;
    local_c8 = local_14[1];
    local_c4 = local_14[2];
    local_70 = local_14[3];
    local_6c = local_14[4];
    local_68 = local_14[5];
    piVar2 = (int *)FUN_004295d0(&local_cc,local_e4,local_70,local_6c,local_68);
    *local_14 = *piVar2;
    local_14[1] = piVar2[1];
    local_14[2] = piVar2[2];
    local_14[0xc] = *local_14;
    local_14[0xd] = local_14[1];
    local_14[0xe] = local_14[2];
  }
  else if (local_14[0x14] == 0x11) {
    local_d8 = *local_14;
    local_d4 = local_14[1];
    local_d0 = local_14[2];
    local_80 = local_14[9];
    local_7c = local_14[10];
    local_78 = local_14[0xb];
    piVar2 = (int *)FUN_004295d0(&local_d8,local_f0,local_80,local_7c,local_78);
    *local_14 = *piVar2;
    local_14[1] = piVar2[1];
    local_14[2] = piVar2[2];
    local_20 = local_14 + 9;
    piVar2 = (int *)FUN_004295d0(local_20,local_fc,local_14[3],local_14[4],local_14[5]);
    local_14[9] = *piVar2;
    local_14[10] = piVar2[1];
    local_14[0xb] = piVar2[2];
    local_14[0xc] = *local_14;
    local_14[0xd] = local_14[1];
    local_14[0xe] = local_14[2];
  }
  else if (local_14[0x14] == 8) {
    fVar3 = FUN_00423bd0((int)(local_14 + 0xf));
    local_24 = (float)fVar3;
    local_18 = local_24 / (float)local_14[0x13];
    local_28 = (local_18 - DAT_0056c8cc) * (local_18 - DAT_0056c8cc) *
               (DAT_0056c8d0 * local_18 + DAT_0056c8cc);
    local_30 = local_18 * local_18 * (DAT_0056c8d8 - DAT_0056c8d0 * local_18);
    local_3c = (DAT_0056c8cc - local_18) * (DAT_0056c8cc - local_18) * local_18;
    local_48 = (local_18 - DAT_0056c8cc) * local_18 * local_18;
    local_2c = local_14;
    local_38 = FUN_00429340(local_14,local_108,local_28);
    local_34 = local_14 + 3;
    piVar2 = (int *)FUN_00429340(local_34,local_114,local_30);
    local_90 = *piVar2;
    local_8c = piVar2[1];
    local_88 = piVar2[2];
    local_44 = FUN_004295d0(local_38,local_120,local_90,local_8c,local_88);
    local_40 = local_14 + 6;
    piVar2 = (int *)FUN_00429340(local_40,local_12c,local_3c);
    local_a0 = *piVar2;
    local_9c = piVar2[1];
    local_98 = piVar2[2];
    local_50 = FUN_004295d0(local_44,local_138,local_a0,local_9c,local_98);
    local_4c = local_14 + 9;
    piVar2 = (int *)FUN_00429340(local_4c,local_144,local_48);
    local_b0 = *piVar2;
    local_ac = piVar2[1];
    local_a8 = piVar2[2];
    piVar2 = (int *)FUN_004295d0(local_50,local_150,local_b0,local_ac,local_a8);
    local_14[0xc] = *piVar2;
    local_14[0xd] = piVar2[1];
    local_14[0xe] = piVar2[2];
  }
  else {
    fVar3 = (float10)FUN_004375a0((int)local_14);
    local_58 = (float)fVar3;
    local_54 = local_14 + 3;
    local_5c = FUN_004294a0(local_54,local_15c,*local_14,local_14[1],local_14[2]);
    local_60 = FUN_00429340(local_5c,local_168,local_58);
    local_c0 = *local_14;
    local_bc = local_14[1];
    local_b8 = local_14[2];
    piVar2 = (int *)FUN_004295d0(local_60,local_174,local_c0,local_bc,local_b8);
    local_14[0xc] = *piVar2;
    local_14[0xd] = piVar2[1];
    local_14[0xe] = piVar2[2];
  }
  *param_1 = local_14[0xc];
  param_1[1] = local_14[0xd];
  param_1[2] = local_14[0xe];
  return param_1;
}

