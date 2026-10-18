/* Entry: 0x00555e4f; symbol: FUN_00555e4f; body bytes: 11 */

undefined4 FUN_00555e4f(void)

{
  undefined4 uVar1;
  
  uVar1 = DAT_005e55dc;
  LOCK();
  DAT_005e55dc = 1;
  UNLOCK();
  return uVar1;
}

