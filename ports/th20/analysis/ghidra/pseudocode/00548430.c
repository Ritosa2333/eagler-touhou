/* Entry: 0x00548430; symbol: FUN_00548430; body bytes: 469 */

uint __cdecl FUN_00548430(short *param_1,uint param_2)

{
  undefined1 auVar1 [32];
  short *psVar2;
  uint uVar3;
  undefined1 (*pauVar4) [32];
  uint uVar5;
  undefined1 auVar6 [16];
  
  if (4 < DAT_005e5288) {
    if (((uint)param_1 & 1) == 0) {
      uVar5 = (-(uint)(((uint)param_1 & 0x1f) != 0) & 0x20 - ((uint)param_1 & 0x1f)) >> 1;
      if (param_2 < uVar5) {
        uVar5 = param_2;
      }
      psVar2 = param_1;
      if (param_1 != param_1 + uVar5) {
        do {
          if (*psVar2 == 0) break;
          psVar2 = psVar2 + 1;
        } while (psVar2 != param_1 + uVar5);
      }
      uVar3 = (int)psVar2 - (int)param_1 >> 1;
      if (uVar3 == uVar5) {
        pauVar4 = (undefined1 (*) [32])(param_1 + uVar3);
        while ((pauVar4 !=
                (undefined1 (*) [32])
                (param_1 + ((param_2 - uVar5) - (param_2 - uVar5 & 0x1f)) + uVar3) &&
               (auVar1 = vpcmpeqw_avx2(SUB6432(ZEXT1664((undefined1  [16])0x0),0),*pauVar4),
               (((((((((((((((((((((((((((((((auVar1 >> 7 & (undefined1  [32])0x1) ==
                                             (undefined1  [32])0x0 &&
                                            (auVar1 >> 0xf & (undefined1  [32])0x1) ==
                                            (undefined1  [32])0x0) &&
                                           (auVar1 >> 0x17 & (undefined1  [32])0x1) ==
                                           (undefined1  [32])0x0) &&
                                          (auVar1 >> 0x1f & (undefined1  [32])0x1) ==
                                          (undefined1  [32])0x0) &&
                                         (auVar1 >> 0x27 & (undefined1  [32])0x1) ==
                                         (undefined1  [32])0x0) &&
                                        (auVar1 >> 0x2f & (undefined1  [32])0x1) ==
                                        (undefined1  [32])0x0) &&
                                       (auVar1 >> 0x37 & (undefined1  [32])0x1) ==
                                       (undefined1  [32])0x0) &&
                                      (auVar1 >> 0x3f & (undefined1  [32])0x1) ==
                                      (undefined1  [32])0x0) &&
                                     (auVar1 >> 0x47 & (undefined1  [32])0x1) ==
                                     (undefined1  [32])0x0) &&
                                    (auVar1 >> 0x4f & (undefined1  [32])0x1) ==
                                    (undefined1  [32])0x0) &&
                                   (auVar1 >> 0x57 & (undefined1  [32])0x1) == (undefined1  [32])0x0
                                   ) && (auVar1 >> 0x5f & (undefined1  [32])0x1) ==
                                        (undefined1  [32])0x0) &&
                                 (auVar1 >> 0x67 & (undefined1  [32])0x1) == (undefined1  [32])0x0)
                                && (auVar1 >> 0x6f & (undefined1  [32])0x1) == (undefined1  [32])0x0
                                ) && (auVar1 >> 0x77 & (undefined1  [32])0x1) ==
                                     (undefined1  [32])0x0) && SUB321(auVar1 >> 0x7f,0) == '\0') &&
                             (auVar1 >> 0x87 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                            (auVar1 >> 0x8f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                           (auVar1 >> 0x97 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                          (auVar1 >> 0x9f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                         (auVar1 >> 0xa7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                        (auVar1 >> 0xaf & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                       (auVar1 >> 0xb7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                      SUB321(auVar1 >> 0xbf,0) == '\0') &&
                     (auVar1 >> 199 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                    (auVar1 >> 0xcf & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                   (auVar1 >> 0xd7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                  (auVar1 >> 0xdf & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                 (auVar1 >> 0xe7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                (auVar1 >> 0xef & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
               (auVar1 >> 0xf7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
               -1 < auVar1[0x1f]))) {
          pauVar4 = pauVar4 + 1;
        }
        for (; (pauVar4 != (undefined1 (*) [32])(param_1 + param_2) && (*(short *)*pauVar4 != 0));
            pauVar4 = (undefined1 (*) [32])(*pauVar4 + 2)) {
        }
        uVar3 = (int)pauVar4 - (int)param_1 >> 1;
      }
      return uVar3;
    }
    psVar2 = param_1;
    if (param_1 != param_1 + param_2) {
      do {
        if (*psVar2 == 0) break;
        psVar2 = psVar2 + 1;
      } while (psVar2 != param_1 + param_2);
    }
    return (int)psVar2 - (int)param_1 >> 1;
  }
  if (DAT_005e5288 < 1) {
    psVar2 = param_1;
    if (param_1 != param_1 + param_2) {
      do {
        if (*psVar2 == 0) break;
        psVar2 = psVar2 + 1;
      } while (psVar2 != param_1 + param_2);
    }
    return (int)psVar2 - (int)param_1 >> 1;
  }
  if (((uint)param_1 & 1) == 0) {
    uVar5 = (-(uint)(((uint)param_1 & 0xf) != 0) & 0x10 - ((uint)param_1 & 0xf)) >> 1;
    if (param_2 < uVar5) {
      uVar5 = param_2;
    }
    psVar2 = param_1;
    if (param_1 != param_1 + uVar5) {
      do {
        if (*psVar2 == 0) break;
        psVar2 = psVar2 + 1;
      } while (psVar2 != param_1 + uVar5);
    }
    uVar3 = (int)psVar2 - (int)param_1 >> 1;
    if (uVar3 != uVar5) {
      return uVar3;
    }
    for (psVar2 = param_1 + uVar3;
        psVar2 != param_1 + ((param_2 - uVar5) - (param_2 - uVar5 & 0xf)) + uVar3;
        psVar2 = psVar2 + 8) {
      auVar6._0_2_ = -(ushort)(*psVar2 == 0);
      auVar6._2_2_ = -(ushort)(psVar2[1] == 0);
      auVar6._4_2_ = -(ushort)(psVar2[2] == 0);
      auVar6._6_2_ = -(ushort)(psVar2[3] == 0);
      auVar6._8_2_ = -(ushort)(psVar2[4] == 0);
      auVar6._10_2_ = -(ushort)(psVar2[5] == 0);
      auVar6._12_2_ = -(ushort)(psVar2[6] == 0);
      auVar6._14_2_ = -(ushort)(psVar2[7] == 0);
      if ((((((((((((((((auVar6 >> 7 & (undefined1  [16])0x1) != (undefined1  [16])0x0 ||
                       (auVar6 >> 0xf & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                      (auVar6 >> 0x17 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                     (auVar6 >> 0x1f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                    (auVar6 >> 0x27 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                   (auVar6 >> 0x2f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                  (auVar6 >> 0x37 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                 (auVar6 >> 0x3f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                (auVar6 >> 0x47 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
               (auVar6 >> 0x4f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
              (auVar6 >> 0x57 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
             (auVar6 >> 0x5f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
            (auVar6 >> 0x67 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
           (auVar6 >> 0x6f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
          (auVar6._14_2_ >> 7 & 1) != 0) || (auVar6._14_2_ & 0x8000) != 0) break;
    }
    for (; (psVar2 != param_1 + param_2 && (*psVar2 != 0)); psVar2 = psVar2 + 1) {
    }
  }
  else {
    psVar2 = param_1;
    if (param_1 != param_1 + param_2) {
      while (*psVar2 != 0) {
        psVar2 = psVar2 + 1;
        if (psVar2 == param_1 + param_2) {
          return (int)psVar2 - (int)param_1 >> 1;
        }
      }
    }
  }
  return (int)psVar2 - (int)param_1 >> 1;
}

