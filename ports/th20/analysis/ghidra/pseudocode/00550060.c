/* Entry: 0x00550060; symbol: FUN_00550060; body bytes: 123 */

float10 __cdecl FUN_00550060(float param_1)

{
  float fVar1;
  byte bVar2;
  uint uVar3;
  
  fVar1 = ABS(param_1);
  if (0x4affffff < (uint)fVar1) {
    return (float10)param_1;
  }
  if (0x3f7fffff < (uint)fVar1) {
    bVar2 = 0x95 - (char)((uint)fVar1 >> 0x17);
    uVar3 = (uint)param_1 >> (bVar2 & 0x1f);
    return (float10)(float)((uVar3 & 1) + uVar3 << (bVar2 & 0x1f));
  }
  if ((uint)fVar1 < 0x3f000000) {
    return (float10)(float)((uint)param_1 & 0x80000000);
  }
  return (float10)(float)((uint)param_1 & 0xbf800000 | 0x3f800000);
}

