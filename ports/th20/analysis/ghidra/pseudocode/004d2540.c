/* Entry: 0x004d2540; symbol: FUN_004d2540; body bytes: 521 */

void __fastcall FUN_004d2540(int param_1)

{
  undefined4 *puVar1;
  bool bVar2;
  int iVar3;
  float *pfVar4;
  float10 fVar5;
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
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (void *)(param_1 + 0x90);
  local_18 = param_1;
  local_28 = (void *)FUN_0047c680(local_1c,7);
  local_20 = (void *)(local_18 + 0x90);
  iVar3 = FUN_0047c680(local_20,7);
  local_24 = *(int *)(iVar3 + 0x30);
  bVar2 = FUN_004235c0(local_28,local_24);
  if (bVar2) {
    *(uint *)(local_18 + 0x698) = *(uint *)(local_18 + 0x698) & 0x7fffffff;
  }
  else {
    puVar1 = *(undefined4 **)(local_18 + 0x1328);
    *(undefined4 *)(local_18 + 0x58) = *puVar1;
    *(undefined4 *)(local_18 + 0x5c) = puVar1[1];
    *(undefined4 *)(local_18 + 0x60) = puVar1[2];
    local_2c = FUN_0040ff90(*(int *)(local_18 + 0x6f0));
    fVar5 = FUN_004ff350(local_2c,(float *)(local_18 + 0x58));
    local_34 = (float)fVar5;
    local_30 = (void *)(local_18 + 0x90);
    iVar3 = FUN_0047c680(local_30,7);
    local_38 = *(float *)(iVar3 + 0x14);
    fVar5 = (float10)FUN_00438600(local_38,local_34);
    local_40 = (float)fVar5;
    FUN_00422e10(local_14);
    local_3c = (void *)(local_18 + 0x90);
    iVar3 = FUN_0047c680(local_3c,7);
    FUN_00439330(local_14,(float *)local_14,local_40,*(float *)(iVar3 + 0x10));
    local_4c = FUN_00429440(local_14,local_60,(float *)(local_18 + 100));
    local_44 = (void *)(local_18 + 0x90);
    iVar3 = FUN_0047c680(local_44,7);
    local_48 = *(float *)(iVar3 + 0x24);
    pfVar4 = (float *)FUN_004292f0(local_4c,local_6c,local_48);
    FUN_004296e0((void *)(local_18 + 100),pfVar4);
    *(undefined4 *)(local_18 + 0x6c) = 0;
    fVar5 = (float10)FUN_0047a2a0((float *)(local_18 + 100));
    *(float *)(local_18 + 0x7c) = (float)fVar5;
    fVar5 = (float10)FUN_00456210((float *)(local_18 + 100));
    *(float *)(local_18 + 0x70) = (float)fVar5;
    local_50 = (void *)(local_18 + 0x90);
    local_54 = (undefined4 *)FUN_0047c680(local_50,7);
    FUN_00423540(local_54);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

