/* Entry: 0x00559c80; symbol: FUN_00559c80; body bytes: 172 */

uint FUN_00559c80(void)

{
  uint uVar1;
  uint uVar2;
  ushort in_FPUStatusWord;
  
  uVar1 = (uint)in_FPUStatusWord;
  uVar1 = ((in_FPUStatusWord >> 2 & 8 | uVar1 & 0x10) >> 2 | uVar1 & 8) >> 1 |
          ((uVar1 & 2) << 3 | uVar1 & 4) * 2 | (uVar1 & 1) << 4;
  if (DAT_005e5288 < 1) {
    uVar2 = 0;
  }
  else {
    uVar2 = MXCSR & 0x3f;
  }
  uVar2 = ((uVar2 >> 2 & 8 | uVar2 & 0x10) >> 2 | uVar2 & 8) >> 1 |
          ((uVar2 & 2) << 3 | uVar2 & 4) * 2 | (uVar2 & 1) << 4;
  return (uVar2 << 8 | uVar1) << 0x10 | uVar2 | uVar1;
}

