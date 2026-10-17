/* Entry: 0x005481a0; symbol: FUN_005481a0; body bytes: 352 */

uint __cdecl FUN_005481a0(undefined1 (*param_1) [32],uint param_2)

{
  bool bVar1;
  undefined1 auVar2 [32];
  undefined1 (*pauVar3) [32];
  uint uVar4;
  undefined1 (*pauVar5) [32];
  undefined1 auVar6 [16];
  
  pauVar3 = param_1;
  if (DAT_005e5288 < 5) {
    if (DAT_005e5288 < 1) {
      if (param_1 != (undefined1 (*) [32])(*param_1 + param_2)) {
        do {
          if ((*pauVar3)[0] == '\0') break;
          pauVar3 = (undefined1 (*) [32])(*pauVar3 + 1);
        } while (pauVar3 != (undefined1 (*) [32])(*param_1 + param_2));
      }
      return (int)pauVar3 - (int)param_1;
    }
    uVar4 = -(uint)(((uint)param_1 & 0xf) != 0) & 0x10 - ((uint)param_1 & 0xf);
    if (param_2 < uVar4) {
      uVar4 = param_2;
    }
    if (param_1 != (undefined1 (*) [32])(*param_1 + uVar4)) {
      do {
        if ((*pauVar3)[0] == '\0') break;
        pauVar3 = (undefined1 (*) [32])(*pauVar3 + 1);
      } while (pauVar3 != (undefined1 (*) [32])(*param_1 + uVar4));
    }
    if ((int)pauVar3 - (int)param_1 != uVar4) {
      return (int)pauVar3 - (int)param_1;
    }
    pauVar5 = (undefined1 (*) [32])((int)pauVar3 + ((param_2 - uVar4) - (param_2 - uVar4 & 0xf)));
    while ((pauVar3 != pauVar5 &&
           (auVar6[0] = -((*pauVar3)[0] == '\0'), auVar6[1] = -((*pauVar3)[1] == '\0'),
           auVar6[2] = -((*pauVar3)[2] == '\0'), auVar6[3] = -((*pauVar3)[3] == '\0'),
           auVar6[4] = -((*pauVar3)[4] == '\0'), auVar6[5] = -((*pauVar3)[5] == '\0'),
           auVar6[6] = -((*pauVar3)[6] == '\0'), auVar6[7] = -((*pauVar3)[7] == '\0'),
           auVar6[8] = -((*pauVar3)[8] == '\0'), auVar6[9] = -((*pauVar3)[9] == '\0'),
           auVar6[10] = -((*pauVar3)[10] == '\0'), auVar6[0xb] = -((*pauVar3)[0xb] == '\0'),
           auVar6[0xc] = -((*pauVar3)[0xc] == '\0'), auVar6[0xd] = -((*pauVar3)[0xd] == '\0'),
           auVar6[0xe] = -((*pauVar3)[0xe] == '\0'), bVar1 = (*pauVar3)[0xf] == '\0',
           auVar6[0xf] = -bVar1,
           (((((((((((((((auVar6 >> 7 & (undefined1  [16])0x1) == (undefined1  [16])0x0 &&
                        (auVar6 >> 0xf & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                       (auVar6 >> 0x17 & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                      (auVar6 >> 0x1f & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                     (auVar6 >> 0x27 & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                    (auVar6 >> 0x2f & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                   (auVar6 >> 0x37 & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                  (auVar6 >> 0x3f & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                 (auVar6 >> 0x47 & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
                (auVar6 >> 0x4f & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
               (auVar6 >> 0x57 & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
              (auVar6 >> 0x5f & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
             (auVar6 >> 0x67 & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
            (auVar6 >> 0x6f & (undefined1  [16])0x1) == (undefined1  [16])0x0) &&
           (auVar6 >> 0x77 & (undefined1  [16])0x1) == (undefined1  [16])0x0) && !bVar1))) {
      pauVar3 = (undefined1 (*) [32])(*pauVar3 + 0x10);
    }
    if (pauVar3 != (undefined1 (*) [32])(*param_1 + param_2)) {
      while ((*pauVar3)[0] != '\0') {
        pauVar3 = (undefined1 (*) [32])(*pauVar3 + 1);
        if (pauVar3 == (undefined1 (*) [32])(*param_1 + param_2)) {
          return (int)pauVar3 - (int)param_1;
        }
      }
    }
  }
  else {
    uVar4 = -(uint)(((uint)param_1 & 0x1f) != 0) & 0x20 - ((uint)param_1 & 0x1f);
    if (param_2 < uVar4) {
      uVar4 = param_2;
    }
    if (param_1 != (undefined1 (*) [32])(*param_1 + uVar4)) {
      do {
        if ((*pauVar3)[0] == '\0') break;
        pauVar3 = (undefined1 (*) [32])(*pauVar3 + 1);
      } while (pauVar3 != (undefined1 (*) [32])(*param_1 + uVar4));
    }
    if ((int)pauVar3 - (int)param_1 != uVar4) {
      return (int)pauVar3 - (int)param_1;
    }
    pauVar5 = (undefined1 (*) [32])((int)pauVar3 + ((param_2 - uVar4) - (param_2 - uVar4 & 0x1f)));
    while ((pauVar3 != pauVar5 &&
           (auVar2 = vpcmpeqb_avx2(SUB6432(ZEXT1664((undefined1  [16])0x0),0),*pauVar3),
           (((((((((((((((((((((((((((((((auVar2 >> 7 & (undefined1  [32])0x1) ==
                                         (undefined1  [32])0x0 &&
                                        (auVar2 >> 0xf & (undefined1  [32])0x1) ==
                                        (undefined1  [32])0x0) &&
                                       (auVar2 >> 0x17 & (undefined1  [32])0x1) ==
                                       (undefined1  [32])0x0) &&
                                      (auVar2 >> 0x1f & (undefined1  [32])0x1) ==
                                      (undefined1  [32])0x0) &&
                                     (auVar2 >> 0x27 & (undefined1  [32])0x1) ==
                                     (undefined1  [32])0x0) &&
                                    (auVar2 >> 0x2f & (undefined1  [32])0x1) ==
                                    (undefined1  [32])0x0) &&
                                   (auVar2 >> 0x37 & (undefined1  [32])0x1) == (undefined1  [32])0x0
                                   ) && (auVar2 >> 0x3f & (undefined1  [32])0x1) ==
                                        (undefined1  [32])0x0) &&
                                 (auVar2 >> 0x47 & (undefined1  [32])0x1) == (undefined1  [32])0x0)
                                && (auVar2 >> 0x4f & (undefined1  [32])0x1) == (undefined1  [32])0x0
                                ) && (auVar2 >> 0x57 & (undefined1  [32])0x1) ==
                                     (undefined1  [32])0x0) &&
                              (auVar2 >> 0x5f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                             (auVar2 >> 0x67 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                            (auVar2 >> 0x6f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                           (auVar2 >> 0x77 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                          SUB321(auVar2 >> 0x7f,0) == '\0') &&
                         (auVar2 >> 0x87 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                        (auVar2 >> 0x8f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                       (auVar2 >> 0x97 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                      (auVar2 >> 0x9f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                     (auVar2 >> 0xa7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                    (auVar2 >> 0xaf & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                   (auVar2 >> 0xb7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                  SUB321(auVar2 >> 0xbf,0) == '\0') &&
                 (auVar2 >> 199 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                (auVar2 >> 0xcf & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
               (auVar2 >> 0xd7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
              (auVar2 >> 0xdf & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
             (auVar2 >> 0xe7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
            (auVar2 >> 0xef & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
           (auVar2 >> 0xf7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) && -1 < auVar2[0x1f]))
          ) {
      pauVar3 = pauVar3 + 1;
    }
    for (; (pauVar3 != (undefined1 (*) [32])(*param_1 + param_2) && ((*pauVar3)[0] != '\0'));
        pauVar3 = (undefined1 (*) [32])(*pauVar3 + 1)) {
    }
  }
  return (int)pauVar3 - (int)param_1;
}

