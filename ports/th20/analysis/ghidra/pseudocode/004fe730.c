/* Entry: 0x004fe730; symbol: FUN_004fe730; body bytes: 46 */

bool __cdecl FUN_004fe730(uint param_1,byte param_2)

{
  return ((1 << (param_2 & 0x1f)) - 1U & param_1) == 0;
}

