/* Entry: 0x004358d0; symbol: FUN_004358d0; body bytes: 944 */

void __fastcall FUN_004358d0(int param_1)

{
  float fVar1;
  int iVar2;
  float *pfVar3;
  undefined1 *puVar4;
  float10 fVar5;
  int local_84 [3];
  float local_78 [3];
  int local_6c [3];
  float local_60 [3];
  float local_54 [3];
  float local_48 [2];
  float local_40 [2];
  float local_38 [2];
  float local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  void *local_1c;
  void *local_18;
  void *local_14;
  void *local_10;
  void *local_c;
  int local_8;
  
  local_8 = param_1;
  iVar2 = FUN_00438500(param_1 + 0x8c);
  if (iVar2 != 0) {
    if ((*(uint *)(local_8 + 0x49c) >> 6 & 1) == 0) {
      local_c = (void *)(local_8 + 0x8c);
      pfVar3 = FUN_0042ad80(local_c,local_54);
      *(float *)(local_8 + 0x2c) = *pfVar3;
      *(float *)(local_8 + 0x30) = pfVar3[1];
      *(float *)(local_8 + 0x34) = pfVar3[2];
    }
    else {
      local_10 = (void *)(local_8 + 0x8c);
      pfVar3 = FUN_0042ad80(local_10,local_60);
      *(float *)(local_8 + 0x484) = *pfVar3;
      *(float *)(local_8 + 0x488) = pfVar3[1];
      *(float *)(local_8 + 0x48c) = pfVar3[2];
    }
  }
  iVar2 = FUN_00438500(local_8 + 0xe0);
  if (iVar2 != 0) {
    local_14 = (void *)(local_8 + 0xe0);
    FUN_0042a3a0(local_14,local_6c);
    iVar2 = FUN_00437750(local_8 + 0xe0);
    *(undefined1 *)(local_8 + 0x492) = *(undefined1 *)(iVar2 + 8);
    iVar2 = FUN_00437750(local_8 + 0xe0);
    *(undefined1 *)(local_8 + 0x491) = *(undefined1 *)(iVar2 + 4);
    puVar4 = (undefined1 *)FUN_00437750(local_8 + 0xe0);
    *(undefined1 *)(local_8 + 0x490) = *puVar4;
  }
  iVar2 = FUN_0041caf0(local_8 + 0x134);
  if (iVar2 != 0) {
    iVar2 = FUN_00429e90((int *)(local_8 + 0x134));
    *(char *)(local_8 + 0x493) = (char)iVar2;
  }
  iVar2 = FUN_00438520(local_8 + 0x1e0);
  if (iVar2 != 0) {
    local_18 = (void *)(local_8 + 0x1e0);
    pfVar3 = FUN_0042a980(local_18,local_38);
    fVar1 = pfVar3[1];
    *(float *)(local_8 + 0x50) = *pfVar3;
    *(float *)(local_8 + 0x54) = fVar1;
    *(uint *)(local_8 + 0x49c) = *(uint *)(local_8 + 0x49c) | 4;
  }
  iVar2 = FUN_00438520(local_8 + 0x220);
  if (iVar2 != 0) {
    local_1c = (void *)(local_8 + 0x220);
    pfVar3 = FUN_0042a980(local_1c,local_40);
    fVar1 = pfVar3[1];
    *(float *)(local_8 + 0x58) = *pfVar3;
    *(float *)(local_8 + 0x5c) = fVar1;
    *(uint *)(local_8 + 0x49c) = *(uint *)(local_8 + 0x49c) | 4;
  }
  iVar2 = FUN_00438520(local_8 + 0x260);
  if (iVar2 != 0) {
    local_20 = (void *)(local_8 + 0x260);
    pfVar3 = FUN_0042a980(local_20,local_48);
    fVar1 = pfVar3[1];
    *(float *)(local_8 + 0x68) = *pfVar3;
    *(float *)(local_8 + 0x6c) = fVar1;
    *(uint *)(local_8 + 0x49c) = *(uint *)(local_8 + 0x49c) | 8;
  }
  iVar2 = FUN_00438500(local_8 + 0x160);
  if (iVar2 != 0) {
    local_24 = (void *)(local_8 + 0x160);
    pfVar3 = FUN_0042ad80(local_24,local_78);
    *(float *)(local_8 + 0x38) = *pfVar3;
    *(float *)(local_8 + 0x3c) = pfVar3[1];
    *(float *)(local_8 + 0x40) = pfVar3[2];
    *(uint *)(local_8 + 0x49c) = *(uint *)(local_8 + 0x49c) | 2;
  }
  iVar2 = FUN_0041caf0(local_8 + 0x1b4);
  if (iVar2 != 0) {
    local_28 = (void *)(local_8 + 0x1b4);
    pfVar3 = FUN_0042b1f0(local_28,&local_30);
    fVar5 = FUN_004292e0(pfVar3);
    *(float *)(local_8 + 0x40) = (float)fVar5;
    *(uint *)(local_8 + 0x49c) = *(uint *)(local_8 + 0x49c) | 2;
  }
  iVar2 = FUN_00438500(local_8 + 0x2a0);
  if (iVar2 != 0) {
    local_2c = (void *)(local_8 + 0x2a0);
    FUN_0042a3a0(local_2c,local_84);
    iVar2 = FUN_00437750(local_8 + 0x2a0);
    *(undefined1 *)(local_8 + 0x496) = *(undefined1 *)(iVar2 + 8);
    iVar2 = FUN_00437750(local_8 + 0x2a0);
    *(undefined1 *)(local_8 + 0x495) = *(undefined1 *)(iVar2 + 4);
    puVar4 = (undefined1 *)FUN_00437750(local_8 + 0x2a0);
    *(undefined1 *)(local_8 + 0x494) = *puVar4;
  }
  iVar2 = FUN_0041caf0(local_8 + 0x2f4);
  if (iVar2 != 0) {
    iVar2 = FUN_00429e90((int *)(local_8 + 0x2f4));
    *(char *)(local_8 + 0x497) = (char)iVar2;
  }
  iVar2 = FUN_0041caf0(local_8 + 800);
  if (iVar2 != 0) {
    fVar5 = FUN_0042a110((float *)(local_8 + 800));
    *(float *)(local_8 + 0x3a0) = (float)fVar5;
  }
  iVar2 = FUN_0041caf0(local_8 + 0x34c);
  if (iVar2 != 0) {
    fVar5 = FUN_0042a110((float *)(local_8 + 0x34c));
    *(float *)(local_8 + 0x3a4) = (float)fVar5;
  }
  return;
}

