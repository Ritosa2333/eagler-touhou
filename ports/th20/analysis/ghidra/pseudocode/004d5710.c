/* Entry: 0x004d5710; symbol: FUN_004d5710; body bytes: 435 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004d5710(int param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  float10 fVar4;
  undefined1 local_50 [12];
  undefined4 *local_44;
  void *local_40;
  float local_3c;
  float local_38;
  void *local_34;
  float local_30;
  void *local_2c;
  float local_28;
  float local_24;
  float local_20;
  void *local_1c;
  void *local_18;
  int local_14;
  void *local_10;
  void *local_c;
  int local_8;
  
  local_c = (void *)(param_1 + 0x90);
  local_8 = param_1;
  local_18 = (void *)FUN_0047c680(local_c,10);
  local_10 = (void *)(local_8 + 0x90);
  iVar2 = FUN_0047c680(local_10,10);
  local_14 = *(int *)(iVar2 + 0x30);
  bVar1 = FUN_004235c0(local_18,local_14);
  if (bVar1) {
    *(uint *)(local_8 + 0x698) = *(uint *)(local_8 + 0x698) & 0xffdfffff;
    return 1;
  }
  local_1c = (void *)(local_8 + 0x90);
  iVar2 = FUN_0047c680(local_1c,10);
  fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
  local_20 = (float)fVar4;
  local_28 = local_20 * *(float *)(iVar2 + 0x10);
  local_24 = *(float *)(local_8 + 0x7c);
  *(float *)(local_8 + 0x7c) = local_24 + local_28;
  local_2c = (void *)(local_8 + 0x90);
  iVar2 = FUN_0047c680(local_2c,10);
  local_34 = (void *)(iVar2 + 0x24);
  fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
  local_30 = (float)fVar4;
  pfVar3 = (float *)FUN_004292f0(local_34,local_50,local_30);
  FUN_004296e0((void *)(local_8 + 100),pfVar3);
  fVar4 = FUN_0047a630(*(float *)(local_8 + 100));
  local_38 = (float)fVar4;
  if (local_38 <= _DAT_0056fd30) {
    fVar4 = FUN_0047a630(*(float *)(local_8 + 0x68));
    local_3c = (float)fVar4;
    if (local_3c <= _DAT_0056fd30) goto LAB_004d589f;
  }
  fVar4 = FUN_00459280(*(float *)(local_8 + 0x68),*(float *)(local_8 + 100));
  *(float *)(local_8 + 0x70) = (float)fVar4;
LAB_004d589f:
  local_40 = (void *)(local_8 + 0x90);
  local_44 = (undefined4 *)FUN_0047c680(local_40,10);
  FUN_00423540(local_44);
  return 0;
}

