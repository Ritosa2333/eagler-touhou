/* Entry: 0x00424260; symbol: FUN_00424260; body bytes: 239 */

undefined4 __cdecl FUN_00424260(int param_1)

{
  bool bVar1;
  uint uVar2;
  float10 fVar3;
  
  if (DAT_005ba518 != 0) {
    return 7;
  }
  if (*(int *)(param_1 + 0x1c) != 0) {
    bVar1 = FUN_00423560((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c));
    if (bVar1) {
      fVar3 = FUN_00423bd0(param_1 + 0x30);
      *(int *)(param_1 + 0x18) =
           (int)(((float)fVar3 * DAT_0056cd9c) / (float)*(int *)(param_1 + 0x1c));
    }
    else {
      *(undefined4 *)(param_1 + 0x18) = 0xff;
    }
    if (*(int *)(param_1 + 0x18) < 0) {
      *(undefined4 *)(param_1 + 0x18) = 0;
    }
  }
  bVar1 = FUN_004235c0((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c) + 2);
  if (!bVar1) {
    if ((DAT_005ba828 == 0) || (uVar2 = FUN_00424030(DAT_005ba828), uVar2 == 0)) {
      FUN_00423540((undefined4 *)(param_1 + 0x30));
    }
    return 1;
  }
  return 7;
}

