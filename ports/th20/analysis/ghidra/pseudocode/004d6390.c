/* Entry: 0x004d6390; symbol: FUN_004d6390; body bytes: 1091 */

void __thiscall
FUN_004d6390(void *this,undefined4 *param_1,float *param_2,float *param_3,float param_4)

{
  undefined4 *puVar1;
  float *pfVar2;
  void *this_00;
  float10 fVar3;
  float fVar4;
  undefined1 local_f0 [12];
  undefined1 local_e4 [12];
  undefined1 local_d8 [12];
  undefined1 local_cc [12];
  undefined1 local_c0 [12];
  undefined1 local_b4 [12];
  undefined1 local_a8 [12];
  undefined1 local_9c [12];
  undefined1 local_90 [12];
  float local_84;
  float local_80;
  float local_7c;
  void *local_78;
  void *local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  float local_60;
  void *local_5c;
  int local_58;
  int local_54;
  float local_50;
  float local_4c;
  void *local_48;
  float local_44 [2];
  undefined4 local_3c;
  float local_38 [2];
  undefined4 local_30;
  float local_2c [2];
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  fVar4 = param_4 - *(float *)((int)this + 8);
  local_54 = *(int *)((int)this + 0x10);
  local_48 = this;
  if (local_54 == 0) {
    local_68 = (void *)((int)this + 0x20);
    local_5c = (void *)((int)this + 0x14);
    local_64 = FUN_004292f0(local_5c,local_90,fVar4);
    local_60 = *(float *)((int)local_48 + 0x30);
    pfVar2 = (float *)FUN_004292f0(local_64,local_9c,local_60);
    puVar1 = (undefined4 *)FUN_00429570(local_68,local_a8,pfVar2);
    *param_1 = *puVar1;
    param_1[1] = puVar1[1];
    param_1[2] = puVar1[2];
    *param_2 = *(float *)((int)local_48 + 0x30);
    fVar3 = FUN_004292e0((float *)((int)local_48 + 0x2c));
    *param_3 = (float)fVar3;
  }
  else if (local_54 == 1) {
    if (DAT_0056fd74 < *(float *)((int)this + 0x38) || DAT_0056fd74 == *(float *)((int)this + 0x38))
    {
      local_20 = *(undefined4 *)((int)this + 0x20);
      local_1c = *(undefined4 *)((int)this + 0x24);
      local_18 = *(undefined4 *)((int)this + 0x28);
      FUN_00422e10(local_2c);
      FUN_00422e10(local_44);
      local_24 = 0;
      local_3c = 0;
      local_7c = *(float *)((int)local_48 + 0x30);
      fVar3 = FUN_004292e0((float *)((int)local_48 + 0x2c));
      local_80 = (float)fVar3;
      FUN_00439330(local_2c,local_2c,local_80,local_7c);
      FUN_00439330(local_44,local_44,*(float *)((int)local_48 + 0x38),
                   *(float *)((int)local_48 + 0x34));
      FUN_004296e0(local_2c,local_44);
      pfVar2 = (float *)FUN_004292f0(local_2c,local_e4,fVar4);
      FUN_004296e0(&local_20,pfVar2);
      *param_1 = local_20;
      param_1[1] = local_1c;
      param_1[2] = local_18;
      fVar3 = (float10)FUN_0047a2a0(local_2c);
      *param_2 = (float)fVar3;
      fVar3 = (float10)FUN_00456210(local_2c);
      *param_3 = (float)fVar3;
    }
    else {
      local_78 = (void *)((int)this + 0x20);
      local_6c = (void *)((int)this + 0x14);
      local_70 = FUN_004292f0(local_6c,local_b4,
                              fVar4 * *(float *)((int)this + 0x34) +
                              *(float *)((int)this + 0x30) * DAT_0056c8d0);
      local_74 = FUN_004292f0(local_70,local_c0,fVar4 + DAT_0056c8cc);
      pfVar2 = (float *)FUN_00452ff0(local_74,local_cc,DAT_0056c8d0);
      puVar1 = (undefined4 *)FUN_00429570(local_78,local_d8,pfVar2);
      *param_1 = *puVar1;
      param_1[1] = puVar1[1];
      param_1[2] = puVar1[2];
      *param_2 = *(float *)((int)local_48 + 0x34) * fVar4 + *(float *)((int)local_48 + 0x30);
      fVar3 = FUN_004292e0((float *)((int)local_48 + 0x2c));
      *param_3 = (float)fVar3;
    }
  }
  else if (local_54 == 2) {
    local_14 = *(undefined4 *)((int)this + 0x20);
    local_10 = *(undefined4 *)((int)this + 0x24);
    local_c = *(undefined4 *)((int)this + 0x28);
    FUN_00422e10(local_38);
    local_30 = 0;
    fVar3 = FUN_004292e0((float *)((int)local_48 + 0x2c));
    local_50 = (float)fVar3;
    local_4c = *(float *)((int)local_48 + 0x30);
    for (local_58 = 0; this_00 = (void *)(int)fVar4, local_58 < (int)this_00;
        local_58 = local_58 + 1) {
      FUN_00439330(this_00,local_38,local_50,local_4c);
      fVar3 = FUN_00438540(local_50 + *(float *)((int)local_48 + 0x38));
      local_50 = (float)fVar3;
      local_4c = local_4c + *(float *)((int)local_48 + 0x34);
      FUN_004296e0(&local_14,local_38);
    }
    FUN_00439330(this_00,local_38,local_50,local_4c);
    fVar3 = FUN_004592b0(fVar4);
    local_84 = (float)fVar3;
    pfVar2 = (float *)FUN_004292f0(local_38,local_f0,fVar4 - local_84);
    FUN_004296e0(&local_14,pfVar2);
    *param_1 = local_14;
    param_1[1] = local_10;
    param_1[2] = local_c;
    *param_2 = local_4c;
    *param_3 = local_50;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

