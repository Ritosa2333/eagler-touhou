/* Entry: 0x004f7210; symbol: FUN_004f7210; body bytes: 543 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004f7210(void *param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  float10 fVar4;
  undefined4 uVar5;
  undefined1 local_20 [12];
  undefined1 local_14 [12];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00461070((void *)((int)param_1 + 0x10),0);
  if (bVar1) {
    FUN_00429420((void *)((int)param_1 + 0x10));
    bVar1 = FUN_00423590((void *)((int)param_1 + 0x10),0);
    if (bVar1) {
      *(undefined4 *)((int)param_1 + 0x34) = 0;
    }
  }
  FUN_00422dd0(local_14,0,0,0);
  FUN_00422dd0(local_20,0,0,0);
  bVar1 = FUN_004785e0((void *)((int)param_1 + 0x20),0);
  if (!bVar1) {
    FUN_00429420((void *)((int)param_1 + 0x20));
  }
  if (*(char *)((int)param_1 + 0x50) == '\0') {
    iVar2 = FUN_00460830(0);
    pfVar3 = (float *)FUN_00460850(iVar2);
    if (*pfVar3 <= DAT_00573170 && DAT_00573170 != *pfVar3) {
      iVar2 = FUN_00460830(0);
      iVar2 = FUN_00460850(iVar2);
      if (*(float *)(iVar2 + 4) <= DAT_0056fa30 && DAT_0056fa30 != *(float *)(iVar2 + 4)) {
        *(undefined1 *)((int)param_1 + 0x50) = 1;
      }
    }
  }
  else {
    iVar2 = FUN_00460830(0);
    pfVar3 = (float *)FUN_00460850(iVar2);
    if (*pfVar3 <= _DAT_0057316c) {
      iVar2 = FUN_00460830(0);
      iVar2 = FUN_00460850(iVar2);
      if (*(float *)(iVar2 + 4) <= DAT_0056fe7c) goto LAB_004f7381;
    }
    *(undefined1 *)((int)param_1 + 0x50) = 0;
  }
LAB_004f7381:
  FUN_00429420(param_1);
  bVar1 = FUN_00423590(param_1,0);
  if (bVar1) {
    FUN_004506b0((void *)((int)param_1 + 0x4c),0,0);
    *(undefined4 *)((int)param_1 + 0x38) = 0;
    FUN_00423520(param_1,0);
  }
  else {
    uVar5 = DAT_0056c8cc;
    fVar4 = FUN_00423bd0((int)param_1);
    FUN_004506b0((void *)((int)param_1 + 0x4c),(float)fVar4 / DAT_0056f280,uVar5);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

