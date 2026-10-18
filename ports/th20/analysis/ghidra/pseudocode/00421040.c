/* Entry: 0x00421040; symbol: FUN_00421040; body bytes: 132 */

void FUN_00421040(void)

{
  int local_10c;
  byte local_108 [256];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  GetKeyboardState(local_108);
  for (local_10c = 0; local_10c < 0x100; local_10c = local_10c + 1) {
    local_108[local_10c] = local_108[local_10c] & 0x7f;
  }
  SetKeyboardState(local_108);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

