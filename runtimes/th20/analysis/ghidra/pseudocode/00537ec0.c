/* Entry: 0x00537ec0; symbol: FUN_00537ec0; body bytes: 671 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00537ec0(int param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  int *piVar4;
  void *this;
  void *this_00;
  float10 fVar5;
  int local_64 [2];
  int local_5c [2];
  float local_54 [2];
  float local_4c [2];
  undefined1 local_44 [4];
  undefined4 *local_40;
  float local_3c;
  float local_38;
  float local_34;
  void *local_30;
  float local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  int local_1c;
  float local_18;
  float local_14;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_20 = (void *)(param_1 + 0xe4);
  local_1c = param_1;
  bVar1 = FUN_004785e0(local_20,0);
  if (bVar1) {
    fVar5 = FUN_004297d0(0x5ba4a8);
    FUN_00452f20((void *)(local_1c + 0xd4),(float)fVar5);
  }
  FUN_00447de0(&local_c);
  iVar2 = FUN_00460830(0);
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    local_28 = (void *)(local_1c + 0x88);
    pfVar3 = FUN_00538160(local_28,local_54);
    FUN_00537170(&local_c,pfVar3);
  }
  else {
    local_24 = (void *)(local_1c + 0x80);
    pfVar3 = FUN_00538160(local_24,local_4c);
    FUN_00537170(&local_c,pfVar3);
  }
  FUN_00422e10(&local_18);
  fVar5 = FUN_004292e0((float *)(local_1c + 0xd4));
  fVar5 = (float10)FUN_00439530(this,(float)fVar5);
  local_2c = (float)fVar5;
  local_14 = local_2c * _DAT_00576678;
  local_30 = (void *)(local_1c + 0xd4);
  pfVar3 = (float *)FUN_00429390(local_30,local_44,DAT_0056c8d0);
  fVar5 = FUN_004292e0(pfVar3);
  fVar5 = (float10)FUN_00439530(this_00,(float)fVar5);
  local_34 = (float)fVar5;
  local_18 = local_34 * DAT_0056d7bc;
  FUN_00453040(&local_18);
  iVar2 = FUN_0040ff90(*(int *)(local_1c + 0x128));
  piVar4 = (int *)FUN_004ff690(iVar2);
  *(int *)(local_1c + 0x70) = (int)(local_18 * DAT_0056cd98) + *piVar4;
  iVar2 = FUN_0040ff90(*(int *)(local_1c + 0x128));
  iVar2 = FUN_004ff690(iVar2);
  *(int *)(local_1c + 0x74) = (int)(local_14 * DAT_0056cd98) + *(int *)(iVar2 + 4);
  iVar2 = FUN_00460830(0);
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    FUN_004f70f0((void *)(local_1c + 0x70),local_64,(int *)(local_1c + 0x88));
  }
  else {
    FUN_004f70f0((void *)(local_1c + 0x70),local_5c,(int *)(local_1c + 0x80));
  }
  local_3c = (DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056cdb0;
  fVar5 = FUN_00429830(0x5ba4a8);
  local_38 = (float)fVar5;
  FUN_004530c0((void *)(local_1c + 0xd4),
               local_3c + (local_38 * DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056cdb0);
  local_40 = (undefined4 *)(local_1c + 0xe4);
  FUN_00423540(local_40);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

