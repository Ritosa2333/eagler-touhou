/* Entry: 0x004505f0; symbol: FUN_004505f0; body bytes: 64 */

void FUN_004505f0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = FUN_0044cd00();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x58) = param_2;
    *(undefined4 *)(iVar1 + 0x5c) = param_3;
  }
  return;
}

