/* Entry: 0x005468e1; symbol: FUN_005468e1; body bytes: 30 */

undefined4 FUN_005468e1(void)

{
  DWORD DVar1;
  
  DVar1 = DAT_005b2630;
  if (DAT_005b2630 != 0xffffffff) {
    DVar1 = ___vcrt_FlsFree(DAT_005b2630);
    DAT_005b2630 = 0xffffffff;
  }
  return CONCAT31((int3)(DVar1 >> 8),1);
}

