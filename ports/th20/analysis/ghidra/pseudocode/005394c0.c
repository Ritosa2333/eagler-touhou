/* Entry: 0x005394c0; symbol: FUN_005394c0; body bytes: 142 */

void __cdecl FUN_005394c0(int param_1)

{
  int iVar1;
  
  if (*(int *)(&DAT_005c8b38 + param_1 * 0xc) != 0) {
    if (*(int *)(&DAT_005c8b40 + param_1 * 0xc) == 0) {
      FUN_00539180(param_1,*(int *)(&DAT_005c8b3c + param_1 * 0xc));
    }
    else if (*(int *)(&DAT_005c8b3c + param_1 * 0xc) == 0) {
      FUN_00539180(param_1,*(int *)(&DAT_005c8b40 + param_1 * 0xc));
    }
    else {
      iVar1 = FUN_00539960(param_1);
      FUN_005394c0(iVar1);
      FUN_00539a10(param_1,iVar1);
    }
  }
  return;
}

