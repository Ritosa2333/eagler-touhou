/* Entry: 0x00536ac0; symbol: FUN_00536ac0; body bytes: 652 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00536ac0(int param_1)

{
  bool bVar1;
  float *pfVar2;
  int iVar3;
  int *piVar4;
  void *this;
  void *this_00;
  float10 fVar5;
  int local_50 [2];
  int local_48 [2];
  undefined1 local_40 [4];
  undefined4 *local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  void *local_24;
  float local_20;
  void *local_1c;
  int local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (void *)(param_1 + 0xe4);
  local_18 = param_1;
  bVar1 = FUN_004785e0(local_1c,0);
  if (bVar1) {
    fVar5 = FUN_004297d0(0x5ba4a8);
    FUN_00452f20((void *)(local_18 + 0xd4),(float)fVar5);
  }
  FUN_00422e10(&local_14);
  fVar5 = FUN_004292e0((float *)(local_18 + 0xd4));
  fVar5 = (float10)FUN_00439530(this,(float)fVar5);
  local_20 = (float)fVar5;
  local_14 = local_20 * DAT_00570560;
  local_24 = (void *)(local_18 + 0xd4);
  pfVar2 = (float *)FUN_00429390(local_24,local_40,DAT_0056c8d0);
  fVar5 = FUN_004292e0(pfVar2);
  fVar5 = (float10)FUN_00439530(this_00,(float)fVar5);
  local_28 = (float)fVar5;
  local_10 = local_28 * DAT_0056d7bc;
  iVar3 = FUN_0040ff90(*(int *)(local_18 + 0x128));
  piVar4 = (int *)FUN_004ff690(iVar3);
  *(int *)(local_18 + 0x70) = (int)(local_14 * DAT_0056cd98) + *piVar4;
  iVar3 = FUN_0040ff90(*(int *)(local_18 + 0x128));
  iVar3 = FUN_004ff690(iVar3);
  *(int *)(local_18 + 0x74) = (int)(local_10 * DAT_0056cd98) + *(int *)(iVar3 + 4);
  iVar3 = FUN_00460830(0);
  bVar1 = FUN_004ff5e0(iVar3);
  if (bVar1) {
    FUN_004f70f0((void *)(local_18 + 0x70),local_50,(int *)(local_18 + 0x88));
    local_38 = (DAT_0056e0f0 * DAT_0056c8d0) / _DAT_00575698;
    fVar5 = FUN_00429830(0x5ba4a8);
    local_34 = (float)fVar5;
    FUN_004530c0((void *)(local_18 + 0xd4),
                 local_38 + (local_34 * DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056cdb0);
  }
  else {
    FUN_004f70f0((void *)(local_18 + 0x70),local_48,(int *)(local_18 + 0x80));
    local_30 = (DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056cdb0;
    fVar5 = FUN_00429830(0x5ba4a8);
    local_2c = (float)fVar5;
    FUN_004530c0((void *)(local_18 + 0xd4),
                 local_30 + (local_2c * DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056cdb0);
  }
  local_3c = (undefined4 *)(local_18 + 0xe4);
  FUN_00423540(local_3c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

