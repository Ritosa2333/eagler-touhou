/* Entry: 0x00480db0; symbol: FUN_00480db0; body bytes: 488 */

void __fastcall FUN_00480db0(int param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  void *this;
  float10 fVar4;
  undefined1 local_6c [12];
  undefined1 local_60 [12];
  undefined4 *local_54;
  void *local_50;
  void *local_4c;
  float local_48;
  void *local_44;
  float local_40;
  void *local_3c;
  float local_38;
  float local_34;
  void *local_30;
  int local_2c;
  void *local_28;
  int local_24;
  void *local_20;
  void *local_1c;
  int local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (void *)(param_1 + 0xa0);
  local_18 = param_1;
  local_28 = (void *)FUN_0047c680(local_1c,7);
  local_20 = (void *)(local_18 + 0xa0);
  iVar2 = FUN_0047c680(local_20,7);
  local_24 = *(int *)(iVar2 + 0x30);
  bVar1 = FUN_004235c0(local_28,local_24);
  if (bVar1) {
    *(uint *)(local_18 + 0x90) = *(uint *)(local_18 + 0x90) & 0x7fffffff;
  }
  else {
    local_2c = FUN_0040ff90(*(int *)(local_18 + 0x520));
    fVar4 = FUN_004ff350(local_2c,(float *)(local_18 + 100));
    local_34 = (float)fVar4;
    local_30 = (void *)(local_18 + 0xa0);
    iVar2 = FUN_0047c680(local_30,7);
    local_38 = *(float *)(iVar2 + 0x14);
    fVar4 = (float10)FUN_00438600(local_38,local_34);
    local_40 = (float)fVar4;
    FUN_00422e10(local_14);
    local_3c = (void *)(local_18 + 0xa0);
    iVar2 = FUN_0047c680(local_3c,7);
    FUN_00439330(this,local_14,local_40,*(float *)(iVar2 + 0x10));
    local_4c = FUN_00429440(local_14,local_60,(float *)(local_18 + 0x70));
    local_44 = (void *)(local_18 + 0xa0);
    iVar2 = FUN_0047c680(local_44,7);
    local_48 = *(float *)(iVar2 + 0x24);
    pfVar3 = (float *)FUN_004292f0(local_4c,local_6c,local_48);
    FUN_004296e0((void *)(local_18 + 0x70),pfVar3);
    *(undefined4 *)(local_18 + 0x78) = 0;
    fVar4 = (float10)FUN_0047a2a0((float *)(local_18 + 0x70));
    *(float *)(local_18 + 0x20) = (float)fVar4;
    fVar4 = (float10)FUN_00456210((float *)(local_18 + 0x70));
    FUN_00452f20((void *)(local_18 + 0x84),(float)fVar4);
    local_50 = (void *)(local_18 + 0xa0);
    local_54 = (undefined4 *)FUN_0047c680(local_50,7);
    FUN_00423540(local_54);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

