/* Entry: 0x00542c70; symbol: FUN_00542c70; body bytes: 162 */

ulonglong FUN_00542c70(void)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined1 auVar6 [16];
  double in_XMM0_Qa;
  uint local_8;
  
  if (((byte)DAT_005b2610 & 0x20) == 0) {
    auVar6._8_8_ = 0;
    auVar6._0_8_ = in_XMM0_Qa;
    auVar6 = vcvttpd2uqq_avx512vl(auVar6);
    return auVar6._0_8_;
  }
  local_8 = SUB84(in_XMM0_Qa,0);
  uVar3 = (uint)((ulonglong)in_XMM0_Qa >> 0x20);
  uVar4 = uVar3 & 0x7fffffff;
  if ((longlong)in_XMM0_Qa < 0) {
    if (uVar4 < 0x3ff00000) {
      return 0;
    }
  }
  else {
    if (uVar4 < 0x41e00000) {
      return (ulonglong)(uint)(int)in_XMM0_Qa;
    }
    uVar5 = uVar3 & 0x1fffff | 0x100000;
    uVar3 = (uVar4 >> 0x14) - 0x433;
    bVar2 = (byte)uVar3;
    if (uVar4 >> 0x14 < 0x433) {
      bVar1 = -bVar2 & 0x1f;
      return CONCAT44(uVar5 >> (-bVar2 & 0x1f),local_8 >> bVar1 | uVar5 << 0x20 - bVar1);
    }
    if (uVar3 < 0xc) {
      return CONCAT44(uVar5 << (bVar2 & 0x1f) | local_8 >> 0x20 - (bVar2 & 0x1f),
                      local_8 << (bVar2 & 0x1f));
    }
  }
  return 0xffffffffffffffff;
}

