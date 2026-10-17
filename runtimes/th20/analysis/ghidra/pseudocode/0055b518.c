/* Entry: 0x0055b518; symbol: FUN_0055b518; body bytes: 19 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __cdecl FUN_0055b518(double param_1)

{
  int iVar1;
  ushort uVar2;
  void *in_ECX;
  float10 extraout_ST0;
  double dVar3;
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
  
  uVar2 = ((ushort)((ulonglong)param_1 >> 0x30) & 0x7fff) + 0xc7e0;
  if (uVar2 < 0x8a9) {
    dVar10 = (_DAT_0059a880 * param_1 + _DAT_0059a890) - _DAT_0059a890;
    dVar11 = (_UNK_0059a888 * param_1 + _UNK_0059a898) - _UNK_0059a898;
    dVar3 = param_1 - _DAT_0059a8a0 * dVar10;
    dVar7 = param_1 - _UNK_0059a8a8 * dVar11;
    dVar4 = dVar3 - _DAT_0059a8b0 * dVar10;
    dVar8 = dVar7 - _UNK_0059a8b8 * dVar11;
    iVar1 = ((int)ROUND(_DAT_0059a880 * param_1) + 0x72900U & 0x1f) * 0xb0;
    dVar15 = (double)((ulonglong)(DAT_0059a8d0 * dVar10 + dVar3) & _DAT_0059a8e0);
    dVar16 = DAT_0059a8d8 / dVar15;
    dVar5 = dVar8 * dVar8;
    dVar9 = dVar8 * dVar8;
    dVar6 = dVar5 * dVar5;
    dVar13 = dVar8 * *(double *)(&DAT_005a1070 + iVar1) + dVar8 * *(double *)(&DAT_005a1078 + iVar1)
    ;
    dVar12 = (double)(*(ulonglong *)(&DAT_005a1088 + iVar1) & (ulonglong)dVar16) -
             *(double *)(&DAT_005a1060 + iVar1);
    dVar14 = dVar13 - dVar12;
    return (float10)(((dVar6 * dVar6 *
                       (*(double *)(&DAT_005a0ff0 + iVar1) * dVar8 +
                        *(double *)(&DAT_005a0fe0 + iVar1) +
                        (*(double *)(&DAT_005a1010 + iVar1) * dVar8 +
                        *(double *)(&DAT_005a1000 + iVar1)) * dVar5 +
                        *(double *)(&DAT_005a1020 + iVar1) * dVar6 +
                       (*(double *)(&DAT_005a1040 + iVar1) * dVar8 +
                        *(double *)(&DAT_005a1030 + iVar1) +
                       *(double *)(&DAT_005a1050 + iVar1) * dVar5) * dVar8 * dVar6) +
                       *(double *)(&UNK_005a0ff8 + iVar1) * dVar8 +
                       *(double *)(&DAT_005a0fe8 + iVar1) +
                       (*(double *)(&UNK_005a1018 + iVar1) * dVar8 +
                       *(double *)(&DAT_005a1008 + iVar1)) * dVar9 +
                       *(double *)(&UNK_005a1028 + iVar1) * dVar9 * dVar9 +
                       (*(double *)(&UNK_005a1048 + iVar1) * dVar8 +
                        *(double *)(&DAT_005a1038 + iVar1) +
                       *(double *)(&UNK_005a1058 + iVar1) * dVar9) * dVar8 * dVar9 * dVar9 +
                       (*(double *)(&DAT_005a1070 + iVar1) + *(double *)(&DAT_005a1078 + iVar1)) *
                       (((dVar7 - dVar8) - _UNK_0059a8b8 * dVar11) - dVar11 * _UNK_0059a8c8) +
                       *(double *)(&DAT_005a1068 + iVar1) +
                       dVar8 * *(double *)(&DAT_005a1078 + iVar1) +
                       (dVar8 * *(double *)(&DAT_005a1070 + iVar1) - dVar13) +
                      (dVar13 - (dVar12 + dVar14))) -
                     ((DAT_0059a8d8 -
                      dVar15 * (double)(*(ulonglong *)(&DAT_005a1088 + iVar1) & (ulonglong)dVar16))
                     - ((((dVar3 - dVar4) - _DAT_0059a8b0 * dVar10) - dVar10 * _DAT_0059a8c0) +
                       (dVar4 - dVar15)) * dVar16) * dVar16 * *(double *)(&DAT_005a1080 + iVar1)) +
                    dVar14);
  }
  if ((short)uVar2 < 0x8a9) {
    return (float10)((DAT_0059a8f8 * param_1 + param_1) * _DAT_0059a900);
  }
  FUN_0055b45f(in_ECX,param_1._0_4_,param_1._4_4_);
  return extraout_ST0;
}

