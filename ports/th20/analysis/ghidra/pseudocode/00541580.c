/* Entry: 0x00541580; symbol: FUN_00541580; body bytes: 264 */

undefined1 (*) [32] FUN_00541580(undefined1 (*param_1) [32],undefined1 (*param_2) [32],byte param_3)

{
  ushort uVar1;
  undefined1 auVar2 [32];
  int iVar3;
  undefined1 *puVar4;
  uint uVar5;
  uint uVar6;
  undefined1 auVar7 [16];
  undefined1 auVar8 [16];
  undefined1 auVar9 [32];
  
  uVar6 = (int)param_2 - (int)param_1;
  if (((uVar6 & 0xffffffe0) == 0) || ((DAT_005b2618 >> 5 & 1) == 0)) {
    if (((uVar6 & 0xfffffff0) != 0) && ((DAT_005b2618 >> 2 & 1) != 0)) {
      puVar4 = *param_1;
      auVar8 = pshufb(ZEXT116(param_3),ZEXT816(0));
      do {
        auVar7[0] = -((*param_1)[0] == auVar8[0]);
        auVar7[1] = -((*param_1)[1] == auVar8[1]);
        auVar7[2] = -((*param_1)[2] == auVar8[2]);
        auVar7[3] = -((*param_1)[3] == auVar8[3]);
        auVar7[4] = -((*param_1)[4] == auVar8[4]);
        auVar7[5] = -((*param_1)[5] == auVar8[5]);
        auVar7[6] = -((*param_1)[6] == auVar8[6]);
        auVar7[7] = -((*param_1)[7] == auVar8[7]);
        auVar7[8] = -((*param_1)[8] == auVar8[8]);
        auVar7[9] = -((*param_1)[9] == auVar8[9]);
        auVar7[10] = -((*param_1)[10] == auVar8[10]);
        auVar7[0xb] = -((*param_1)[0xb] == auVar8[0xb]);
        auVar7[0xc] = -((*param_1)[0xc] == auVar8[0xc]);
        auVar7[0xd] = -((*param_1)[0xd] == auVar8[0xd]);
        auVar7[0xe] = -((*param_1)[0xe] == auVar8[0xe]);
        auVar7[0xf] = -((*param_1)[0xf] == auVar8[0xf]);
        uVar1 = (ushort)(SUB161(auVar7 >> 7,0) & 1) | (ushort)(SUB161(auVar7 >> 0xf,0) & 1) << 1 |
                (ushort)(SUB161(auVar7 >> 0x17,0) & 1) << 2 |
                (ushort)(SUB161(auVar7 >> 0x1f,0) & 1) << 3 |
                (ushort)(SUB161(auVar7 >> 0x27,0) & 1) << 4 |
                (ushort)(SUB161(auVar7 >> 0x2f,0) & 1) << 5 |
                (ushort)(SUB161(auVar7 >> 0x37,0) & 1) << 6 |
                (ushort)(SUB161(auVar7 >> 0x3f,0) & 1) << 7 |
                (ushort)(SUB161(auVar7 >> 0x47,0) & 1) << 8 |
                (ushort)(SUB161(auVar7 >> 0x4f,0) & 1) << 9 |
                (ushort)(SUB161(auVar7 >> 0x57,0) & 1) << 10 |
                (ushort)(SUB161(auVar7 >> 0x5f,0) & 1) << 0xb |
                (ushort)(SUB161(auVar7 >> 0x67,0) & 1) << 0xc |
                (ushort)(SUB161(auVar7 >> 0x6f,0) & 1) << 0xd |
                (ushort)(SUB161(auVar7 >> 0x77,0) & 1) << 0xe | (ushort)(auVar7[0xf] >> 7) << 0xf;
        if (uVar1 != 0) {
          iVar3 = 0;
          if (uVar1 != 0) {
            for (; (uVar1 >> iVar3 & 1) == 0; iVar3 = iVar3 + 1) {
            }
          }
          return (undefined1 (*) [32])(*param_1 + iVar3);
        }
        param_1 = (undefined1 (*) [32])(*param_1 + 0x10);
      } while (param_1 != (undefined1 (*) [32])(puVar4 + (uVar6 & 0xfffffff0)));
    }
  }
  else {
    auVar8 = vpshufb_avx(ZEXT416((uint)(int)(char)param_3),(undefined1  [16])0x0);
    auVar9._16_16_ = auVar8;
    auVar9._0_16_ = auVar8;
    puVar4 = *param_1;
    do {
      auVar2 = vpcmpeqb_avx2(auVar9,*param_1);
      uVar5 = (uint)(SUB321(auVar2 >> 7,0) & 1) | (uint)(SUB321(auVar2 >> 0xf,0) & 1) << 1 |
              (uint)(SUB321(auVar2 >> 0x17,0) & 1) << 2 | (uint)(SUB321(auVar2 >> 0x1f,0) & 1) << 3
              | (uint)(SUB321(auVar2 >> 0x27,0) & 1) << 4 |
              (uint)(SUB321(auVar2 >> 0x2f,0) & 1) << 5 | (uint)(SUB321(auVar2 >> 0x37,0) & 1) << 6
              | (uint)(SUB321(auVar2 >> 0x3f,0) & 1) << 7 |
              (uint)(SUB321(auVar2 >> 0x47,0) & 1) << 8 | (uint)(SUB321(auVar2 >> 0x4f,0) & 1) << 9
              | (uint)(SUB321(auVar2 >> 0x57,0) & 1) << 10 |
              (uint)(SUB321(auVar2 >> 0x5f,0) & 1) << 0xb |
              (uint)(SUB321(auVar2 >> 0x67,0) & 1) << 0xc |
              (uint)(SUB321(auVar2 >> 0x6f,0) & 1) << 0xd |
              (uint)(SUB321(auVar2 >> 0x77,0) & 1) << 0xe | (uint)SUB321(auVar2 >> 0x7f,0) << 0xf |
              (uint)(SUB321(auVar2 >> 0x87,0) & 1) << 0x10 |
              (uint)(SUB321(auVar2 >> 0x8f,0) & 1) << 0x11 |
              (uint)(SUB321(auVar2 >> 0x97,0) & 1) << 0x12 |
              (uint)(SUB321(auVar2 >> 0x9f,0) & 1) << 0x13 |
              (uint)(SUB321(auVar2 >> 0xa7,0) & 1) << 0x14 |
              (uint)(SUB321(auVar2 >> 0xaf,0) & 1) << 0x15 |
              (uint)(SUB321(auVar2 >> 0xb7,0) & 1) << 0x16 | (uint)SUB321(auVar2 >> 0xbf,0) << 0x17
              | (uint)(SUB321(auVar2 >> 199,0) & 1) << 0x18 |
              (uint)(SUB321(auVar2 >> 0xcf,0) & 1) << 0x19 |
              (uint)(SUB321(auVar2 >> 0xd7,0) & 1) << 0x1a |
              (uint)(SUB321(auVar2 >> 0xdf,0) & 1) << 0x1b |
              (uint)(SUB321(auVar2 >> 0xe7,0) & 1) << 0x1c |
              (uint)(SUB321(auVar2 >> 0xef,0) & 1) << 0x1d |
              (uint)(SUB321(auVar2 >> 0xf7,0) & 1) << 0x1e | (uint)(byte)(auVar2[0x1f] >> 7) << 0x1f
      ;
      if (uVar5 != 0) goto LAB_00541631;
      param_1 = param_1 + 1;
    } while (param_1 != (undefined1 (*) [32])(puVar4 + (uVar6 & 0xffffffe0)));
    uVar6 = uVar6 & 0x1c;
    if (uVar6 != 0) {
      auVar2 = vpmaskmovd_avx2(*(undefined1 (*) [32])(&DAT_00579f20 + -uVar6),*param_1);
      auVar9 = vpcmpeqb_avx2(auVar2,auVar9);
      auVar9 = vpand_avx2(auVar9,*(undefined1 (*) [32])(&DAT_00579f20 + -uVar6));
      uVar5 = (uint)(SUB321(auVar9 >> 7,0) & 1) | (uint)(SUB321(auVar9 >> 0xf,0) & 1) << 1 |
              (uint)(SUB321(auVar9 >> 0x17,0) & 1) << 2 | (uint)(SUB321(auVar9 >> 0x1f,0) & 1) << 3
              | (uint)(SUB321(auVar9 >> 0x27,0) & 1) << 4 |
              (uint)(SUB321(auVar9 >> 0x2f,0) & 1) << 5 | (uint)(SUB321(auVar9 >> 0x37,0) & 1) << 6
              | (uint)(SUB321(auVar9 >> 0x3f,0) & 1) << 7 |
              (uint)(SUB321(auVar9 >> 0x47,0) & 1) << 8 | (uint)(SUB321(auVar9 >> 0x4f,0) & 1) << 9
              | (uint)(SUB321(auVar9 >> 0x57,0) & 1) << 10 |
              (uint)(SUB321(auVar9 >> 0x5f,0) & 1) << 0xb |
              (uint)(SUB321(auVar9 >> 0x67,0) & 1) << 0xc |
              (uint)(SUB321(auVar9 >> 0x6f,0) & 1) << 0xd |
              (uint)(SUB321(auVar9 >> 0x77,0) & 1) << 0xe | (uint)SUB321(auVar9 >> 0x7f,0) << 0xf |
              (uint)(SUB321(auVar9 >> 0x87,0) & 1) << 0x10 |
              (uint)(SUB321(auVar9 >> 0x8f,0) & 1) << 0x11 |
              (uint)(SUB321(auVar9 >> 0x97,0) & 1) << 0x12 |
              (uint)(SUB321(auVar9 >> 0x9f,0) & 1) << 0x13 |
              (uint)(SUB321(auVar9 >> 0xa7,0) & 1) << 0x14 |
              (uint)(SUB321(auVar9 >> 0xaf,0) & 1) << 0x15 |
              (uint)(SUB321(auVar9 >> 0xb7,0) & 1) << 0x16 | (uint)SUB321(auVar9 >> 0xbf,0) << 0x17
              | (uint)(SUB321(auVar9 >> 199,0) & 1) << 0x18 |
              (uint)(SUB321(auVar9 >> 0xcf,0) & 1) << 0x19 |
              (uint)(SUB321(auVar9 >> 0xd7,0) & 1) << 0x1a |
              (uint)(SUB321(auVar9 >> 0xdf,0) & 1) << 0x1b |
              (uint)(SUB321(auVar9 >> 0xe7,0) & 1) << 0x1c |
              (uint)(SUB321(auVar9 >> 0xef,0) & 1) << 0x1d |
              (uint)(SUB321(auVar9 >> 0xf7,0) & 1) << 0x1e | (uint)(byte)(auVar9[0x1f] >> 7) << 0x1f
      ;
      if (uVar5 != 0) {
LAB_00541631:
        iVar3 = 0;
        for (; (uVar5 & 1) == 0; uVar5 = uVar5 >> 1 | 0x80000000) {
          iVar3 = iVar3 + 1;
        }
        return (undefined1 (*) [32])(*param_1 + iVar3);
      }
      param_1 = (undefined1 (*) [32])(*param_1 + uVar6);
    }
  }
  for (; (param_1 != param_2 && ((*param_1)[0] != param_3));
      param_1 = (undefined1 (*) [32])(*param_1 + 1)) {
  }
  return param_1;
}

