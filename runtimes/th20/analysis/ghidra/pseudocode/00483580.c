/* Entry: 0x00483580; symbol: FUN_00483580; body bytes: 898 */

undefined4 __fastcall FUN_00483580(int param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  void *this;
  void *this_00;
  float10 fVar6;
  float local_c;
  
  pvVar2 = (void *)FUN_0047c680((void *)(param_1 + 0xa0),3);
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
  bVar1 = FUN_004235c0(pvVar2,*(int *)(iVar3 + 0x30));
  if (bVar1) {
    if (-1 < *(int *)(param_1 + 0x40)) {
      FUN_00426d70(&DAT_005ba830,*(int *)(param_1 + 0x40),0);
    }
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    *(int *)(iVar3 + 0x38) = *(int *)(iVar3 + 0x38) + 1;
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    switch(*(undefined4 *)(iVar3 + 0x3c)) {
    case 0:
    case 5:
      iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
      FUN_004530c0((void *)(param_1 + 0x84),*(float *)(iVar3 + 0x14));
      break;
    case 1:
    case 6:
      iVar3 = FUN_0040ff90(*(int *)(param_1 + 0x520));
      fVar6 = FUN_004ff350(iVar3,(float *)(param_1 + 100));
      iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
      FUN_00452f20((void *)(param_1 + 0x84),(float)fVar6 + *(float *)(iVar3 + 0x14));
      break;
    case 2:
      iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
      FUN_00452f20((void *)(param_1 + 0x84),*(float *)(iVar3 + 0x14));
      break;
    case 3:
      iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
      FUN_00452f20((void *)(param_1 + 0x84),*(float *)(iVar3 + 0x14));
      break;
    case 4:
      iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
      FUN_00452f20((void *)(param_1 + 0x84),*(float *)(iVar3 + 0x14));
    }
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    *(undefined4 *)(param_1 + 0x20) = *(undefined4 *)(iVar3 + 0x10);
    local_c = *(float *)(param_1 + 0x20);
    iVar3 = 0;
    pvVar2 = (void *)FUN_0047c680((void *)(param_1 + 0xa0),3);
    FUN_00423520(pvVar2,iVar3);
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    iVar4 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    if (*(int *)(iVar3 + 0x34) <= *(int *)(iVar4 + 0x38)) {
      fVar6 = FUN_004292e0((float *)(param_1 + 0x84));
      FUN_00439330(this,(float *)(param_1 + 0x70),(float)fVar6,local_c);
      *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xffffffef;
      return 1;
    }
  }
  else {
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    iVar3 = *(int *)(iVar3 + 0x30);
    iVar4 = FUN_0047c680((void *)(param_1 + 0xa0),3);
    fVar6 = FUN_00423bd0(iVar4);
    local_c = *(float *)(param_1 + 0x20) -
              ((float)fVar6 * *(float *)(param_1 + 0x20)) / (float)iVar3;
  }
  fVar6 = FUN_004292e0((float *)(param_1 + 0x84));
  FUN_00439330(this_00,(float *)(param_1 + 0x70),(float)fVar6,local_c);
  puVar5 = (undefined4 *)FUN_0047c680((void *)(param_1 + 0xa0),3);
  FUN_00423540(puVar5);
  return 0;
}

