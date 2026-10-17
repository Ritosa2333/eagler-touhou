/* Entry: 0x004d50a0; symbol: FUN_004d50a0; body bytes: 609 */

undefined4 __fastcall FUN_004d50a0(void *param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  float10 fVar6;
  float local_c;
  
  pvVar2 = (void *)FUN_0047c680((void *)((int)param_1 + 0x90),3);
  iVar3 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
  bVar1 = FUN_004235c0(pvVar2,*(int *)(iVar3 + 0x30));
  if (bVar1) {
    if (-1 < *(int *)((int)param_1 + 0x738)) {
      FUN_00426d70(&DAT_005ba830,*(int *)((int)param_1 + 0x738),0);
    }
    iVar3 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    *(int *)(iVar3 + 0x38) = *(int *)(iVar3 + 0x38) + 1;
    iVar3 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    *(float *)((int)param_1 + 0x70) = *(float *)((int)param_1 + 0x70) + *(float *)(iVar3 + 0x14);
    iVar3 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    *(undefined4 *)((int)param_1 + 0x7c) = *(undefined4 *)(iVar3 + 0x10);
    local_c = *(float *)((int)param_1 + 0x7c);
    iVar3 = 0;
    pvVar2 = (void *)FUN_0047c680((void *)((int)param_1 + 0x90),3);
    FUN_00423520(pvVar2,iVar3);
    iVar3 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    iVar4 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    pvVar2 = *(void **)(iVar4 + 0x38);
    if (*(int *)(iVar3 + 0x34) <= (int)pvVar2) {
      FUN_00439330(pvVar2,(float *)((int)param_1 + 100),*(float *)((int)param_1 + 0x70),local_c);
      *(uint *)((int)param_1 + 0x698) = *(uint *)((int)param_1 + 0x698) & 0xffffffef;
      return 1;
    }
  }
  else {
    iVar3 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    iVar3 = *(int *)(iVar3 + 0x30);
    iVar4 = FUN_0047c680((void *)((int)param_1 + 0x90),3);
    fVar6 = FUN_00423bd0(iVar4);
    local_c = *(float *)((int)param_1 + 0x7c) -
              ((float)fVar6 * *(float *)((int)param_1 + 0x7c)) / (float)iVar3;
    pvVar2 = param_1;
  }
  FUN_00439330(pvVar2,(float *)((int)param_1 + 100),*(float *)((int)param_1 + 0x70),local_c);
  puVar5 = (undefined4 *)FUN_0047c680((void *)((int)param_1 + 0x90),3);
  FUN_00423540(puVar5);
  return 0;
}

