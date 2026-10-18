/* Entry: 0x00542036; symbol: thunk_FUN_005463d0; body bytes: 5 */

undefined4 thunk_FUN_005463d0(void)

{
  int iVar1;
  
  iVar1 = FUN_00546876();
  if (iVar1 != 0) {
    return *(undefined4 *)(iVar1 + 0x18);
  }
  return 0;
}

