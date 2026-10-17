/* Entry: 0x00555910; symbol: FUN_00555910; body bytes: 29 */

bool FUN_00555910(void)

{
  byte bVar1;
  
  bVar1 = (byte)DAT_005b25c0 & 0x1f;
  return (DAT_005e55d4 ^ DAT_005b25c0) >> bVar1 != 0 ||
         (DAT_005e55d4 ^ DAT_005b25c0) << 0x20 - bVar1 != 0;
}

