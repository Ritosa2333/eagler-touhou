/* Entry: 0x00530010; symbol: FUN_00530010; body bytes: 421 */

void FUN_00530010(void)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  float *pfVar4;
  void *this;
  float10 fVar5;
  float fVar6;
  undefined1 local_2c [4];
  undefined1 local_28 [4];
  void *local_24;
  void *local_20;
  float local_1c;
  int local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  iVar2 = FUN_0040ff90(*(int *)(local_18 + 0x128));
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    local_1c = DAT_0056cd94;
  }
  else {
    local_1c = DAT_0056e048;
  }
  fVar6 = local_1c;
  fVar5 = FUN_004292e0((float *)(local_18 + 0xd4));
  FUN_00439330(this,&local_14,(float)fVar5,fVar6);
  iVar2 = FUN_0040ff90(*(int *)(local_18 + 0x128));
  piVar3 = (int *)FUN_004ff690(iVar2);
  *(int *)(local_18 + 0x70) = (int)(local_14 * DAT_0056cd98) + *piVar3;
  iVar2 = FUN_0040ff90(*(int *)(local_18 + 0x128));
  iVar2 = FUN_004ff690(iVar2);
  *(int *)(local_18 + 0x74) = (int)(local_10 * DAT_0056cd98) + *(int *)(iVar2 + 4);
  iVar2 = FUN_0040ff90(*(int *)(local_18 + 0x128));
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    local_24 = (void *)(local_18 + 0xd4);
    pfVar4 = (float *)FUN_00452fc0(local_24,local_2c,DAT_0056e0f0 / DAT_00570560);
    fVar5 = FUN_004292e0(pfVar4);
    fVar5 = FUN_00438540((float)fVar5);
    FUN_00452f20((void *)(local_18 + 0xd4),(float)fVar5);
  }
  else {
    local_20 = (void *)(local_18 + 0xd4);
    pfVar4 = (float *)FUN_00452fc0(local_20,local_28,DAT_0056e0f0 / DAT_00570ad0);
    fVar5 = FUN_004292e0(pfVar4);
    fVar5 = FUN_00438540((float)fVar5);
    FUN_00452f20((void *)(local_18 + 0xd4),(float)fVar5);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

