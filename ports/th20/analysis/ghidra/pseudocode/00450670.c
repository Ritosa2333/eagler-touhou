/* Entry: 0x00450670; symbol: FUN_00450670; body bytes: 64 */

void FUN_00450670(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = FUN_0044cd00();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x50) = param_2;
    *(undefined4 *)(iVar1 + 0x54) = param_3;
  }
  return;
}

