/* Entry: 0x0041b490; symbol: FUN_0041b490; body bytes: 81 */

void FUN_0041b490(void)

{
  SystemParametersInfoW(0x11,(uint)DAT_005b87e1,(PVOID)0x0,2);
  SystemParametersInfoW(0x55,(uint)DAT_005b87e2,(PVOID)0x0,2);
  SystemParametersInfoW(0x56,(uint)DAT_005b87e3,(PVOID)0x0,2);
  WINNLSEnableIME(0,1);
  return;
}

