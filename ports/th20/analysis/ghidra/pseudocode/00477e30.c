/* Entry: 0x00477e30; symbol: FUN_00477e30; body bytes: 199 */

undefined4 __fastcall FUN_00477e30(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  void *this;
  
  FUN_0041f7c0(*(undefined4 **)(param_1 + 0x14));
  *(undefined4 *)(param_1 + 0x14) = 0;
  iVar1 = FUN_0041caf0(*(int *)(param_1 + 0x38));
  iVar1 = FUN_00412730(iVar1);
  uVar2 = (*(code *)(&PTR_FUN_005aefec)[iVar1])(*(undefined4 *)(param_1 + 0x34));
  *(undefined4 *)(param_1 + 0x14) = uVar2;
  (**(code **)(**(int **)(param_1 + 0x14) + 4))(*(undefined4 *)(param_1 + 0x34));
  *(undefined4 *)(param_1 + 0x18) = 1;
  FUN_00423520((void *)(param_1 + 0x1c),0);
  iVar1 = FUN_0041caf0(*(int *)(param_1 + 0x38));
  FUN_004e1710(iVar1);
  this = (void *)FUN_00464080(0);
  FUN_00477f60(this,-200);
  return 0;
}

