/* Entry: 0x00543320; symbol: FUN_00543320; body bytes: 124 */

uint FUN_00543320(void)

{
  uint uVar1;
  ushort uVar2;
  float10 in_ST0;
  undefined4 uStack_1c;
  undefined2 uStack_18;
  
  if (1 < DAT_005e5288) {
    return (int)in_ST0;
  }
  uStack_1c = (uint)((unkuint10)in_ST0 >> 0x20);
  uStack_18 = (ushort)((unkuint10)in_ST0 >> 0x40);
  uVar2 = uStack_18 & 0x7fff;
  uVar1 = (uint)((short)uStack_18 < 0);
  if (uVar2 < 0x3fff) {
    return 0;
  }
  if ((int)uStack_1c < 0) {
    if (uVar2 < 0x401e) {
      return (uStack_1c >> (0x3eU - (char)uVar2 & 0x1f) ^ -uVar1) + uVar1;
    }
    if (((uVar2 < 0x401f) && (uVar1 != 0)) && (uStack_1c == 0x80000000)) {
      return uStack_1c;
    }
  }
  return 0x80000000;
}

