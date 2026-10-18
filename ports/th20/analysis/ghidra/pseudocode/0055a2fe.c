/* Entry: 0x0055a2fe; symbol: FUN_0055a2fe; body bytes: 1334 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __cdecl FUN_0055a2fe(undefined8 param_1)

{
  undefined1 auVar1 [12];
  double dVar2;
  undefined1 auVar3 [12];
  uint uVar4;
  int iVar5;
  int iVar6;
  ulonglong uVar7;
  int in_XMM0_Da;
  uint in_XMM0_Db;
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
  undefined1 auVar19 [16];
  double local_c;
  
  dVar2 = (double)CONCAT44(in_XMM0_Db,in_XMM0_Da);
  uVar4 = in_XMM0_Db >> 0xc & 0x7ffff;
  if (uVar4 - 0x3fb00 < 0x3bb) {
    auVar10._0_8_ = DAT_0059a4d8 & (ulonglong)dVar2;
    auVar10._8_8_ = in_XMM2_Qa & in_XMM7_Qa;
    iVar5 = (in_XMM0_Db >> 0xc & 0xfffc) - 0xfb00;
    iVar6 = iVar5 * 4;
    dVar13 = SUB168(auVar10 | ZEXT816(0x200000000000),0);
    dVar12 = SQRT(DAT_0059a4e0 - dVar2 * dVar2) * dVar13;
    dVar8 = dVar2 * *(double *)(&DAT_0059e7d0 + iVar5 * 2) - dVar12;
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
                     ((dVar2 + dVar13) * ((double)CONCAT44(in_XMM0_Db,in_XMM0_Da) - dVar13)) /
                     (dVar2 * *(double *)(&DAT_0059e7d0 + iVar5 * 2) + dVar12)) -
                    ((double)(*(ulonglong *)(&DAT_0059d8d8 + iVar6) ^ auVar11._8_8_) - DAT_0059a418)
                    );
  }
  if (uVar4 - 0x3febb < 0x41) {
    dVar18 = (double)((ulonglong)(in_XMM0_Db >> 6) << 0x26);
    auVar1._4_4_ = in_XMM0_Db;
    auVar1._0_4_ = in_XMM0_Da;
    auVar1._8_4_ = in_XMM0_Dd >> 0xc;
    dVar13 = DAT_0059a4e0 - dVar18 * dVar18;
    dVar8 = ((double)CONCAT44(in_XMM0_Db,in_XMM0_Da) + dVar18) * (dVar2 - dVar18);
    dVar12 = SQRT(dVar13 - dVar8);
    iVar5 = -(uint)(SUB121(auVar1 >> 0x3f,0) & 1);
    dVar9 = (double)(DAT_0059a4d8 & (ulonglong)dVar12 |
                    ~DAT_0059a4d0 & CONCAT44(in_XMM0_Db,in_XMM0_Da) | 0x200000000000);
    auVar14._0_8_ = CONCAT44(iVar5,iVar5);
    auVar14._8_4_ = iVar5;
    auVar14._12_4_ = iVar5;
    iVar6 = (ushort)((ulonglong)((longlong)dVar12 << 2) >> 0x30) - 0xfec0;
    dVar12 = dVar12 * *(double *)(&DAT_0059e7d0 + iVar6 * 8);
    dVar18 = (dVar18 * dVar9 - dVar12) + (dVar2 - dVar18) * dVar9;
    dVar2 = ((dVar13 - dVar9 * dVar9) - dVar8) / (dVar12 + dVar12 + dVar18);
    iVar6 = iVar6 * 0x10;
    dVar9 = (double)(auVar14._8_8_ & _UNK_0059a428) + *(double *)(&DAT_0059d8d8 + iVar6);
    dVar13 = dVar18 * dVar18;
    dVar8 = dVar2 + dVar9;
    return (float10)(double)((ulonglong)
                             ((DAT_0059a488 * dVar13 + DAT_0059a480) * dVar18 * dVar13 * dVar13 +
                              DAT_0059a478 * dVar18 * dVar13 +
                              (double)(auVar14._0_8_ & _DAT_0059a420) +
                              *(double *)(&DAT_0059d8d0 + iVar6) + dVar2 + (dVar9 - dVar8) + dVar8)
                            ^ (ulonglong)((ushort)iVar5 & 0x8000) << 0x30);
  }
  if (uVar4 - 0x3c300 < 0x3800) {
    dVar8 = (double)CONCAT44(in_XMM0_Db,in_XMM0_Da) * (double)CONCAT44(in_XMM0_Db,in_XMM0_Da);
    dVar9 = (double)CONCAT44(in_XMM0_Db,in_XMM0_Da) * (double)CONCAT44(in_XMM0_Db,in_XMM0_Da);
    dVar13 = dVar2 * dVar8;
    return (float10)((DAT_0059a418 - dVar2) +
                    (((DAT_0059a410 -
                      dVar13 * dVar13 * dVar13 *
                      (_DAT_0059a490 * dVar8 + _DAT_0059a4a0 + _DAT_0059a4b0 * dVar8 * dVar8)) -
                     (double)CONCAT44(in_XMM0_Db,in_XMM0_Da) * dVar9 *
                     (_UNK_0059a498 * dVar9 + _UNK_0059a4a8 + _UNK_0059a4b8 * dVar9 * dVar9)) -
                    (dVar2 - (DAT_0059a418 - (DAT_0059a418 - dVar2)))));
  }
  if (uVar4 - 0x3fefc < 4) {
    dVar2 = DAT_0059a4e8 - (double)((ulonglong)dVar2 & DAT_0059a4d0) * DAT_0059a4e8;
    uVar16 = (undefined4)((ulonglong)dVar2 >> 0x20);
    auVar19._8_4_ = SUB84(dVar2,0);
    auVar19._0_8_ = dVar2;
    auVar19._12_4_ = uVar16;
    dVar8 = SQRT(dVar2);
    dVar13 = auVar19._8_8_;
    auVar17._8_4_ = SUB84(dVar2,0);
    auVar17._0_8_ = dVar2;
    auVar17._12_4_ = uVar16;
    uVar7 = -(ulonglong)((double)CONCAT44(in_XMM0_Db,in_XMM0_Da) < 0.0);
    dVar9 = (double)(DAT_0059a450 & (ulonglong)dVar8);
    auVar15._8_4_ = SUB84(dVar8,0);
    auVar15._0_8_ = dVar8;
    auVar15._12_4_ = (int)((ulonglong)dVar8 >> 0x20);
    return (float10)(double)((ulonglong)
                             ((double)(uVar7 & _UNK_0059a428) +
                             (_DAT_0059a4a0 + _DAT_0059a490 * dVar2 + _DAT_0059a4b0 * dVar2 * dVar2)
                             * dVar2 * dVar2 * dVar2 * dVar2 * (dVar8 + dVar8) +
                             (double)(uVar7 & _DAT_0059a420) +
                             (_UNK_0059a4a8 + _UNK_0059a498 * dVar13 +
                             _UNK_0059a4b8 * dVar13 * dVar13) * auVar17._8_8_ *
                             (auVar15._8_8_ + auVar15._8_8_) +
                             ((dVar2 - dVar9 * dVar9) -
                             (dVar8 - dVar9) * ((dVar8 + dVar8) - (dVar8 - dVar9))) / dVar8 +
                             dVar8 + dVar8) ^
                            (ulonglong)((ushort)(in_XMM0_Db >> 0x10) & 0x8000) << 0x30);
  }
  if (uVar4 < 0x3ff00) {
    return (float10)(DAT_0059a418 + DAT_0059a410);
  }
  if ((in_XMM0_Db & 0x7fffffff) != 0x3ff00000 || in_XMM0_Da != 0) {
    if ((int)((((uint)((ulonglong)param_1 >> 0x20) & 0x7fffffff) + 0x80100000) -
             (uint)((int)param_1 == 0)) < 0) {
      local_c = -NAN;
      iVar6 = 0x3a;
    }
    else {
      local_c = (double)CONCAT44(in_XMM0_Db,in_XMM0_Da) + 0.0;
      iVar6 = 0x3f0;
    }
    FUN_0055ba70(&param_1,&param_1,&local_c,iVar6);
    return (float10)local_c;
  }
  uVar7 = CONCAT44((int)in_XMM0_Db >> 0x1f,(int)in_XMM0_Db >> 0x1f);
  return (float10)((double)(DAT_0059a438 & uVar7) + (double)(DAT_0059a430 & uVar7));
}

