/* Entry: 0x0055acfe; symbol: FUN_0055acfe; body bytes: 395 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __thiscall FUN_0055acfe(void *param_1,int param_2,undefined4 param_3)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  float10 extraout_ST0;
  double in_XMM0_Qa;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  
  uVar1 = (ushort)((ulonglong)in_XMM0_Qa >> 0x30);
  uVar2 = (uVar1 & 0x7fff) + 0xcfd0;
  if (uVar2 < 0x10c6) {
    dVar9 = (DAT_0059a6d0 * in_XMM0_Qa + DAT_0059a6d8) - DAT_0059a6d8;
    iVar3 = ((int)ROUND(DAT_0059a6d0 * in_XMM0_Qa) + 0x1c7610U & 0x3f) * 0x20;
    dVar10 = _DAT_0059a6e0 * dVar9;
    dVar4 = in_XMM0_Qa - DAT_0059a6f0 * dVar9;
    dVar14 = in_XMM0_Qa - DAT_0059a6f0 * dVar9;
    dVar15 = dVar14 - dVar10;
    dVar5 = dVar4 - dVar10;
    dVar7 = dVar4 - _UNK_0059a6e8 * dVar9;
    dVar6 = dVar5 * dVar5;
    dVar8 = dVar7 * dVar7;
    dVar11 = *(double *)(&DAT_005a07e0 + iVar3) + *(double *)(&DAT_005a07f8 + iVar3);
    dVar12 = *(double *)(&DAT_005a07f8 + iVar3) * dVar15;
    dVar16 = dVar15 * *(double *)(&DAT_005a07e0 + iVar3);
    dVar13 = dVar12 + *(double *)(&DAT_005a07e8 + iVar3);
    dVar17 = dVar16 + dVar13;
    return (float10)(dVar17 + (dVar9 * _DAT_0059a6f8 - ((dVar14 - dVar15) - dVar10)) *
                              (*(double *)(&DAT_005a07e8 + iVar3) * dVar15 - dVar11) +
                              *(double *)(&DAT_005a07f0 + iVar3) +
                              (*(double *)(&DAT_005a07e8 + iVar3) - dVar13) + dVar12 +
                              (dVar13 - dVar17) + dVar16 +
                              (_DAT_0059a6a0 * dVar6 + _DAT_0059a690 +
                              (_DAT_0059a6c0 * dVar4 * dVar5 + _DAT_0059a6b0) * dVar6 * dVar6) *
                              dVar11 * dVar15 * dVar6 +
                              (_UNK_0059a6a8 * dVar8 + _UNK_0059a698 +
                              (_UNK_0059a6c8 * dVar4 * dVar7 + _UNK_0059a6b8) * dVar8 * dVar8) *
                              *(double *)(&DAT_005a07e8 + iVar3) * dVar8);
  }
  if ((short)uVar2 < 0x10c6) {
    return (float10)(DAT_0059a720 -
                    (double)((ulonglong)in_XMM0_Qa & 0xffffffffffff |
                            (ulonglong)(uVar1 & 0x7fff) << 0x30));
  }
  FUN_0055ac3f(param_1,param_2,param_3);
  return extraout_ST0;
}

