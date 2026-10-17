/* Entry: 0x00544ae9; symbol: FUN_00544ae9; body bytes: 654 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined1 (*) [16] __cdecl FUN_00544ae9(undefined1 (*param_1) [16],undefined1 (*param_2) [16])

{
  ushort uVar1;
  undefined1 (*pauVar2) [16];
  uint uVar3;
  int iVar4;
  ushort uVar5;
  undefined1 (*pauVar6) [16];
  undefined1 auVar7 [16];
  undefined1 auVar8 [16];
  undefined1 auVar9 [16];
  undefined1 auVar10 [16];
  undefined1 auVar11 [16];
  undefined1 auVar12 [16];
  short sVar13;
  short sVar14;
  undefined1 (*local_8) [16];
  
  uVar1 = *(ushort *)*param_2;
  if (uVar1 != 0) {
    if (1 < DAT_005e5288) {
      if (((uint)param_2 & 0xfff) < 0xff1) {
        auVar7 = *param_2;
        uVar5 = uVar1;
      }
      else {
        iVar4 = 8;
        pauVar2 = param_2;
        auVar8 = ZEXT816(0);
        uVar5 = uVar1;
        do {
          auVar7._0_8_ = auVar8._2_8_;
          auVar7._8_8_ = auVar8._8_8_ >> 0x10 | (ulonglong)uVar5 << 0x30;
          if (uVar5 != 0) {
            uVar5 = *(ushort *)*(undefined1 (*) [16])(*pauVar2 + 2);
            pauVar2 = (undefined1 (*) [16])(*pauVar2 + 2);
          }
          iVar4 = iVar4 + -1;
          auVar8 = auVar7;
        } while (iVar4 != 0);
        uVar5 = *(ushort *)*param_2;
      }
LAB_00544b7c:
      while( true ) {
        while( true ) {
          uVar3 = (uint)param_1 & 0xfff;
          pauVar2 = param_2;
          if (uVar3 < 0xff1) break;
          if (*(ushort *)*param_1 == 0) goto LAB_00544d6d;
          pauVar6 = param_1;
          if (*(ushort *)*param_1 == uVar5) goto LAB_00544bae;
          param_1 = (undefined1 (*) [16])(*param_1 + 2);
        }
        pcmpistri(auVar7,*param_1,0xd);
        if (uVar3 < 0xff1) break;
        param_1 = param_1 + 1;
      }
      if (uVar3 < 0xff0) {
        iVar4 = pcmpistri(auVar7,*param_1,0xd);
        param_1 = (undefined1 (*) [16])(*param_1 + iVar4 * 2);
        pauVar6 = param_1;
LAB_00544bae:
        do {
          for (; 0xff0 < ((uint)param_1 & 0xfff); param_1 = (undefined1 (*) [16])(*param_1 + 2)) {
LAB_00544c09:
            if (*(short *)*pauVar2 == 0) {
              return pauVar6;
            }
            if (*(short *)*param_1 != *(short *)*pauVar2) goto LAB_00544c28;
            pauVar2 = (undefined1 (*) [16])(*pauVar2 + 2);
          }
          uVar3 = (uint)pauVar2 & 0xfff;
          if (0xff0 < uVar3) goto LAB_00544c09;
          pcmpistri(*pauVar2,*param_1,0xd);
          if (!SBORROW4(uVar3,0xff0)) goto LAB_00544c28;
          if ((int)(uVar3 - 0xff0) < 0) {
            return pauVar6;
          }
          param_1 = param_1 + 1;
          pauVar2 = pauVar2 + 1;
        } while( true );
      }
      goto LAB_00544d6d;
    }
    if (DAT_005e5288 == 1) {
      auVar8 = pshuflw(ZEXT216(uVar1),ZEXT216(uVar1),0);
LAB_00544c52:
      for (; ((uint)param_1 & 0xfff) < 0xff1; param_1 = param_1 + 1) {
        auVar9._0_2_ = -(ushort)(*(short *)*param_1 == 0);
        auVar9._2_2_ = -(ushort)(*(short *)(*param_1 + 2) == 0);
        auVar9._4_2_ = -(ushort)(*(short *)(*param_1 + 4) == 0);
        auVar9._6_2_ = -(ushort)(*(short *)(*param_1 + 6) == 0);
        auVar9._8_2_ = -(ushort)(*(short *)(*param_1 + 8) == 0);
        auVar9._10_2_ = -(ushort)(*(short *)(*param_1 + 10) == 0);
        auVar9._12_2_ = -(ushort)(*(short *)(*param_1 + 0xc) == 0);
        auVar9._14_2_ = -(ushort)(*(short *)(*param_1 + 0xe) == 0);
        sVar13 = auVar8._0_2_;
        auVar11._0_2_ = -(ushort)(*(short *)*param_1 == sVar13);
        sVar14 = auVar8._2_2_;
        auVar11._2_2_ = -(ushort)(*(short *)(*param_1 + 2) == sVar14);
        auVar11._4_2_ = -(ushort)(*(short *)(*param_1 + 4) == sVar13);
        auVar11._6_2_ = -(ushort)(*(short *)(*param_1 + 6) == sVar14);
        auVar11._8_2_ = -(ushort)(*(short *)(*param_1 + 8) == sVar13);
        auVar11._10_2_ = -(ushort)(*(short *)(*param_1 + 10) == sVar14);
        auVar11._12_2_ = -(ushort)(*(short *)(*param_1 + 0xc) == sVar13);
        auVar11._14_2_ = -(ushort)(*(short *)(*param_1 + 0xe) == sVar14);
        auVar9 = auVar9 | auVar11;
        uVar5 = (ushort)(SUB161(auVar9 >> 7,0) & 1) | (ushort)(SUB161(auVar9 >> 0xf,0) & 1) << 1 |
                (ushort)(SUB161(auVar9 >> 0x17,0) & 1) << 2 |
                (ushort)(SUB161(auVar9 >> 0x1f,0) & 1) << 3 |
                (ushort)(SUB161(auVar9 >> 0x27,0) & 1) << 4 |
                (ushort)(SUB161(auVar9 >> 0x2f,0) & 1) << 5 |
                (ushort)(SUB161(auVar9 >> 0x37,0) & 1) << 6 |
                (ushort)(SUB161(auVar9 >> 0x3f,0) & 1) << 7 |
                (ushort)(SUB161(auVar9 >> 0x47,0) & 1) << 8 |
                (ushort)(SUB161(auVar9 >> 0x4f,0) & 1) << 9 |
                (ushort)(SUB161(auVar9 >> 0x57,0) & 1) << 10 |
                (ushort)(SUB161(auVar9 >> 0x5f,0) & 1) << 0xb |
                (ushort)(SUB161(auVar9 >> 0x67,0) & 1) << 0xc |
                (ushort)(SUB161(auVar9 >> 0x6f,0) & 1) << 0xd |
                (ushort)(SUB161(auVar9 >> 0x77,0) & 1) << 0xe |
                (ushort)(byte)(auVar9[0xf] >> 7) << 0xf;
        if (uVar5 != 0) {
          uVar3 = 0;
          if (uVar5 != 0) {
            for (; (uVar5 >> uVar3 & 1) == 0; uVar3 = uVar3 + 1) {
            }
          }
          param_1 = (undefined1 (*) [16])(*param_1 + (uVar3 & 0xfffffffe));
          break;
        }
      }
      if (*(ushort *)*param_1 != 0) {
        pauVar2 = param_1;
        pauVar6 = param_2;
        if (uVar1 == *(ushort *)*param_1) {
LAB_00544c9e:
          for (; (((uint)pauVar6 & 0xfff) < 0xff1 && (((uint)pauVar2 & 0xfff) < 0xff1));
              pauVar2 = pauVar2 + 1) {
            auVar10._0_2_ = -(ushort)(*(short *)*pauVar6 == 0);
            auVar10._2_2_ = -(ushort)(*(short *)(*pauVar6 + 2) == 0);
            auVar10._4_2_ = -(ushort)(*(short *)(*pauVar6 + 4) == 0);
            auVar10._6_2_ = -(ushort)(*(short *)(*pauVar6 + 6) == 0);
            auVar10._8_2_ = -(ushort)(*(short *)(*pauVar6 + 8) == 0);
            auVar10._10_2_ = -(ushort)(*(short *)(*pauVar6 + 10) == 0);
            auVar10._12_2_ = -(ushort)(*(short *)(*pauVar6 + 0xc) == 0);
            auVar10._14_2_ = -(ushort)(*(short *)(*pauVar6 + 0xe) == 0);
            auVar12._0_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)*pauVar2 == *(short *)*pauVar6) ==
                          _DAT_00597170);
            auVar12._2_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 2) == *(short *)(*pauVar6 + 2))
                          == _UNK_00597172);
            auVar12._4_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 4) == *(short *)(*pauVar6 + 4))
                          == _UNK_00597174);
            auVar12._6_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 6) == *(short *)(*pauVar6 + 6))
                          == _UNK_00597176);
            auVar12._8_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 8) == *(short *)(*pauVar6 + 8))
                          == _UNK_00597178);
            auVar12._10_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 10) == *(short *)(*pauVar6 + 10))
                          == _UNK_0059717a);
            auVar12._12_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 0xc) ==
                                            *(short *)(*pauVar6 + 0xc)) == _UNK_0059717c);
            auVar12._14_2_ =
                 -(ushort)((ushort)-(ushort)(*(short *)(*pauVar2 + 0xe) ==
                                            *(short *)(*pauVar6 + 0xe)) == _UNK_0059717e);
            auVar12 = auVar12 | auVar10;
            uVar5 = (ushort)(SUB161(auVar12 >> 7,0) & 1) |
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
                    (ushort)(byte)(auVar12[0xf] >> 7) << 0xf;
            if (uVar5 != 0) {
              uVar3 = 0;
              if (uVar5 != 0) {
                for (; (uVar5 >> uVar3 & 1) == 0; uVar3 = uVar3 + 1) {
                }
              }
              pauVar2 = (undefined1 (*) [16])(*pauVar2 + (uVar3 & 0xfffffffe));
              pauVar6 = (undefined1 (*) [16])(*pauVar6 + (uVar3 & 0xfffffffe));
              break;
            }
            pauVar6 = pauVar6 + 1;
          }
          if (*(short *)*pauVar6 == 0) {
            return param_1;
          }
          if (*(short *)*pauVar2 == *(short *)*pauVar6) {
            pauVar2 = (undefined1 (*) [16])(*pauVar2 + 2);
            pauVar6 = (undefined1 (*) [16])(*pauVar6 + 2);
            goto LAB_00544c9e;
          }
        }
        param_1 = (undefined1 (*) [16])(*param_1 + 2);
        goto LAB_00544c52;
      }
    }
    else if (*(short *)*param_1 != 0) {
      iVar4 = (int)param_1 - (int)param_2;
      do {
        local_8 = param_2;
        do {
          if (*(short *)*local_8 == 0) {
            return param_1;
          }
        } while ((*(short *)(*local_8 + iVar4) == *(short *)*local_8) &&
                (local_8 = (undefined1 (*) [16])(*local_8 + 2), *(short *)(*local_8 + iVar4) != 0));
        if (*(short *)*local_8 == 0) {
          return param_1;
        }
        param_1 = (undefined1 (*) [16])(*param_1 + 2);
        iVar4 = iVar4 + 2;
      } while (*(short *)*param_1 != 0);
    }
LAB_00544d6d:
    param_1 = (undefined1 (*) [16])0x0;
  }
  return param_1;
LAB_00544c28:
  param_1 = (undefined1 (*) [16])(*pauVar6 + 2);
  uVar5 = uVar1;
  goto LAB_00544b7c;
}

