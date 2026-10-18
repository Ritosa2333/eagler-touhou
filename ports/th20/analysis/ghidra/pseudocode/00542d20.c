/* Entry: 0x00542d20; symbol: FUN_00542d20; body bytes: 168 */

ulonglong FUN_00542d20(void)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  undefined1 auVar5 [16];
  double in_XMM0_Qa;
  ulonglong uVar6;
  undefined8 uVar7;
  uint local_8;
  
  uVar4 = (uint)((ulonglong)in_XMM0_Qa >> 0x20);
  if (uVar4 < 0x41e00000) {
    return (ulonglong)(uint)(int)in_XMM0_Qa;
  }
  if (uVar4 < 0x43f00000) {
    uVar6 = FUN_00542c70();
    return uVar6;
  }
  if (((byte)DAT_005b2610 & 0x20) == 0) {
    auVar5._8_8_ = 0;
    auVar5._0_8_ = in_XMM0_Qa;
    auVar5 = vcvttpd2qq_avx512vl(auVar5);
    return auVar5._0_8_;
  }
  local_8 = SUB84(in_XMM0_Qa,0);
  uVar3 = uVar4 & 0x7fffffff;
  if (0x41dfffff < uVar3) {
    if (0x43dfffff < uVar3) {
      return 0x8000000000000000;
    }
    if (-1 < (longlong)in_XMM0_Qa) {
      uVar4 = uVar4 & 0x1fffff | 0x100000;
      bVar2 = (char)(uVar3 >> 0x14) - 0x33;
      if (0x432 < uVar3 >> 0x14) {
        return CONCAT44(uVar4 << (bVar2 & 0x1f) | local_8 >> 0x20 - (bVar2 & 0x1f),
                        local_8 << (bVar2 & 0x1f));
      }
      bVar1 = -bVar2 & 0x1f;
      return CONCAT44(uVar4 >> (-bVar2 & 0x1f),local_8 >> bVar1 | uVar4 << 0x20 - bVar1);
    }
    uVar7 = dtol3_getbits();
    return CONCAT44(-((int)((ulonglong)uVar7 >> 0x20) + (uint)((int)uVar7 != 0)),-(int)uVar7);
  }
  return (ulonglong)(int)in_XMM0_Qa;
}

