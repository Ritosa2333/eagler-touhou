/* Entry: 0x00557b1e; symbol: FUN_00557b1e; body bytes: 26 */

undefined4 FUN_00557b1e(void)

{
  DWORD DVar1;
  
  DVar1 = DAT_005b2784;
  if (DAT_005b2784 != 0xffffffff) {
    DVar1 = FUN_005580fd(DAT_005b2784);
    DAT_005b2784 = 0xffffffff;
  }
  return CONCAT31((int3)(DVar1 >> 8),1);
}

