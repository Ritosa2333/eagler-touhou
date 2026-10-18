/* Entry: 0x00543b4e; symbol: FUN_00543b4e; body bytes: 48 */

WORD FUN_00543b4e(void)

{
  WORD WVar1;
  _STARTUPINFOW local_48;
  
  _memset(&local_48,0,0x44);
  GetStartupInfoW(&local_48);
  WVar1 = 10;
  if (((byte)local_48.dwFlags & 1) != 0) {
    WVar1 = local_48.wShowWindow;
  }
  return WVar1;
}

