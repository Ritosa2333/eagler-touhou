/* Entry: 0x004240c0; symbol: FUN_004240c0; body bytes: 180 */

undefined4 __cdecl FUN_004240c0(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  float10 fVar3;
  
  if (DAT_005ba518 == 0) {
    if (*(int *)(param_1 + 0x1c) != 0) {
      fVar3 = FUN_00423bd0(param_1 + 0x30);
      *(int *)(param_1 + 0x18) =
           (int)(DAT_0056cd9c - ((float)fVar3 * DAT_0056cd9c) / (float)*(int *)(param_1 + 0x1c));
      if (*(int *)(param_1 + 0x18) < 0) {
        *(undefined4 *)(param_1 + 0x18) = 0;
      }
    }
    bVar1 = FUN_004235c0((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c));
    if (bVar1) {
      uVar2 = 7;
    }
    else {
      FUN_00423540((undefined4 *)(param_1 + 0x30));
      uVar2 = 1;
    }
  }
  else {
    uVar2 = 7;
  }
  return uVar2;
}

