/* Entry: 0x0054187d; symbol: FUN_0054187d; body bytes: 69 */

DWORD FUN_0054187d(void)

{
  LARGE_INTEGER LVar1;
  LARGE_INTEGER local_c;
  
  local_c.s = DAT_005e4d30.s;
  LVar1.s = local_c.s;
  local_c.s.LowPart = DAT_005e4d30.s.LowPart;
  local_c.s.HighPart = DAT_005e4d30.s.HighPart;
  if (local_c.s.LowPart == 0 && local_c.s.HighPart == 0) {
    local_c.s = LVar1.s;
    QueryPerformanceFrequency(&local_c);
    DAT_005e4d30.s = local_c.s;
  }
  return local_c.s.LowPart;
}

