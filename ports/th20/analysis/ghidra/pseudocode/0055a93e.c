/* Entry: 0x0055a93e; symbol: FUN_0055a93e; body bytes: 681 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __cdecl FUN_0055a93e(double param_1)

{
  int iVar1;
  double dVar2;
  double dVar3;
  int iVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double in_XMM7_Qa;
  
  dVar2 = (double)((ulonglong)in_XMM7_Qa & _DAT_0059a540);
  dVar3 = (double)((ulonglong)in_XMM7_Qa & _UNK_0059a548);
  if (!NAN(dVar2) && !NAN(_DAT_0059a668)) {
    if (dVar2 < _DAT_0059a668) {
      if (_DAT_0059a658 <= dVar2) {
        if (dVar2 < _DAT_0059a650) {
          dVar2 = dVar2 * dVar2;
          dVar5 = dVar2 * dVar2;
          dVar3 = dVar3 * dVar3 * dVar3 * dVar3;
          return (float10)(in_XMM7_Qa -
                          ((((((((_DAT_0059a5d0 * dVar5 + _DAT_0059a5c0) * dVar5 + _DAT_0059a5b0) *
                                dVar5 + _DAT_0059a5a0) * dVar5 + _DAT_0059a590) * dVar5 +
                             _DAT_0059a580) * dVar5 + _DAT_0059a570) * dVar5 + _DAT_0059a560) *
                           dVar2 + ((((((_UNK_0059a5d8 * dVar3 + _UNK_0059a5c8) * dVar3 +
                                       _UNK_0059a5b8) * dVar3 + _UNK_0059a5a8) * dVar3 +
                                     _UNK_0059a598) * dVar3 + _UNK_0059a588) * dVar3 + _UNK_0059a578
                                   ) * dVar3 + _UNK_0059a568) * in_XMM7_Qa);
        }
        if (_DAT_0059a648 <= dVar2) {
          iVar1 = 0x300;
          dVar3 = DAT_0059a630 / dVar2;
        }
        else {
          iVar4 = (uint)((ulonglong)(dVar2 + DAT_0059a640) >> 0x2c) - (int)DAT_0059a620;
          iVar1 = iVar4 * 3;
          dVar3 = (dVar2 - *(double *)(&DAT_0059efe0 + iVar4 * 0x18)) /
                  (dVar2 * *(double *)(&DAT_0059efe0 + iVar4 * 0x18) + _DAT_0059a638);
        }
        dVar5 = dVar3 * dVar3;
        dVar6 = dVar5 * dVar5;
        dVar7 = dVar3 * dVar3 * dVar3 * dVar3;
        return (float10)(double)((ulonglong)
                                 (*(double *)(&DAT_0059efd0 + iVar1 * 8) -
                                 ((((((_DAT_0059a610 * dVar6 + _DAT_0059a600) * dVar6 +
                                     _DAT_0059a5f0) * dVar6 + _DAT_0059a5e0) * dVar5 +
                                   ((_UNK_0059a618 * dVar7 + _UNK_0059a608) * dVar7 + _UNK_0059a5f8)
                                   * dVar7 + _UNK_0059a5e8) * dVar3 -
                                  *(double *)(&DAT_0059efd8 + iVar1 * 8)) - dVar3)) |
                                (ulonglong)in_XMM7_Qa ^ (ulonglong)dVar2);
      }
      if (_DAT_0059a660 <= dVar2) {
        dVar2 = dVar2 * dVar2;
        dVar5 = dVar2 * dVar2;
        dVar3 = dVar3 * dVar3 * dVar3 * dVar3;
        return (float10)(in_XMM7_Qa -
                        ((((_DAT_0059a610 * dVar5 + _DAT_0059a600) * dVar5 + _DAT_0059a5f0) * dVar5
                         + _DAT_0059a5e0) * dVar2 +
                        ((_UNK_0059a618 * dVar3 + _UNK_0059a608) * dVar3 + _UNK_0059a5f8) * dVar3 +
                        _UNK_0059a5e8) * in_XMM7_Qa);
      }
      if (dVar2 == _DAT_0059a628) {
        return (float10)param_1;
      }
      if (dVar2 < _DAT_0059a670) {
        return (float10)(_DAT_0059a678 * _DAT_0059a678) + (float10)param_1;
      }
      return (float10)_DAT_0059a678 * (float10)_DAT_0059a678 + (float10)param_1;
    }
    if (!NAN((double)((ulonglong)dVar2 & (ulonglong)DAT_0059a530)) && !NAN(DAT_0059a530)) {
      return (float10)_DAT_0059a678 +
             (float10)*(double *)(&DAT_0059a550 + (param_1._4_4_ >> 0x1f) * -8);
    }
  }
  return (float10)param_1;
}

