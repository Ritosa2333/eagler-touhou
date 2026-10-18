/* Entry: 0x0055299d; symbol: FUN_0055299d; body bytes: 27 */

uint __cdecl FUN_0055299d(uint param_1)

{
  byte bVar1;
  
  bVar1 = (byte)DAT_005b25c0 & 0x1f;
  return (param_1 ^ DAT_005b25c0) >> bVar1 | (param_1 ^ DAT_005b25c0) << 0x20 - bVar1;
}

