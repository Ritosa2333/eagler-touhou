/* Entry: 0x005448c0; symbol: FUN_005448c0; body bytes: 311 */

undefined1 (*) [16] __cdecl FUN_005448c0(undefined1 (*param_1) [16],byte param_2)

{
  byte bVar1;
  undefined1 *puVar2;
  uint uVar3;
  undefined1 (*pauVar4) [16];
  uint uVar5;
  int iVar6;
  undefined1 (*pauVar7) [16];
  char *pcVar8;
  byte *pbVar9;
  undefined1 auVar11 [16];
  undefined1 auVar12 [16];
  undefined1 auVar13 [16];
  undefined1 auVar14 [16];
  byte *pbVar10;
  
  if (DAT_005e5288 != 0) {
    if (DAT_005e5288 < 2) {
      auVar14 = pshuflw(ZEXT216(CONCAT11(param_2,param_2)),ZEXT216(CONCAT11(param_2,param_2)),0);
      uVar3 = -1 << (sbyte)((uint)param_1 & 0xf);
      pcVar8 = (char *)((int)param_1 - ((uint)param_1 & 0xf));
      pauVar7 = (undefined1 (*) [16])0x0;
      while( true ) {
        auVar13[0] = -(*pcVar8 == '\0');
        auVar13[1] = -(pcVar8[1] == '\0');
        auVar13[2] = -(pcVar8[2] == '\0');
        auVar13[3] = -(pcVar8[3] == '\0');
        auVar13[4] = -(pcVar8[4] == '\0');
        auVar13[5] = -(pcVar8[5] == '\0');
        auVar13[6] = -(pcVar8[6] == '\0');
        auVar13[7] = -(pcVar8[7] == '\0');
        auVar13[8] = -(pcVar8[8] == '\0');
        auVar13[9] = -(pcVar8[9] == '\0');
        auVar13[10] = -(pcVar8[10] == '\0');
        auVar13[0xb] = -(pcVar8[0xb] == '\0');
        auVar13[0xc] = -(pcVar8[0xc] == '\0');
        auVar13[0xd] = -(pcVar8[0xd] == '\0');
        auVar13[0xe] = -(pcVar8[0xe] == '\0');
        auVar13[0xf] = -(pcVar8[0xf] == '\0');
        auVar12[0] = -(*pcVar8 == auVar14[0]);
        auVar12[1] = -(pcVar8[1] == auVar14[1]);
        auVar12[2] = -(pcVar8[2] == auVar14[2]);
        auVar12[3] = -(pcVar8[3] == auVar14[3]);
        auVar12[4] = -(pcVar8[4] == auVar14[4]);
        auVar12[5] = -(pcVar8[5] == auVar14[5]);
        auVar12[6] = -(pcVar8[6] == auVar14[6]);
        auVar12[7] = -(pcVar8[7] == auVar14[7]);
        auVar12[8] = -(pcVar8[8] == auVar14[0]);
        auVar12[9] = -(pcVar8[9] == auVar14[1]);
        auVar12[10] = -(pcVar8[10] == auVar14[2]);
        auVar12[0xb] = -(pcVar8[0xb] == auVar14[3]);
        auVar12[0xc] = -(pcVar8[0xc] == auVar14[4]);
        auVar12[0xd] = -(pcVar8[0xd] == auVar14[5]);
        auVar12[0xe] = -(pcVar8[0xe] == auVar14[6]);
        auVar12[0xf] = -(pcVar8[0xf] == auVar14[7]);
        uVar5 = (ushort)((ushort)(SUB161(auVar13 >> 7,0) & 1) |
                         (ushort)(SUB161(auVar13 >> 0xf,0) & 1) << 1 |
                         (ushort)(SUB161(auVar13 >> 0x17,0) & 1) << 2 |
                         (ushort)(SUB161(auVar13 >> 0x1f,0) & 1) << 3 |
                         (ushort)(SUB161(auVar13 >> 0x27,0) & 1) << 4 |
                         (ushort)(SUB161(auVar13 >> 0x2f,0) & 1) << 5 |
                         (ushort)(SUB161(auVar13 >> 0x37,0) & 1) << 6 |
                         (ushort)(SUB161(auVar13 >> 0x3f,0) & 1) << 7 |
                         (ushort)(SUB161(auVar13 >> 0x47,0) & 1) << 8 |
                         (ushort)(SUB161(auVar13 >> 0x4f,0) & 1) << 9 |
                         (ushort)(SUB161(auVar13 >> 0x57,0) & 1) << 10 |
                         (ushort)(SUB161(auVar13 >> 0x5f,0) & 1) << 0xb |
                         (ushort)(SUB161(auVar13 >> 0x67,0) & 1) << 0xc |
                         (ushort)(SUB161(auVar13 >> 0x6f,0) & 1) << 0xd |
                         (ushort)(SUB161(auVar13 >> 0x77,0) & 1) << 0xe |
                        (ushort)(auVar13[0xf] >> 7) << 0xf) & uVar3;
        if (uVar5 != 0) break;
        uVar3 = (ushort)((ushort)(SUB161(auVar12 >> 7,0) & 1) |
                         (ushort)(SUB161(auVar12 >> 0xf,0) & 1) << 1 |
                         (ushort)(SUB161(auVar12 >> 0x17,0) & 1) << 2 |
                         (ushort)(SUB161(auVar12 >> 0x1f,0) & 1) << 3 |
                         (ushort)(SUB161(auVar12 >> 0x27,0) & 1) << 4 |
                         (ushort)(SUB161(auVar12 >> 0x2f,0) & 1) << 5 |
                         (ushort)(SUB161(auVar12 >> 0x37,0) & 1) << 6 |
                         (ushort)(SUB161(auVar12 >> 0x3f,0) & 1) << 7 |
                         (ushort)(SUB161(auVar12 >> 0x47,0) & 1) << 8 |
                         (ushort)(SUB161(auVar12 >> 0x4f,0) & 1) << 9 |
                         (ushort)(SUB161(auVar12 >> 0x57,0) & 1) << 10 |
                         (ushort)(SUB161(auVar12 >> 0x5f,0) & 1) << 0xb |
                         (ushort)(SUB161(auVar12 >> 0x67,0) & 1) << 0xc |
                         (ushort)(SUB161(auVar12 >> 0x6f,0) & 1) << 0xd |
                         (ushort)(SUB161(auVar12 >> 0x77,0) & 1) << 0xe |
                        (ushort)(auVar12[0xf] >> 7) << 0xf) & uVar3;
        iVar6 = 0x1f;
        if (uVar3 != 0) {
          for (; uVar3 >> iVar6 == 0; iVar6 = iVar6 + -1) {
          }
        }
        if (uVar3 != 0) {
          pauVar7 = (undefined1 (*) [16])(pcVar8 + iVar6);
        }
        uVar3 = 0xffffffff;
        pcVar8 = pcVar8 + 0x10;
      }
      uVar3 = (uVar5 * 2 & uVar5 * -2) - 1 &
              (ushort)((ushort)(SUB161(auVar12 >> 7,0) & 1) |
                       (ushort)(SUB161(auVar12 >> 0xf,0) & 1) << 1 |
                       (ushort)(SUB161(auVar12 >> 0x17,0) & 1) << 2 |
                       (ushort)(SUB161(auVar12 >> 0x1f,0) & 1) << 3 |
                       (ushort)(SUB161(auVar12 >> 0x27,0) & 1) << 4 |
                       (ushort)(SUB161(auVar12 >> 0x2f,0) & 1) << 5 |
                       (ushort)(SUB161(auVar12 >> 0x37,0) & 1) << 6 |
                       (ushort)(SUB161(auVar12 >> 0x3f,0) & 1) << 7 |
                       (ushort)(SUB161(auVar12 >> 0x47,0) & 1) << 8 |
                       (ushort)(SUB161(auVar12 >> 0x4f,0) & 1) << 9 |
                       (ushort)(SUB161(auVar12 >> 0x57,0) & 1) << 10 |
                       (ushort)(SUB161(auVar12 >> 0x5f,0) & 1) << 0xb |
                       (ushort)(SUB161(auVar12 >> 0x67,0) & 1) << 0xc |
                       (ushort)(SUB161(auVar12 >> 0x6f,0) & 1) << 0xd |
                       (ushort)(SUB161(auVar12 >> 0x77,0) & 1) << 0xe |
                      (ushort)(auVar12[0xf] >> 7) << 0xf) & uVar3;
      iVar6 = 0x1f;
      if (uVar3 != 0) {
        for (; uVar3 >> iVar6 == 0; iVar6 = iVar6 + -1) {
        }
      }
      pauVar4 = (undefined1 (*) [16])(pcVar8 + iVar6);
      if (uVar3 == 0) {
        pauVar4 = pauVar7;
      }
      return pauVar4;
    }
    uVar3 = (uint)param_2;
    if (uVar3 == 0) {
      pcVar8 = (char *)((uint)param_1 & 0xfffffff0);
      auVar14[0] = -(*pcVar8 == '\0');
      auVar14[1] = -(pcVar8[1] == '\0');
      auVar14[2] = -(pcVar8[2] == '\0');
      auVar14[3] = -(pcVar8[3] == '\0');
      auVar14[4] = -(pcVar8[4] == '\0');
      auVar14[5] = -(pcVar8[5] == '\0');
      auVar14[6] = -(pcVar8[6] == '\0');
      auVar14[7] = -(pcVar8[7] == '\0');
      auVar14[8] = -(pcVar8[8] == '\0');
      auVar14[9] = -(pcVar8[9] == '\0');
      auVar14[10] = -(pcVar8[10] == '\0');
      auVar14[0xb] = -(pcVar8[0xb] == '\0');
      auVar14[0xc] = -(pcVar8[0xc] == '\0');
      auVar14[0xd] = -(pcVar8[0xd] == '\0');
      auVar14[0xe] = -(pcVar8[0xe] == '\0');
      auVar14[0xf] = -(pcVar8[0xf] == '\0');
      uVar3 = (uint)(ushort)((ushort)(SUB161(auVar14 >> 7,0) & 1) |
                             (ushort)(SUB161(auVar14 >> 0xf,0) & 1) << 1 |
                             (ushort)(SUB161(auVar14 >> 0x17,0) & 1) << 2 |
                             (ushort)(SUB161(auVar14 >> 0x1f,0) & 1) << 3 |
                             (ushort)(SUB161(auVar14 >> 0x27,0) & 1) << 4 |
                             (ushort)(SUB161(auVar14 >> 0x2f,0) & 1) << 5 |
                             (ushort)(SUB161(auVar14 >> 0x37,0) & 1) << 6 |
                             (ushort)(SUB161(auVar14 >> 0x3f,0) & 1) << 7 |
                             (ushort)(SUB161(auVar14 >> 0x47,0) & 1) << 8 |
                             (ushort)(SUB161(auVar14 >> 0x4f,0) & 1) << 9 |
                             (ushort)(SUB161(auVar14 >> 0x57,0) & 1) << 10 |
                             (ushort)(SUB161(auVar14 >> 0x5f,0) & 1) << 0xb |
                             (ushort)(SUB161(auVar14 >> 0x67,0) & 1) << 0xc |
                             (ushort)(SUB161(auVar14 >> 0x6f,0) & 1) << 0xd |
                             (ushort)(SUB161(auVar14 >> 0x77,0) & 1) << 0xe |
                            (ushort)(auVar14[0xf] >> 7) << 0xf) & -1 << ((byte)param_1 & 0xf);
      while (uVar3 == 0) {
        auVar11[0] = -(pcVar8[0x10] == '\0');
        auVar11[1] = -(pcVar8[0x11] == '\0');
        auVar11[2] = -(pcVar8[0x12] == '\0');
        auVar11[3] = -(pcVar8[0x13] == '\0');
        auVar11[4] = -(pcVar8[0x14] == '\0');
        auVar11[5] = -(pcVar8[0x15] == '\0');
        auVar11[6] = -(pcVar8[0x16] == '\0');
        auVar11[7] = -(pcVar8[0x17] == '\0');
        auVar11[8] = -(pcVar8[0x18] == '\0');
        auVar11[9] = -(pcVar8[0x19] == '\0');
        auVar11[10] = -(pcVar8[0x1a] == '\0');
        auVar11[0xb] = -(pcVar8[0x1b] == '\0');
        auVar11[0xc] = -(pcVar8[0x1c] == '\0');
        auVar11[0xd] = -(pcVar8[0x1d] == '\0');
        auVar11[0xe] = -(pcVar8[0x1e] == '\0');
        auVar11[0xf] = -(pcVar8[0x1f] == '\0');
        pcVar8 = pcVar8 + 0x10;
        uVar3 = (uint)(ushort)((ushort)(SUB161(auVar11 >> 7,0) & 1) |
                               (ushort)(SUB161(auVar11 >> 0xf,0) & 1) << 1 |
                               (ushort)(SUB161(auVar11 >> 0x17,0) & 1) << 2 |
                               (ushort)(SUB161(auVar11 >> 0x1f,0) & 1) << 3 |
                               (ushort)(SUB161(auVar11 >> 0x27,0) & 1) << 4 |
                               (ushort)(SUB161(auVar11 >> 0x2f,0) & 1) << 5 |
                               (ushort)(SUB161(auVar11 >> 0x37,0) & 1) << 6 |
                               (ushort)(SUB161(auVar11 >> 0x3f,0) & 1) << 7 |
                               (ushort)(SUB161(auVar11 >> 0x47,0) & 1) << 8 |
                               (ushort)(SUB161(auVar11 >> 0x4f,0) & 1) << 9 |
                               (ushort)(SUB161(auVar11 >> 0x57,0) & 1) << 10 |
                               (ushort)(SUB161(auVar11 >> 0x5f,0) & 1) << 0xb |
                               (ushort)(SUB161(auVar11 >> 0x67,0) & 1) << 0xc |
                               (ushort)(SUB161(auVar11 >> 0x6f,0) & 1) << 0xd |
                               (ushort)(SUB161(auVar11 >> 0x77,0) & 1) << 0xe |
                              (ushort)(auVar11[0xf] >> 7) << 0xf);
      }
      iVar6 = 0;
      if (uVar3 != 0) {
        for (; (uVar3 >> iVar6 & 1) == 0; iVar6 = iVar6 + 1) {
        }
      }
      pauVar7 = (undefined1 (*) [16])(pcVar8 + iVar6);
    }
    else {
      pauVar7 = (undefined1 (*) [16])0x0;
      uVar5 = (uint)param_1 & 0xf;
      while (uVar5 != 0) {
        if ((byte)(*param_1)[0] == uVar3) {
          pauVar7 = param_1;
        }
        if ((byte)(*param_1)[0] == 0) {
          return pauVar7;
        }
        param_1 = (undefined1 (*) [16])(*param_1 + 1);
        uVar5 = (uint)param_1 & 0xf;
      }
      do {
        pauVar4 = param_1 + 1;
        iVar6 = pcmpistri(ZEXT416(uVar3),*param_1,0x40);
        if ((undefined1 (*) [16])0xffffffef < param_1) {
          pauVar7 = (undefined1 (*) [16])(pauVar4[-1] + iVar6);
        }
        param_1 = pauVar4;
      } while (pauVar4 != (undefined1 (*) [16])0x0);
    }
    return pauVar7;
  }
  iVar6 = -1;
  do {
    pauVar7 = param_1;
    if (iVar6 == 0) break;
    iVar6 = iVar6 + -1;
    pauVar7 = (undefined1 (*) [16])(*param_1 + 1);
    puVar2 = *param_1;
    param_1 = pauVar7;
  } while (*puVar2 != '\0');
  iVar6 = -(iVar6 + 1);
  pbVar10 = pauVar7[-1] + 0xf;
  do {
    pbVar9 = pbVar10;
    if (iVar6 == 0) break;
    iVar6 = iVar6 + -1;
    pbVar9 = pbVar10 + -1;
    bVar1 = *pbVar10;
    pbVar10 = pbVar9;
  } while (param_2 != bVar1);
  pauVar7 = (undefined1 (*) [16])(pbVar9 + 1);
  if ((*pauVar7)[0] != param_2) {
    pauVar7 = (undefined1 (*) [16])0x0;
  }
  return pauVar7;
}

