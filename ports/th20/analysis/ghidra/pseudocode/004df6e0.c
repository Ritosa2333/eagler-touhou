/* Entry: 0x004df6e0; symbol: FUN_004df6e0; body bytes: 107 */

void FUN_004df6e0(void)

{
  int iVar1;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,0xe,(char **)"notice.anm");
  *(int *)(DAT_005c5b38 + 0x9c) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571f5c);
  }
  else {
    FUN_004ba8b0((undefined4 *)&DAT_005c5ad0);
    FUN_00412d80(*(int *)(DAT_005c5b38 + 8));
    FUN_00412d80(*(int *)(DAT_005c5b38 + 0xc));
  }
  return;
}

