/* Entry: 0x004780a0; symbol: FUN_004780a0; body bytes: 131 */

undefined1 __fastcall FUN_004780a0(int param_1)

{
  bool bVar1;
  int iVar2;
  undefined3 extraout_var;
  
  iVar2 = FUN_0041caf0(*(int *)(param_1 + 0x38));
  iVar2 = FUN_00477ff0(iVar2);
  if ((((0 < iVar2) && (bVar1 = FUN_00478130(param_1), !bVar1)) && (DAT_005c06a4 != 0)) &&
     ((bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) == 0 &&
      (iVar2 = FUN_00412730(*(int *)(param_1 + 0x38)), iVar2 != 0)))) {
    iVar2 = FUN_00412730(*(int *)(param_1 + 0x38));
    iVar2 = FUN_00478080(iVar2);
    if (iVar2 != 0) {
      return 1;
    }
  }
  return 0;
}

