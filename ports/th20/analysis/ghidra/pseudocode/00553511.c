/* Entry: 0x00553511; symbol: thunk_FUN_00553148; body bytes: 5 */

int thunk_FUN_00553148(void)

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

