/* Entry: 0x004a0600; symbol: FUN_004a0600; body bytes: 876 */

undefined4 __fastcall FUN_004a0600(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  byte *pbVar3;
  undefined1 *puVar4;
  undefined1 (*pauVar5) [16];
  undefined4 *puVar6;
  SIZE_T *pSVar7;
  int iVar8;
  
  DAT_005c49e8 = param_1;
  piVar1 = FUN_004122c0(0x2f,FUN_004a0de0,param_1);
  *(int **)(param_1 + 8) = piVar1;
  piVar1 = FUN_00412360(0x57,FUN_0049dea0,param_1);
  *(int **)(param_1 + 0xc) = piVar1;
  FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
  uVar2 = FUN_004bd4e0(0x5ba568);
  *(undefined4 *)(param_1 + 0x1c) = uVar2;
  *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) & 0xfffffffb;
  if (-1 < DAT_005afc98) {
    *(int *)(param_1 + 0x1c) = DAT_005afc98;
    DAT_005afc98 = -1;
    *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 4;
    goto LAB_004a08bc;
  }
  iVar8 = FUN_004a0df0(DAT_005c6108,*(uint *)(param_1 + 0x1c));
  if (iVar8 == 0) {
    *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 1;
  }
  iVar8 = FUN_004a0df0(DAT_005c6108,0x12);
  if (iVar8 == 0) {
LAB_004a0725:
    *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 2;
  }
  else {
    iVar8 = FUN_004a0df0(DAT_005c6108,0x13);
    if (iVar8 == 0) goto LAB_004a0725;
    iVar8 = FUN_004a0df0(DAT_005c6108,0x14);
    if (iVar8 == 0) goto LAB_004a0725;
    iVar8 = FUN_004a0df0(DAT_005c6108,0x15);
    if (iVar8 == 0) goto LAB_004a0725;
  }
  iVar8 = FUN_004640e0((int)DAT_005c6108);
  pbVar3 = (byte *)FUN_00411c10((void *)(iVar8 + 0x16),*(int *)(param_1 + 0x1c));
  *pbVar3 = *pbVar3 | 1;
  iVar8 = FUN_004640a0(0x5ba568);
  if (iVar8 == 1) {
    iVar8 = FUN_004640e0((int)DAT_005c6108);
    pbVar3 = (byte *)FUN_00411c10((void *)(iVar8 + 0x16),*(int *)(param_1 + 0x1c));
    *pbVar3 = *pbVar3 | 2;
  }
  else {
    iVar8 = FUN_004640a0(0x5ba568);
    if (iVar8 == 2) {
      iVar8 = FUN_004640e0((int)DAT_005c6108);
      pbVar3 = (byte *)FUN_00411c10((void *)(iVar8 + 0x16),*(int *)(param_1 + 0x1c));
      *pbVar3 = *pbVar3 | 4;
    }
    else {
      iVar8 = FUN_004640a0(0x5ba568);
      if (iVar8 == 3) {
        iVar8 = FUN_004640e0((int)DAT_005c6108);
        pbVar3 = (byte *)FUN_00411c10((void *)(iVar8 + 0x16),*(int *)(param_1 + 0x1c));
        *pbVar3 = *pbVar3 | 8;
      }
    }
  }
  iVar8 = FUN_004640e0((int)DAT_005c6108);
  puVar4 = (undefined1 *)FUN_00411c10((void *)(iVar8 + 0x16),0x12);
  *puVar4 = 1;
  iVar8 = FUN_004640e0((int)DAT_005c6108);
  puVar4 = (undefined1 *)FUN_00411c10((void *)(iVar8 + 0x16),0x13);
  *puVar4 = 1;
  iVar8 = FUN_004640e0((int)DAT_005c6108);
  puVar4 = (undefined1 *)FUN_00411c10((void *)(iVar8 + 0x16),0x14);
  *puVar4 = 1;
  iVar8 = FUN_004640e0((int)DAT_005c6108);
  puVar4 = (undefined1 *)FUN_00411c10((void *)(iVar8 + 0x16),0x15);
  *puVar4 = 1;
LAB_004a08bc:
  iVar8 = 0;
  pSVar7 = (SIZE_T *)0x0;
  pauVar5 = (undefined1 (*) [16])
            FUN_004bd4a0((int)(&PTR_s_e00a0_msg_005afca0)[*(int *)(param_1 + 0x1c)]);
  uVar2 = FUN_00410aa0(pauVar5,pSVar7,iVar8);
  *(undefined4 *)(param_1 + 0x14) = uVar2;
  if (*(int *)(param_1 + 0x14) == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar2 = 0xffffffff;
  }
  else {
    FUN_00471160(*(int *)(*(int *)(param_1 + 0x14) + 4),*(int *)(param_1 + 0x14));
    puVar6 = FUN_0049e980();
    *(undefined4 **)(param_1 + 0x18) = puVar6;
    if (0x11 < *(int *)(param_1 + 0x1c)) {
      FUN_004a0ff0(*(int *)(param_1 + 0x18));
    }
    uVar2 = 0;
  }
  return uVar2;
}

