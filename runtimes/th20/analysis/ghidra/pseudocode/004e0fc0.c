/* Entry: 0x004e0fc0; symbol: FUN_004e0fc0; body bytes: 185 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004e0fc0(void)

{
  float fVar1;
  
  DAT_005c000c = (int)DAT_005c4f86;
  FUN_004e0f40(&DAT_005ba830);
  DAT_005c0010 = (int)DAT_005c4f87;
  if (DAT_005c4f87 == 0) {
    _DAT_005c0014 = -10000;
  }
  else {
    fVar1 = DAT_0056c8cc - (float)DAT_005c000c / DAT_0056e04c;
    fVar1 = fVar1 * fVar1;
    _DAT_005c0014 = (int)(DAT_0056cdb8 * (DAT_0056c8cc - fVar1 * fVar1)) + -5000;
  }
  return;
}

