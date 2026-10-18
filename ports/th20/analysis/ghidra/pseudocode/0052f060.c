/* Entry: 0x0052f060; symbol: FUN_0052f060; body bytes: 106 */

undefined * __cdecl FUN_0052f060(byte *param_1)

{
  byte bVar1;
  int local_c;
  char local_6;
  byte local_5;
  
  local_c = 0;
  local_5 = 0x77;
  local_6 = '\a';
  do {
    bVar1 = *param_1 ^ local_5;
    (&DAT_005c6860)[local_c] = bVar1;
    local_c = local_c + 1;
    param_1 = param_1 + 1;
    local_5 = local_5 + local_6;
    local_6 = local_6 + '\x10';
  } while (bVar1 != 0);
  return &DAT_005c6860;
}

