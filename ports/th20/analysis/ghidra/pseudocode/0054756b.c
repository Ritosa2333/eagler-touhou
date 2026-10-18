/* Entry: 0x0054756b; symbol: FUN_0054756b; body bytes: 20 */

void FUN_0054756b(void)

{
  int iVar1;
  
  iVar1 = ___vcrt_getptd();
  if (0 < *(int *)(iVar1 + 0x18)) {
    iVar1 = ___vcrt_getptd();
    *(int *)(iVar1 + 0x18) = *(int *)(iVar1 + 0x18) + -1;
  }
  return;
}

