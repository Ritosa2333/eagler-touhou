/* Entry: 0x00541026; symbol: FUN_00541026; body bytes: 31 */

void FUN_00541026(HANDLE param_1)

{
  BOOL BVar1;
  
  if (param_1 != (HANDLE)0xffffffff) {
    BVar1 = CloseHandle(param_1);
    if (BVar1 == 0) {
                    /* WARNING: Subroutine does not return */
      _abort();
    }
  }
  return;
}

