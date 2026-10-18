/* Entry: 0x004d6830; symbol: FUN_004d6830; body bytes: 843 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_004d6830(void *this,undefined4 *param_1,float *param_2,float *param_3,void *param_4,
            float param_5,float param_6,float param_7)

{
  float *pfVar1;
  undefined4 *puVar2;
  void *this_00;
  void *this_01;
  float10 fVar3;
  undefined1 local_b4 [12];
  undefined1 local_a8 [12];
  undefined1 local_9c [12];
  undefined1 local_90 [12];
  undefined1 local_84 [12];
  undefined1 local_78 [12];
  undefined1 local_6c [12];
  undefined1 local_60 [12];
  undefined1 local_54 [12];
  float local_48;
  float local_44;
  float local_40;
  void *local_3c;
  void *local_38;
  int local_34;
  void *local_30;
  float local_2c [2];
  undefined4 local_24;
  float local_20 [2];
  undefined4 local_18;
  float local_14 [2];
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_34 = *(int *)((int)this + 0x10);
  local_30 = this;
  if (local_34 == 0) {
    local_38 = (void *)((int)this + 0x14);
    pfVar1 = (float *)FUN_004292f0(local_38,local_54,*(float *)((int)this + 0x30));
    puVar2 = (undefined4 *)FUN_00429440(param_4,local_60,pfVar1);
    *param_1 = *puVar2;
    param_1[1] = puVar2[1];
    param_1[2] = puVar2[2];
    *param_2 = *(float *)((int)local_30 + 0x30);
    fVar3 = FUN_004292e0((float *)((int)local_30 + 0x2c));
    *param_3 = (float)fVar3;
  }
  else if (local_34 == 1) {
    if (DAT_0056fd74 < *(float *)((int)this + 0x38) || DAT_0056fd74 == *(float *)((int)this + 0x38))
    {
      FUN_00422e10(local_14);
      FUN_00422e10(local_2c);
      local_c = 0;
      local_24 = 0;
      FUN_00439330(this_00,local_14,param_6,(float)((uint)param_5 ^ _DAT_0056d7d0));
      FUN_00439330(local_30,local_2c,*(float *)((int)local_30 + 0x38),
                   (float)(*(uint *)((int)local_30 + 0x34) ^ _DAT_0056d7d0));
      FUN_004296e0(local_14,local_2c);
      puVar2 = (undefined4 *)FUN_00429570(param_4,local_84,local_14);
      *param_1 = *puVar2;
      param_1[1] = puVar2[1];
      param_1[2] = puVar2[2];
      fVar3 = (float10)FUN_0047a2a0(local_14);
      *param_2 = (float)fVar3;
      fVar3 = (float10)FUN_00456210(local_14);
      *param_3 = (float)fVar3;
    }
    else {
      local_3c = (void *)((int)this + 0x14);
      pfVar1 = (float *)FUN_004292f0(local_3c,local_6c,param_5 - *(float *)((int)this + 0x34));
      puVar2 = (undefined4 *)FUN_00429440(param_4,local_78,pfVar1);
      *param_1 = *puVar2;
      param_1[1] = puVar2[1];
      param_1[2] = puVar2[2];
      *param_2 = *(float *)((int)local_30 + 0x30) - *(float *)((int)local_30 + 0x34);
      *param_3 = param_6;
    }
  }
  else if (local_34 == 2) {
    FUN_00422e10(local_20);
    local_18 = 0;
    FUN_00439330(this_01,local_20,param_6,param_5);
    fVar3 = FUN_004592b0(param_7);
    local_40 = (float)fVar3;
    pfVar1 = (float *)FUN_004292f0(local_20,local_90,param_7 - local_40);
    puVar2 = (undefined4 *)FUN_00429440(param_4,local_9c,pfVar1);
    *param_1 = *puVar2;
    param_1[1] = puVar2[1];
    param_1[2] = puVar2[2];
    *param_2 = param_5 - *(float *)((int)local_30 + 0x34);
    fVar3 = FUN_00438540(param_6 - *(float *)((int)local_30 + 0x38));
    *param_3 = (float)fVar3;
    FUN_00439330(local_20,local_20,*param_3,*param_2);
    local_44 = DAT_0056c8cc - param_7;
    fVar3 = FUN_004592b0(param_7);
    local_48 = (float)fVar3;
    pfVar1 = (float *)FUN_004292f0(local_20,local_a8,local_44 + local_48);
    puVar2 = (undefined4 *)FUN_00429440(param_1,local_b4,pfVar1);
    *param_1 = *puVar2;
    param_1[1] = puVar2[1];
    param_1[2] = puVar2[2];
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

