/* Entry: 0x00450270; symbol: FUN_00450270; body bytes: 66 */

void FUN_00450270(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  
  iVar1 = FUN_0044cd00();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x5bc) = *param_2;
    *(undefined4 *)(iVar1 + 0x5c0) = param_2[1];
    *(undefined4 *)(iVar1 + 0x5c4) = param_2[2];
  }
  return;
}

