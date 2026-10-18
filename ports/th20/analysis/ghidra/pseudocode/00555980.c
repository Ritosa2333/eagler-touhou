/* Entry: 0x00555980; symbol: FUN_00555980; body bytes: 38 */

void __cdecl FUN_00555980(uint param_1)

{
  byte bVar1;
  
  bVar1 = 0x20 - ((byte)DAT_005b25c0 & 0x1f) & 0x1f;
  DAT_005e55d4 = (param_1 >> bVar1 | param_1 << 0x20 - bVar1) ^ DAT_005b25c0;
  return;
}

