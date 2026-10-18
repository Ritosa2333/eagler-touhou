/* Entry: 0x005593ca; symbol: FUN_005593ca; body bytes: 65 */

uint __cdecl FUN_005593ca(byte *param_1)

{
  byte bVar1;
  uint uVar2;
  undefined4 uStack_8;
  
  bVar1 = *param_1;
  if (-1 < (char)bVar1) {
    return (uint)(bVar1 != 0);
  }
  if ((bVar1 & 0xe0) == 0xc0) {
    uStack_8 = 2;
  }
  else {
    if ((bVar1 & 0xf0) != 0xe0) {
      uVar2 = 0xffffffff;
      if ((bVar1 & 0xf8) == 0xf0) {
        uVar2 = 4;
      }
      return uVar2;
    }
    uStack_8 = 3;
  }
  return uStack_8;
}

