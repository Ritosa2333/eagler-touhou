/* Entry: 0x00559f80; symbol: FUN_00559f80; body bytes: 265 */

void __cdecl FUN_00559f80(uint param_1)

{
  uint uVar1;
  
  uVar1 = param_1 >> 0x18;
  if (0 < DAT_005e5288) {
    MXCSR = MXCSR & 0xffffffc0 |
            -(uint)((uVar1 & 1) != 0) & 0x20 | -(uint)((uVar1 & 2) != 0) & 0x10 |
            -(uint)((uVar1 & 4) != 0) & 8 | -(uint)((uVar1 & 8) != 0) & 4 |
            (uint)((uVar1 & 0x10) != 0) | -(uint)((uVar1 & 0x20) != 0) & 2;
  }
  return;
}

