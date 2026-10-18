/* Entry: 0x00541927; symbol: FUN_00541927; body bytes: 45 */

undefined4 __fastcall FUN_00541927(undefined4 param_1)

{
  LPCRITICAL_SECTION p_Var1;
  
  LOCK();
  DAT_005b257c = DAT_005b257c + 1;
  UNLOCK();
  if (DAT_005b257c == 0) {
    p_Var1 = (LPCRITICAL_SECTION)&DAT_005e4d40;
    do {
      FUN_0054248e(p_Var1);
      p_Var1 = p_Var1 + 1;
    } while (p_Var1 != (LPCRITICAL_SECTION)&DAT_005e4e00);
  }
  return param_1;
}

