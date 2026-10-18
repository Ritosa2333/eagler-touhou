/* Entry: 0x004827e0; symbol: FUN_004827e0; body bytes: 632 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004827e0(int param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  void *this;
  float10 fVar4;
  undefined1 local_74 [12];
  float local_68 [3];
  undefined4 *local_5c;
  void *local_58;
  float local_54;
  float local_50;
  float *local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  float local_38;
  float local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  int local_24;
  void *local_20;
  void *local_1c;
  int local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (void *)(param_1 + 0xa0);
  local_18 = param_1;
  local_28 = (void *)FUN_0047c680(local_1c,8);
  local_20 = (void *)(local_18 + 0xa0);
  iVar2 = FUN_0047c680(local_20,8);
  local_24 = *(int *)(iVar2 + 0x30);
  bVar1 = FUN_004235c0(local_28,local_24);
  if (bVar1) {
    *(uint *)(local_18 + 0x90) = *(uint *)(local_18 + 0x90) & 0xfffdffff;
    local_2c = (void *)(local_18 + 0xa0);
    iVar2 = FUN_0047c680(local_2c,8);
    *(undefined4 *)(local_18 + 100) = *(undefined4 *)(iVar2 + 0x24);
    *(undefined4 *)(local_18 + 0x68) = *(undefined4 *)(iVar2 + 0x28);
    *(undefined4 *)(local_18 + 0x6c) = *(undefined4 *)(iVar2 + 0x2c);
    local_30 = (void *)(local_18 + 0xa0);
    iVar2 = FUN_0047c680(local_30,8);
    *(undefined4 *)(local_18 + 0x20) = *(undefined4 *)(iVar2 + 0x10);
    local_34 = *(float *)(local_18 + 0x20);
    fVar4 = FUN_004292e0((float *)(local_18 + 0x84));
    local_38 = (float)fVar4;
    FUN_00439330(this,(float *)(local_18 + 0x70),local_38,local_34);
    *(undefined4 *)(local_18 + 0x78) = 0;
    goto LAB_00482a4f;
  }
  local_3c = (void *)(local_18 + 0xa0);
  local_40 = (void *)FUN_0047c680(local_3c,8);
  bVar1 = FUN_004785e0(local_40,0);
  if (bVar1) {
    local_44 = (void *)(local_18 + 0x420);
    FUN_004393f0(local_44,(undefined4 *)(local_18 + 100));
  }
  local_14 = *(float *)(local_18 + 100);
  local_10 = *(undefined4 *)(local_18 + 0x68);
  local_c = *(undefined4 *)(local_18 + 0x6c);
  local_48 = (void *)(local_18 + 0x420);
  local_4c = FUN_0042ad80(local_48,local_68);
  puVar3 = (undefined4 *)FUN_00429440(local_4c,local_74,&local_14);
  *(undefined4 *)(local_18 + 0x70) = *puVar3;
  *(undefined4 *)(local_18 + 0x74) = puVar3[1];
  *(undefined4 *)(local_18 + 0x78) = puVar3[2];
  fVar4 = FUN_0047a630(*(float *)(local_18 + 0x70));
  local_50 = (float)fVar4;
  if (_DAT_0056fd30 < local_50) {
LAB_004829ea:
    fVar4 = FUN_00459280(*(float *)(local_18 + 0x74),*(float *)(local_18 + 0x70));
    FUN_00452f20((void *)(local_18 + 0x84),(float)fVar4);
  }
  else {
    fVar4 = FUN_0047a630(*(float *)(local_18 + 0x74));
    local_54 = (float)fVar4;
    if (_DAT_0056fd30 < local_54) goto LAB_004829ea;
  }
  *(undefined4 *)(local_18 + 0x78) = 0;
  local_58 = (void *)(local_18 + 0xa0);
  local_5c = (undefined4 *)FUN_0047c680(local_58,8);
  FUN_00423540(local_5c);
LAB_00482a4f:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

