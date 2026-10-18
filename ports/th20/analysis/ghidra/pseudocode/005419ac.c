/* Entry: 0x005419ac; symbol: FUN_005419ac; body bytes: 40 */

void __fastcall FUN_005419ac(int *param_1)

{
  int iVar1;
  
  iVar1 = *param_1;
  if (iVar1 != 0) {
    if (iVar1 < 8) {
      FUN_005424b1((LPCRITICAL_SECTION)(&DAT_005e4d40 + iVar1 * 0x18));
    }
    return;
  }
  ___acrt_unlock(4);
  return;
}

