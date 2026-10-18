/* Entry: 0x00424180; symbol: FUN_00424180; body bytes: 221 */

undefined4 __cdecl FUN_00424180(int param_1)

{
  bool bVar1;
  float10 fVar2;
  
  if (*(int *)(param_1 + 0x2c) == 0) {
    if ((*(int *)(param_1 + 0x1c) != 0) &&
       (bVar1 = FUN_00423590((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c)), bVar1)) {
      fVar2 = FUN_00423bd0(param_1 + 0x30);
      *(int *)(param_1 + 0x18) =
           (int)(((float)fVar2 * DAT_0056cd98) / (float)*(int *)(param_1 + 0x1c));
    }
  }
  else {
    bVar1 = FUN_00423590((void *)(param_1 + 0x30),8);
    if (!bVar1) {
      return 7;
    }
    fVar2 = FUN_00423bd0(param_1 + 0x30);
    *(int *)(param_1 + 0x18) = 0x80 - (int)(((float)fVar2 * DAT_0056cd98) / DAT_0056d7bc);
  }
  FUN_00423540((undefined4 *)(param_1 + 0x30));
  return 1;
}

