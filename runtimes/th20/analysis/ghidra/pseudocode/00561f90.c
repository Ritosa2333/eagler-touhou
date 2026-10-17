/* Entry: 0x00561f90; symbol: FUN_00561f90; body bytes: 281 */

uint FUN_00561f90(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  ushort in_FPUStatusWord;
  
  uVar3 = MXCSR;
  if (0 < DAT_005e5288) {
    uVar5 = 0;
    if ((in_FPUStatusWord & 0x3f) != 0) {
      uVar1 = (in_FPUStatusWord & 1) << 4;
      uVar5 = uVar1 | 8;
      if ((in_FPUStatusWord & 4) == 0) {
        uVar5 = uVar1;
      }
      uVar1 = uVar5 | 4;
      if ((in_FPUStatusWord & 8) == 0) {
        uVar1 = uVar5;
      }
      uVar2 = uVar1 | 2;
      if ((in_FPUStatusWord & 0x10) == 0) {
        uVar2 = uVar1;
      }
      uVar5 = uVar2 | 1;
      if ((in_FPUStatusWord & 0x20) == 0) {
        uVar5 = uVar2;
      }
      if ((in_FPUStatusWord & 2) != 0) {
        uVar5 = uVar5 | 0x80000;
      }
    }
    MXCSR = MXCSR & 0xffffffc0;
    uVar1 = 0;
    if ((uVar3 & 0x3f) != 0) {
      uVar2 = (uVar3 & 1) << 4;
      uVar1 = uVar2 | 8;
      if ((uVar3 & 4) == 0) {
        uVar1 = uVar2;
      }
      uVar2 = uVar1 | 4;
      if ((uVar3 & 8) == 0) {
        uVar2 = uVar1;
      }
      uVar4 = uVar2 | 2;
      if ((uVar3 & 0x10) == 0) {
        uVar4 = uVar2;
      }
      uVar1 = uVar4 | 1;
      if ((uVar3 & 0x20) == 0) {
        uVar1 = uVar4;
      }
      if ((uVar3 & 2) != 0) {
        uVar1 = uVar1 | 0x80000;
      }
    }
    return uVar1 | uVar5;
  }
  uVar3 = 0;
  if ((in_FPUStatusWord & 0x3f) != 0) {
    uVar5 = (in_FPUStatusWord & 1) << 4;
    uVar3 = uVar5 | 8;
    if ((in_FPUStatusWord & 4) == 0) {
      uVar3 = uVar5;
    }
    uVar5 = uVar3 | 4;
    if ((in_FPUStatusWord & 8) == 0) {
      uVar5 = uVar3;
    }
    uVar1 = uVar5 | 2;
    if ((in_FPUStatusWord & 0x10) == 0) {
      uVar1 = uVar5;
    }
    uVar3 = uVar1 | 1;
    if ((in_FPUStatusWord & 0x20) == 0) {
      uVar3 = uVar1;
    }
    if ((in_FPUStatusWord & 2) != 0) {
      uVar3 = uVar3 | 0x80000;
    }
  }
  return uVar3;
}

