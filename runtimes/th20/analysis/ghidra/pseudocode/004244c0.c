/* Entry: 0x004244c0; symbol: FUN_004244c0; body bytes: 1318 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_004244c0(int param_1)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  float10 fVar7;
  float fVar8;
  float local_8;
  
  if (DAT_005ba518 != 0) {
    return 7;
  }
  if (DAT_005ba828 == 0) {
    return 1;
  }
  uVar4 = FUN_00424030(DAT_005ba828);
  if ((((uVar4 != 0) || (uVar4 = FUN_00424060(DAT_005ba828), uVar4 != 0)) ||
      ((*(uint *)(DAT_005ba828 + 0xe8) >> 4 & 1) != 0)) ||
     (((*(uint *)(DAT_005ba828 + 0xe8) >> 5 & 1) != 0 ||
      ((*(uint *)(DAT_005ba828 + 0xe8) >> 6 & 1) != 0)))) {
    return 1;
  }
  FUN_00423540((undefined4 *)(param_1 + 0x30));
  bVar3 = FUN_00423560((void *)(param_1 + 0x30),*(int *)(param_1 + 0x20));
  if (bVar3) {
    fVar7 = FUN_00423bd0(param_1 + 0x30);
    local_8 = (float)fVar7 / (float)*(int *)(param_1 + 0x20);
  }
  else {
    iVar5 = FUN_0040ff90(param_1 + 0x30);
    if (iVar5 < *(int *)(param_1 + 0x20) + *(int *)(param_1 + 0x24)) {
      local_8 = DAT_0056c8cc;
    }
    else {
      iVar5 = FUN_0040ff90(param_1 + 0x30);
      if (*(int *)(param_1 + 0x20) + *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x28) <= iVar5) {
        return 7;
      }
      iVar5 = *(int *)(param_1 + 0x20);
      iVar1 = *(int *)(param_1 + 0x24);
      iVar2 = *(int *)(param_1 + 0x28);
      iVar6 = FUN_0040ff90(param_1 + 0x30);
      iVar6 = (iVar5 + iVar1 + iVar2) - iVar6;
      local_8 = (float)((double)iVar6 + *(double *)(&DAT_0056cdd0 + (iVar6 >> 0x1f) * -8)) /
                (float)((double)*(int *)(param_1 + 0x28) +
                       *(double *)(&DAT_0056cdd0 + (*(int *)(param_1 + 0x28) >> 0x1f) * -8));
    }
  }
  local_8 = (float)*(int *)(param_1 + 0x1c) * local_8;
  uVar4 = FUN_00423ea0(&DAT_005ba4c4,3);
  if (uVar4 != 0) {
    if (uVar4 == 1) {
      (&DAT_005c5528)[*(int *)(param_1 + 0x40) * 2] = local_8;
      fVar8 = local_8 * DAT_005b8818;
      *(float *)(&DAT_005c5250 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      *(float *)(&DAT_005c50e4 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      *(float *)(&DAT_005c5800 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      goto LAB_00424833;
    }
    if (uVar4 == 2) {
      (&DAT_005c5528)[*(int *)(param_1 + 0x40) * 2] = (uint)local_8 ^ _DAT_0056d7d0;
      fVar8 = (float)((uint)local_8 ^ _DAT_0056d7d0) * DAT_005b8818;
      *(float *)(&DAT_005c5250 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      *(float *)(&DAT_005c50e4 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      *(float *)(&DAT_005c5800 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      goto LAB_00424833;
    }
  }
  (&DAT_005c5528)[*(int *)(param_1 + 0x40) * 2] = 0;
  *(undefined4 *)(&DAT_005c5250 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c50e4 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c5800 + *(int *)(param_1 + 0x40) * 8) = 0;
LAB_00424833:
  uVar4 = FUN_00423ea0(&DAT_005ba4c4,3);
  if (uVar4 != 0) {
    if (uVar4 == 1) {
      (&DAT_005c552c)[*(int *)(param_1 + 0x40) * 2] = local_8;
      local_8 = local_8 * DAT_005b8818;
      *(float *)(&DAT_005c5254 + *(int *)(param_1 + 0x40) * 8) = local_8;
      *(float *)(&DAT_005c50e8 + *(int *)(param_1 + 0x40) * 8) = local_8;
      *(float *)(&DAT_005c5804 + *(int *)(param_1 + 0x40) * 8) = local_8;
      return 1;
    }
    if (uVar4 == 2) {
      (&DAT_005c552c)[*(int *)(param_1 + 0x40) * 2] = (uint)local_8 ^ _DAT_0056d7d0;
      fVar8 = (float)((uint)local_8 ^ _DAT_0056d7d0) * DAT_005b8818;
      *(float *)(&DAT_005c5254 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      *(float *)(&DAT_005c50e8 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      *(float *)(&DAT_005c5804 + *(int *)(param_1 + 0x40) * 8) = fVar8;
      return 1;
    }
  }
  (&DAT_005c552c)[*(int *)(param_1 + 0x40) * 2] = 0;
  *(undefined4 *)(&DAT_005c5254 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c50e8 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c5804 + *(int *)(param_1 + 0x40) * 8) = 0;
  return 1;
}

