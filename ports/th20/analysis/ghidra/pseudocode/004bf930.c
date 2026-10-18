/* Entry: 0x004bf930; symbol: FUN_004bf930; body bytes: 89 */

void FUN_004bf930(void)

{
  int iVar1;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,0xe,(char **)"help.anm");
  *(int *)(DAT_005c4d24 + 0xb0) = iVar1;
  if (iVar1 != 0) {
    FUN_004ba8b0((undefined4 *)&DAT_005c5ad0);
    FUN_00412d80(*(int *)(DAT_005c4d24 + 8));
    FUN_00412d80(*(int *)(DAT_005c4d24 + 0xc));
  }
  return;
}

