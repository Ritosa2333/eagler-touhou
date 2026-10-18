/* Entry: 0x004e1710; symbol: FUN_004e1710; body bytes: 161 */

void __fastcall FUN_004e1710(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  *(int *)(param_1 + 0xcc) = *(int *)(param_1 + 0xcc) + -1;
  if (*(int *)(param_1 + 0xcc) < 0) {
    *(undefined4 *)(param_1 + 0xcc) = 0;
  }
  iVar1 = FUN_004b7e20(param_1);
  if (iVar1 < *(int *)(param_1 + 0xcc)) {
    uVar2 = FUN_004b7e20(param_1);
    *(undefined4 *)(param_1 + 0xcc) = uVar2;
  }
  if (DAT_005c06a4 != (void *)0x0) {
    iVar1 = FUN_004b7e20(param_1);
    iVar3 = FUN_004b7e70(param_1);
    iVar4 = FUN_00477ff0(param_1);
    FUN_004b8650(DAT_005c06a4,iVar4,iVar3,iVar1);
  }
  return;
}

