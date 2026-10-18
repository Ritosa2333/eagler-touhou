/* Entry: 0x00553148; symbol: FUN_00553148; body bytes: 44 */

int FUN_00553148(void)

{
  int iVar1;
  
  iVar1 = DAT_005e55ac;
  if (DAT_005e55ac == 0) {
    if (DAT_005e55a8 != 0) {
      iVar1 = FUN_005531ce();
      if (iVar1 == 0) {
        return DAT_005e55ac;
      }
      iVar1 = FUN_0055345a();
      if (iVar1 == 0) {
        return DAT_005e55ac;
      }
    }
    iVar1 = 0;
  }
  return iVar1;
}

