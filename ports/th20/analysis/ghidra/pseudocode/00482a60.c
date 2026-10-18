/* Entry: 0x00482a60; symbol: FUN_00482a60; body bytes: 832 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00482a60(int param_1)

{
  float fVar1;
  bool bVar2;
  int iVar3;
  float *pfVar4;
  int iVar5;
  float10 fVar6;
  
  iVar3 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  pfVar4 = (float *)FUN_00485990(iVar3);
  if (*pfVar4 <= 0.0) {
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    fVar1 = *(float *)(iVar3 + 0x1c);
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    iVar3 = FUN_00485420((float *)(param_1 + 100),0.0,0.0,*(float *)(iVar3 + 0x18),fVar1);
    if (iVar3 != 0) goto LAB_00482ba8;
  }
  iVar3 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  pfVar4 = (float *)FUN_00485990(iVar3);
  if (*pfVar4 <= _DAT_0056e0e8) {
    return 0;
  }
  iVar3 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  iVar3 = FUN_00485990(iVar3);
  fVar1 = *(float *)(iVar3 + 4);
  iVar3 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  pfVar4 = (float *)FUN_00485990(iVar3);
  iVar3 = FUN_00485420((float *)(param_1 + 100),0.0,0.0,*pfVar4,fVar1);
  if (iVar3 == 0) {
    return 0;
  }
LAB_00482ba8:
  bVar2 = false;
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
  if (((*(uint *)(iVar3 + 0x3c) & 1) != 0) && (iVar3 = FUN_00483150(param_1), iVar3 != 0)) {
    bVar2 = true;
  }
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
  if (((*(uint *)(iVar3 + 0x3c) & 2) != 0) && (iVar3 = FUN_00482da0(param_1), iVar3 != 0)) {
    bVar2 = true;
  }
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
  if (((*(uint *)(iVar3 + 0x3c) & 8) != 0) && (iVar3 = FUN_00483010(param_1), iVar3 != 0)) {
    bVar2 = true;
  }
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
  if (((*(uint *)(iVar3 + 0x3c) & 4) != 0) && (iVar3 = FUN_00482ec0(param_1), iVar3 != 0)) {
    bVar2 = true;
  }
  if (bVar2) {
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    if (DAT_0056fd74 < *(float *)(iVar3 + 0x10)) {
      iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
      *(undefined4 *)(param_1 + 0x20) = *(undefined4 *)(iVar3 + 0x10);
    }
    fVar1 = *(float *)(param_1 + 0x20);
    fVar6 = FUN_004292e0((float *)(param_1 + 0x84));
    FUN_00439330((void *)(param_1 + 0x70),(float *)(param_1 + 0x70),(float)fVar6,fVar1);
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    *(int *)(iVar3 + 0x30) = *(int *)(iVar3 + 0x30) + 1;
    if (-1 < *(int *)(param_1 + 0x40)) {
      FUN_00426d70(&DAT_005ba830,*(int *)(param_1 + 0x40),0);
    }
  }
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),4);
  iVar5 = FUN_0047c680((void *)(param_1 + 0xa0),4);
  if (*(int *)(iVar5 + 0x30) < *(int *)(iVar3 + 0x34)) {
    return 0;
  }
  *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xffffffbf;
  return 1;
}

