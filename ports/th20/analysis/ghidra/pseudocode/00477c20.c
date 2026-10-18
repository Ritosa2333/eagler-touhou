/* Entry: 0x00477c20; symbol: FUN_00477c20; body bytes: 130 */

undefined4 __fastcall FUN_00477c20(int param_1)

{
  bool bVar1;
  int iVar2;
  
  FUN_004776a0(DAT_005c0028,*(undefined4 *)(param_1 + 0x34));
  if (*(int *)(param_1 + 0x14) != 0) {
    bVar1 = FUN_00478130(param_1);
    if (bVar1) {
      iVar2 = FUN_00478060(0);
      FUN_00478190(iVar2);
    }
    iVar2 = (**(code **)(**(int **)(param_1 + 0x14) + 8))();
    if (iVar2 != 0) {
      FUN_00477f00(param_1);
    }
  }
  FUN_00423540((undefined4 *)(param_1 + 0x1c));
  return 1;
}

