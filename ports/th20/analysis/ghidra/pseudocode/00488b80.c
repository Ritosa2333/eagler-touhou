/* Entry: 0x00488b80; symbol: FUN_00488b80; body bytes: 758 */

void __fastcall FUN_00488b80(int param_1)

{
  float fVar1;
  float fVar2;
  float fVar3;
  bool bVar4;
  char cVar5;
  undefined4 *puVar6;
  int iVar7;
  void *pvVar8;
  int *piVar9;
  float *pfVar10;
  void *this;
  int *unaff_FS_OFFSET;
  float10 fVar11;
  float10 fVar12;
  float10 fVar13;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056908d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar6 = (undefined4 *)FUN_0048bce0((void *)(param_1 + 0xc),0);
  iVar7 = FUN_0044ced0(puVar6);
  fVar11 = FUN_004379c0(iVar7);
  fVar12 = FUN_004379a0(iVar7);
  fVar13 = FUN_00437930(iVar7);
  iVar7 = FUN_0040c300(*(undefined4 **)(param_1 + 0x2ec));
  pvVar8 = (void *)FUN_00485970(iVar7);
  FUN_0040c080(local_1c,8);
  FUN_00412280(pvVar8,local_1c);
  local_8 = 0;
  piVar9 = (int *)FUN_00412540();
  while( true ) {
    bVar4 = FUN_00411bb0(local_1c,piVar9);
    if (!bVar4) break;
    puVar6 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar8 = (void *)FUN_0040c300(puVar6);
    iVar7 = FUN_00499370((int)pvVar8);
    fVar1 = *(float *)(iVar7 + 4);
    pfVar10 = (float *)FUN_00499370((int)pvVar8);
    fVar2 = *pfVar10;
    iVar7 = FUN_00499350(param_1);
    fVar3 = *(float *)(iVar7 + 4);
    pfVar10 = (float *)FUN_00499350(param_1);
    cVar5 = FUN_00457610(*pfVar10,fVar3,(float)fVar11,(float)fVar12,(float)fVar13,fVar2,fVar1,
                         DAT_0056f00c);
    if (cVar5 != '\0') {
      FUN_0049cae0(pvVar8,*(float *)(param_1 + 0x88));
      this = (void *)FUN_00421810((int)pvVar8);
      FUN_0044fa10(this,0xffa05000);
      FUN_0049c760((int)pvVar8);
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  iVar7 = FUN_0040ff90(*(int *)(param_1 + 0x2ec));
  iVar7 = FUN_00460850(iVar7);
  fVar1 = *(float *)(iVar7 + 4);
  iVar7 = FUN_0040ff90(*(int *)(param_1 + 0x2ec));
  pfVar10 = (float *)FUN_00460850(iVar7);
  fVar2 = *pfVar10;
  iVar7 = FUN_00499350(param_1);
  fVar3 = *(float *)(iVar7 + 4);
  pfVar10 = (float *)FUN_00499350(param_1);
  cVar5 = FUN_00457610(*pfVar10,fVar3,(float)fVar11,(float)fVar12,(float)fVar13,fVar2,fVar1,
                       DAT_0056e734);
  if (cVar5 != '\0') {
    pvVar8 = (void *)FUN_00460830(0);
    FUN_00479080(pvVar8,*(undefined4 *)(param_1 + 0x88));
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

