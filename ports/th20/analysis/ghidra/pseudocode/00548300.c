/* Entry: 0x00548300; symbol: FUN_00548300; body bytes: 290 */

uint __cdecl FUN_00548300(undefined1 (*param_1) [32])

{
  undefined1 auVar1 [32];
  undefined1 (*pauVar2) [32];
  uint uVar3;
  uint uVar4;
  undefined1 auVar5 [16];
  
  if (DAT_005e5288 < 5) {
    pauVar2 = param_1;
    if (DAT_005e5288 < 1) {
      for (; *(short *)*pauVar2 != 0; pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2)) {
      }
      return (int)pauVar2 - (int)param_1 >> 1;
    }
    if (((uint)param_1 & 1) == 0) {
      uVar4 = (-(uint)(((uint)param_1 & 0xf) != 0) & 0x10 - ((uint)param_1 & 0xf)) >> 1;
      if (param_1 != (undefined1 (*) [32])(*param_1 + uVar4 * 2)) {
        do {
          if (*(short *)*pauVar2 == 0) break;
          pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2);
        } while (pauVar2 != (undefined1 (*) [32])(*param_1 + uVar4 * 2));
      }
      uVar3 = (int)pauVar2 - (int)param_1 >> 1;
      if (uVar3 != uVar4) {
        return uVar3;
      }
      pauVar2 = (undefined1 (*) [32])(*param_1 + uVar3 * 2);
      while( true ) {
        auVar5._0_2_ = -(ushort)(*(short *)*pauVar2 == 0);
        auVar5._2_2_ = -(ushort)(*(short *)(*pauVar2 + 2) == 0);
        auVar5._4_2_ = -(ushort)(*(short *)(*pauVar2 + 4) == 0);
        auVar5._6_2_ = -(ushort)(*(short *)(*pauVar2 + 6) == 0);
        auVar5._8_2_ = -(ushort)(*(short *)(*pauVar2 + 8) == 0);
        auVar5._10_2_ = -(ushort)(*(short *)(*pauVar2 + 10) == 0);
        auVar5._12_2_ = -(ushort)(*(short *)(*pauVar2 + 0xc) == 0);
        auVar5._14_2_ = -(ushort)(*(short *)(*pauVar2 + 0xe) == 0);
        if ((((((((((((((((auVar5 >> 7 & (undefined1  [16])0x1) != (undefined1  [16])0x0 ||
                         (auVar5 >> 0xf & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                        (auVar5 >> 0x17 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                       (auVar5 >> 0x1f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                      (auVar5 >> 0x27 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                     (auVar5 >> 0x2f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                    (auVar5 >> 0x37 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                   (auVar5 >> 0x3f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                  (auVar5 >> 0x47 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                 (auVar5 >> 0x4f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
                (auVar5 >> 0x57 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
               (auVar5 >> 0x5f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
              (auVar5 >> 0x67 & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
             (auVar5 >> 0x6f & (undefined1  [16])0x1) != (undefined1  [16])0x0) ||
            (auVar5._14_2_ >> 7 & 1) != 0) || (auVar5._14_2_ & 0x8000) != 0) break;
        pauVar2 = (undefined1 (*) [32])(*pauVar2 + 0x10);
      }
      for (; *(short *)*pauVar2 != 0; pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2)) {
      }
    }
    else {
      for (; *(short *)*pauVar2 != 0; pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2)) {
      }
    }
    return (int)pauVar2 - (int)param_1 >> 1;
  }
  pauVar2 = param_1;
  if (((uint)param_1 & 1) == 0) {
    uVar4 = (-(uint)(((uint)param_1 & 0x1f) != 0) & 0x20 - ((uint)param_1 & 0x1f)) >> 1;
    if (param_1 != (undefined1 (*) [32])(*param_1 + uVar4 * 2)) {
      do {
        if (*(short *)*pauVar2 == 0) break;
        pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2);
      } while (pauVar2 != (undefined1 (*) [32])(*param_1 + uVar4 * 2));
    }
    uVar3 = (int)pauVar2 - (int)param_1 >> 1;
    if (uVar3 != uVar4) {
      return uVar3;
    }
    pauVar2 = (undefined1 (*) [32])(*param_1 + uVar3 * 2);
    while (auVar1 = vpcmpeqw_avx2(SUB6432(ZEXT1664((undefined1  [16])0x0),0),*pauVar2),
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
                                   (auVar1 >> 0x2f & (undefined1  [32])0x1) == (undefined1  [32])0x0
                                   ) && (auVar1 >> 0x37 & (undefined1  [32])0x1) ==
                                        (undefined1  [32])0x0) &&
                                 (auVar1 >> 0x3f & (undefined1  [32])0x1) == (undefined1  [32])0x0)
                                && (auVar1 >> 0x47 & (undefined1  [32])0x1) == (undefined1  [32])0x0
                                ) && (auVar1 >> 0x4f & (undefined1  [32])0x1) ==
                                     (undefined1  [32])0x0) &&
                              (auVar1 >> 0x57 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                             (auVar1 >> 0x5f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                            (auVar1 >> 0x67 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                           (auVar1 >> 0x6f & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                          (auVar1 >> 0x77 & (undefined1  [32])0x1) == (undefined1  [32])0x0) &&
                         SUB321(auVar1 >> 0x7f,0) == '\0') &&
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
          (auVar1 >> 0xf7 & (undefined1  [32])0x1) == (undefined1  [32])0x0) && -1 < auVar1[0x1f]) {
      pauVar2 = pauVar2 + 1;
    }
    for (; *(short *)*pauVar2 != 0; pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2)) {
    }
  }
  else {
    for (; *(short *)*pauVar2 != 0; pauVar2 = (undefined1 (*) [32])(*pauVar2 + 2)) {
    }
  }
  return (int)pauVar2 - (int)param_1 >> 1;
}

