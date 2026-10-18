/* Entry: 0x00424350; symbol: FUN_00424350; body bytes: 264 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_00424350(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  uint uVar3;
  float10 fVar4;
  
  uVar3 = *(uint *)(param_1 + 0x24) >> 0x18;
  if (DAT_005ba518 == 0) {
    bVar1 = FUN_00423560((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c));
    if (bVar1) {
      fVar4 = FUN_00423bd0(param_1 + 0x30);
      *(uint *)(param_1 + 0x18) =
           uVar3 - (int)(((float)fVar4 * (float)((double)uVar3 + _DAT_0056cdd0)) /
                        (float)*(int *)(param_1 + 0x1c));
      if (*(int *)(param_1 + 0x18) < 0) {
        *(undefined4 *)(param_1 + 0x18) = 0;
      }
    }
    else {
      *(undefined4 *)(param_1 + 0x18) = 0;
      *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + -1;
      if (*(int *)(param_1 + 0x20) < 1) {
        return 7;
      }
      FUN_00423520((void *)(param_1 + 0x30),0);
    }
    FUN_00423540((undefined4 *)(param_1 + 0x30));
    uVar2 = 1;
  }
  else {
    uVar2 = 7;
  }
  return uVar2;
}

