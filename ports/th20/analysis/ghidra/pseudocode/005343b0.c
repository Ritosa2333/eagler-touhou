/* Entry: 0x005343b0; symbol: FUN_005343b0; body bytes: 111 */

undefined1 FUN_005343b0(void)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  
  cVar1 = FUN_00530520();
  if ((cVar1 == '\0') && (iVar3 = FUN_00460830(0), iVar3 != 0)) {
    iVar3 = FUN_00460830(0);
    iVar3 = FUN_004ff6b0(iVar3);
    iVar3 = FUN_00506460(iVar3);
    if (-1 < iVar3) {
      iVar3 = FUN_00460830(0);
      bVar2 = FUN_004ff5e0(iVar3);
      if (!bVar2) {
        return 1;
      }
    }
  }
  return 0;
}

