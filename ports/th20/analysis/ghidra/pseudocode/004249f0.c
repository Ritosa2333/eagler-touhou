/* Entry: 0x004249f0; symbol: FUN_004249f0; body bytes: 1040 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_004249f0(int param_1)

{
  bool bVar1;
  uint uVar2;
  float10 fVar3;
  float fVar4;
  float fVar5;
  
  if (DAT_005ba518 != 0) {
    return 7;
  }
  FUN_00423540((undefined4 *)(param_1 + 0x30));
  bVar1 = FUN_004235c0((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c));
  if (bVar1) {
    return 7;
  }
  fVar3 = FUN_00423bd0(param_1 + 0x30);
  fVar4 = (float)*(int *)(param_1 + 0x20) +
          ((float)fVar3 * (float)(*(int *)(param_1 + 0x24) - *(int *)(param_1 + 0x20))) /
          (float)*(int *)(param_1 + 0x1c);
  uVar2 = FUN_00423ea0(&DAT_005ba4c4,3);
  if (uVar2 != 0) {
    if (uVar2 == 1) {
      fVar5 = fVar4 * DAT_005b8818;
      DAT_005c5528 = fVar4;
      *(float *)(&DAT_005c5250 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      *(float *)(&DAT_005c50e4 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      *(float *)(&DAT_005c5800 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      goto LAB_00424c49;
    }
    if (uVar2 == 2) {
      DAT_005c5528 = (float)((uint)fVar4 ^ _DAT_0056d7d0);
      fVar5 = (float)((uint)fVar4 ^ _DAT_0056d7d0) * DAT_005b8818;
      *(float *)(&DAT_005c5250 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      *(float *)(&DAT_005c50e4 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      *(float *)(&DAT_005c5800 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      goto LAB_00424c49;
    }
  }
  DAT_005c5528 = 0.0;
  *(undefined4 *)(&DAT_005c5250 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c50e4 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c5800 + *(int *)(param_1 + 0x40) * 8) = 0;
LAB_00424c49:
  uVar2 = FUN_00423ea0(&DAT_005ba4c4,3);
  if (uVar2 != 0) {
    if (uVar2 == 1) {
      fVar5 = fVar4 * DAT_005b8818;
      DAT_005c552c = fVar4;
      *(float *)(&DAT_005c5254 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      *(float *)(&DAT_005c50e8 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      *(float *)(&DAT_005c5804 + *(int *)(param_1 + 0x40) * 8) = fVar5;
      return 1;
    }
    if (uVar2 == 2) {
      DAT_005c552c = (float)((uint)fVar4 ^ _DAT_0056d7d0);
      fVar4 = (float)((uint)fVar4 ^ _DAT_0056d7d0) * DAT_005b8818;
      *(float *)(&DAT_005c5254 + *(int *)(param_1 + 0x40) * 8) = fVar4;
      *(float *)(&DAT_005c50e8 + *(int *)(param_1 + 0x40) * 8) = fVar4;
      *(float *)(&DAT_005c5804 + *(int *)(param_1 + 0x40) * 8) = fVar4;
      return 1;
    }
  }
  *(undefined4 *)(&DAT_005c5254 + *(int *)(param_1 + 0x40) * 8) = 0;
  DAT_005c553c = 0;
  *(undefined4 *)(&DAT_005c50e8 + *(int *)(param_1 + 0x40) * 8) = 0;
  *(undefined4 *)(&DAT_005c5804 + *(int *)(param_1 + 0x40) * 8) = 0;
  return 1;
}

