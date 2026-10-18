/* Entry: 0x0055a2f8; symbol: FUN_0055a2f8; body bytes: 6 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 FUN_0055a2f8(double param_1)

{
  undefined1 auVar1 [12];
  ushort uVar2;
  undefined1 auVar3 [12];
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint in_XMM0_Dd;
  double dVar8;
  double dVar9;
  ulonglong in_XMM2_Qa;
  undefined1 auVar10 [16];
  undefined1 auVar11 [16];
  double dVar12;
  double dVar13;
  undefined1 auVar14 [16];
  undefined1 auVar15 [16];
  undefined4 uVar16;
  undefined1 auVar17 [16];
  ulonglong in_XMM7_Qa;
  double dVar18;
  ulonglong uVar19;
  undefined1 auVar20 [16];
  double dStack_c;
  
  uVar7 = (uint)((ulonglong)param_1 >> 0x20);
  uVar4 = uVar7 >> 0xc & 0x7ffff;
  if (uVar4 - 0x3fb00 < 0x3bb) {
    auVar10._0_8_ = DAT_0059a4d8 & (ulonglong)param_1;
    auVar10._8_8_ = in_XMM2_Qa & in_XMM7_Qa;
    iVar5 = (uVar7 >> 0xc & 0xfffc) - 0xfb00;
    iVar6 = iVar5 * 4;
    dVar13 = SUB168(auVar10 | ZEXT816(0x200000000000),0);
    dVar12 = SQRT(DAT_0059a4e0 - param_1 * param_1) * dVar13;
    dVar8 = param_1 * *(double *)(&DAT_0059e7d0 + iVar5 * 2) - dVar12;
    uVar4 = SUB164(auVar10 | ZEXT816(0x200000000000),4) & 0x80000000;
    dVar9 = dVar8 * dVar8;
    auVar3._4_8_ = 0;
    auVar3._0_4_ = uVar4;
    auVar11._0_12_ = auVar3 << 0x20;
    auVar11._12_4_ = uVar4;
    return (float10)((((DAT_0059a488 * dVar9 + DAT_0059a480) * dVar8 * dVar9 * dVar9 +
                      (DAT_0059a478 * dVar8 * dVar9 -
                      ((double)(*(ulonglong *)(&DAT_0059d8d0 + iVar6) ^ (ulonglong)uVar4 << 0x20) -
                      DAT_0059a410))) -
                     ((param_1 + dVar13) * (param_1 - dVar13)) /
                     (param_1 * *(double *)(&DAT_0059e7d0 + iVar5 * 2) + dVar12)) -
                    ((double)(*(ulonglong *)(&DAT_0059d8d8 + iVar6) ^ auVar11._8_8_) - DAT_0059a418)
                    );
  }
  if (uVar4 - 0x3febb < 0x41) {
    dVar18 = (double)((ulonglong)param_1 & 0xffffffc000000000);
    auVar1._8_4_ = in_XMM0_Dd >> 0xc;
    auVar1._0_8_ = param_1;
    dVar13 = DAT_0059a4e0 - dVar18 * dVar18;
    dVar8 = (param_1 + dVar18) * (param_1 - dVar18);
    dVar12 = SQRT(dVar13 - dVar8);
    iVar5 = -(uint)(SUB121(auVar1 >> 0x3f,0) & 1);
    dVar9 = (double)(DAT_0059a4d8 & (ulonglong)dVar12 |
                    ~DAT_0059a4d0 & (ulonglong)param_1 | 0x200000000000);
    auVar14._0_8_ = CONCAT44(iVar5,iVar5);
    auVar14._8_4_ = iVar5;
    auVar14._12_4_ = iVar5;
    iVar6 = (ushort)((ulonglong)((longlong)dVar12 << 2) >> 0x30) - 0xfec0;
    dVar12 = dVar12 * *(double *)(&DAT_0059e7d0 + iVar6 * 8);
    dVar18 = (dVar18 * dVar9 - dVar12) + (param_1 - dVar18) * dVar9;
    dVar8 = ((dVar13 - dVar9 * dVar9) - dVar8) / (dVar12 + dVar12 + dVar18);
    iVar6 = iVar6 * 0x10;
    dVar13 = (double)(auVar14._8_8_ & _UNK_0059a428) + *(double *)(&DAT_0059d8d8 + iVar6);
    dVar12 = dVar18 * dVar18;
    dVar9 = dVar8 + dVar13;
    return (float10)(double)((ulonglong)
                             ((DAT_0059a488 * dVar12 + DAT_0059a480) * dVar18 * dVar12 * dVar12 +
                              DAT_0059a478 * dVar18 * dVar12 +
                              (double)(auVar14._0_8_ & _DAT_0059a420) +
                              *(double *)(&DAT_0059d8d0 + iVar6) + dVar8 + (dVar13 - dVar9) + dVar9)
                            ^ (ulonglong)((ushort)iVar5 & 0x8000) << 0x30);
  }
  if (uVar4 - 0x3c300 < 0x3800) {
    dVar8 = param_1 * param_1;
    dVar9 = param_1 * param_1;
    dVar13 = param_1 * dVar8;
    return (float10)((DAT_0059a418 - param_1) +
                    (((DAT_0059a410 -
                      dVar13 * dVar13 * dVar13 *
                      (_DAT_0059a490 * dVar8 + _DAT_0059a4a0 + _DAT_0059a4b0 * dVar8 * dVar8)) -
                     param_1 * dVar9 *
                     (_UNK_0059a498 * dVar9 + _UNK_0059a4a8 + _UNK_0059a4b8 * dVar9 * dVar9)) -
                    (param_1 - (DAT_0059a418 - (DAT_0059a418 - param_1)))));
  }
  uVar2 = (ushort)((ulonglong)param_1 >> 0x30);
  if (uVar4 - 0x3fefc < 4) {
    dVar8 = DAT_0059a4e8 - (double)((ulonglong)param_1 & DAT_0059a4d0) * DAT_0059a4e8;
    uVar16 = (undefined4)((ulonglong)dVar8 >> 0x20);
    auVar20._8_4_ = SUB84(dVar8,0);
    auVar20._0_8_ = dVar8;
    auVar20._12_4_ = uVar16;
    dVar9 = SQRT(dVar8);
    dVar12 = auVar20._8_8_;
    auVar17._8_4_ = SUB84(dVar8,0);
    auVar17._0_8_ = dVar8;
    auVar17._12_4_ = uVar16;
    dVar13 = (double)(DAT_0059a450 & (ulonglong)dVar9);
    auVar15._8_4_ = SUB84(dVar9,0);
    auVar15._0_8_ = dVar9;
    auVar15._12_4_ = (int)((ulonglong)dVar9 >> 0x20);
    return (float10)(double)((ulonglong)
                             ((double)(-(ulonglong)(param_1 < 0.0) & _UNK_0059a428) +
                             (_DAT_0059a4a0 + _DAT_0059a490 * dVar8 + _DAT_0059a4b0 * dVar8 * dVar8)
                             * dVar8 * dVar8 * dVar8 * dVar8 * (dVar9 + dVar9) +
                             (double)(-(ulonglong)(param_1 < 0.0) & _DAT_0059a420) +
                             (_UNK_0059a4a8 + _UNK_0059a498 * dVar12 +
                             _UNK_0059a4b8 * dVar12 * dVar12) * auVar17._8_8_ *
                             (auVar15._8_8_ + auVar15._8_8_) +
                             ((dVar8 - dVar13 * dVar13) -
                             (dVar9 - dVar13) * ((dVar9 + dVar9) - (dVar9 - dVar13))) / dVar9 +
                             dVar9 + dVar9) ^ (ulonglong)(uVar2 & 0x8000) << 0x30);
  }
  if (uVar4 < 0x3ff00) {
    return (float10)(DAT_0059a418 + DAT_0059a410);
  }
  if ((uVar7 & 0x7fffffff) != 0x3ff00000 || SUB84(param_1,0) != 0) {
    if ((int)(((uVar7 & 0x7fffffff) + 0x80100000) - (uint)(SUB84(param_1,0) == 0)) < 0) {
      dStack_c = -NAN;
      iVar6 = 0x3a;
    }
    else {
      dStack_c = param_1 + 0.0;
      iVar6 = 0x3f0;
    }
    FUN_0055ba70(&param_1,&param_1,&dStack_c,iVar6);
    return (float10)dStack_c;
  }
  iVar6 = -(uint)(uVar2 >> 0xf);
  uVar19 = CONCAT44(iVar6,iVar6);
  return (float10)((double)(DAT_0059a438 & uVar19) + (double)(DAT_0059a430 & uVar19));
}

