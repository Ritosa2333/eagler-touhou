// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4beef2

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004bd660(int param_1)

{
  short sVar1;
  byte bVar2;
  float10 fVar3;
  undefined4 uVar4;
  float fVar5;
  undefined1 uVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  undefined4 uVar10;
  undefined4 *puVar11;
  float *pfVar12;
  float *******pppppppfVar13;
  undefined4 uVar14;
  uint uVar15;
  uint uVar16;
  short *psVar17;
  bool bVar18;
  float10 fVar19;
  float *******in_XMM0_Da;
  float ******ppppppfVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  undefined1 auStack_f8 [4];
  float *******pppppppfStack_f4;
  float *******pppppppfStack_f0;
  float *******pppppppfStack_ec;
  byte bStack_e5;
  float *******pppppppfStack_e4;
  byte bStack_dd;
  undefined1 uStack_dc;
  undefined1 uStack_db;
  undefined1 uStack_da;
  undefined1 uStack_d8;
  undefined1 uStack_d7;
  undefined1 uStack_d6;
  undefined1 uStack_d4;
  undefined1 uStack_d3;
  undefined1 uStack_d2;
  undefined1 uStack_d0;
  undefined1 uStack_cf;
  undefined1 uStack_ce;
  float *******local_cc;
  int local_c8;
  float ******ppppppfStack_c4;
  undefined4 uStack_c0;
  undefined4 uStack_bc;
  float *******pppppppfStack_b8;
  undefined4 uStack_b4;
  undefined4 uStack_b0;
  undefined4 uStack_ac;
  undefined4 uStack_a8;
  undefined4 uStack_a4;
  float *******pppppppfStack_a0;
  float fStack_9c;
  undefined4 uStack_98;
  undefined4 uStack_94;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  undefined4 uStack_88;
  undefined1 auStack_84 [4];
  uint uStack_80;
  uint uStack_7c;
  float ******ppppppfStack_78;
  float *******pppppppfStack_74;
  undefined1 auStack_6c [4];
  undefined1 auStack_68 [4];
  undefined1 auStack_64 [4];
  undefined1 auStack_60 [4];
  undefined1 auStack_5c [4];
  undefined1 auStack_58 [28];
  float fStack_3c;
  float fStack_38;
  undefined4 uStack_34;
  float fStack_30;
  float fStack_2c;
  float fStack_24;
  float fStack_20;
  float fStack_18;
  float fStack_14;
  uint local_c;

  local_c = DAT_005c3d80 ^ (uint)auStack_f8;
  local_cc = DAT_005c6418;
  if ((*(byte *)(param_1 + 0x4bc) & 0x10) != 0) {
    DAT_005c6418 = (float *******)0x3f800000;
  }
  local_c8 = param_1;
  if ((*(int *)(param_1 + 0x564) == 0) || ((*(uint *)(param_1 + 0x4bc) & 0x100) != 0)) {
    in_XMM0_Da = *(float ********)(param_1 + 0x56c);
  }
  else {
    FUN_004049d0();
  }
  if (0.0 < (float)in_XMM0_Da) {
    if ((*(int *)(param_1 + 0x564) == 0) || ((*(uint *)(param_1 + 0x4bc) & 0x100) != 0)) {
      in_XMM0_Da = *(float ********)(param_1 + 0x56c);
    }
    else {
      FUN_004049d0();
    }
    in_XMM0_Da = (float *******)((float)local_cc - (float)in_XMM0_Da * (float)local_cc);
    DAT_005c6418 = in_XMM0_Da;
    if ((float)in_XMM0_Da < 0.0) {
      DAT_005c6418 = (float *******)0x0;
    }
  }
  if ((*(int *)(param_1 + 0x5f8) != 0) &&
     (iVar7 = (**(code **)(&DAT_00596450 + *(int *)(param_1 + 0x5f8) * 4))(param_1), iVar7 != 0)) {
    DAT_005c6418 = local_cc;
    return 1;
  }
  if (*(int *)(param_1 + 0x2c) < 0) {
    DAT_005c6418 = local_cc;
    return 0;
  }
  if ((*(uint *)(param_1 + 0x4bc) & 0x200000) != 0) {
    DAT_005c6418 = local_cc;
    return 0;
  }
  FUN_004d6970();
  ppppppfStack_c4 = *(float *******)(param_1 + 0x454);
  uVar15 = DAT_005aa690;
  if (ppppppfStack_c4 != (float ******)0x0) {
    iVar7 = *(int *)(param_1 + 0x1c);
    psVar17 = (short *)(*(int *)(*(int *)(*(int *)(DAT_005d19bc + 0x64c0780 + iVar7 * 4) + 0x224) +
                                *(int *)(param_1 + 0x28) * 4) + *(int *)(param_1 + 0x2c));
    iVar8 = param_1;
    goto LAB_004bd8eb;
  }
  if (((*(char *)(param_1 + 0x4c0) == '\x01') && (DAT_005d1a54 != 0)) &&
     ((*(byte *)(DAT_005d1a54 + 0x140) & 2) != 0)) {
    DAT_005c6418 = local_cc;
    return 0;
  }
LAB_004bd7c5:
  uVar16 = DAT_005aa690;
  iVar7 = *(int *)(param_1 + 0x1c);
  pppppppfStack_ec = *(float ********)(DAT_005d19bc + 0x64c0780 + iVar7 * 4);
  psVar17 = (short *)((int)pppppppfStack_ec[0x89][*(int *)(param_1 + 0x28)] +
                     *(int *)(param_1 + 0x2c));
  if (*(int *)(param_1 + 0x4e4) < (int)psVar17[2]) goto LAB_004bd995;
  iVar8 = (int)*psVar17;
  if (iVar8 < 0x12d) {
    if (iVar8 == 300) {
      *(undefined1 *)(param_1 + 0x4b4) = 1;
      pppppppfVar13 = *(float ********)(psVar17 + 4);
      if (*(int *)(param_1 + 0x610) == 0) {
        if ((psVar17[3] & 1U) != 0) {
          pppppppfVar13 = (float *******)FUN_004c1600(pppppppfVar13);
        }
      }
      else {
        pppppppfStack_f4 = *(float ********)(&DAT_00596438 + *(int *)(param_1 + 0x610) * 4);
        if ((psVar17[3] & 1U) != 0) {
          pppppppfVar13 = (float *******)FUN_004c1600(pppppppfVar13);
        }
        pppppppfVar13 = (float *******)(*(code *)pppppppfStack_f4)(param_1,pppppppfVar13);
      }
      if ((int)pppppppfVar13 < 0) {
        pppppppfVar13 = (float *******)0x120;
      }
      goto LAB_004bf1e1;
    }
    switch(iVar8) {
    default:
      goto switchD_004bd81e_caseD_0;
    case 2:
      *(undefined4 *)(param_1 + 0x2c) = 0xffffffff;
      DAT_005c6418 = local_cc;
      return 0;
    case 3:
      goto switchD_004bd81e_caseD_3;
    case 4:
      *(undefined1 *)(param_1 + 0x4b4) = 0;
switchD_004bd81e_caseD_3:
      ppppppfStack_c4 = *(float *******)(param_1 + 0x454);
      iVar8 = param_1;
      if (ppppppfStack_c4 == (float ******)0x0) {
        *(undefined1 *)(param_1 + 0x4c4) = 1;
        FUN_00404810(0);
        goto LAB_004bd995;
      }
LAB_004bd8eb:
      param_1 = local_c8;
      pppppppfStack_ec = (float *******)0x0;
      pppppppfStack_f0 = (float *******)0x0;
      pppppppfStack_e4 = (float *******)0x0;
      pppppppfStack_f4 =
           *(float ********)
            (*(int *)(*(int *)(DAT_005d19bc + 0x64c0780 + iVar7 * 4) + 0x224) +
            *(int *)(iVar8 + 0x28) * 4);
      while( true ) {
        sVar1 = *(short *)pppppppfStack_f4;
        if (sVar1 == 5) {
          bVar18 = ppppppfStack_c4 == pppppppfStack_f4[2];
        }
        else {
          bVar18 = sVar1 == -1;
        }
        if (bVar18) break;
        if ((sVar1 == 5) && (pppppppfStack_f4[2] == (float ******)0xffffffff)) {
          pppppppfStack_f0 = pppppppfStack_ec;
          pppppppfStack_e4 = pppppppfStack_f4;
        }
        pppppppfStack_ec =
             (float *******)((int)pppppppfStack_ec + (uint)*(ushort *)((int)pppppppfStack_f4 + 2));
        pppppppfStack_f4 =
             (float *******)((int)pppppppfStack_f4 + (uint)*(ushort *)((int)pppppppfStack_f4 + 2));
      }
      *(undefined4 *)(local_c8 + 0x454) = 0;
      *(undefined1 *)(local_c8 + 0x4c4) = 0;
      pppppppfVar13 = pppppppfStack_f4;
      if (*(short *)pppppppfStack_f4 == 5) {
LAB_004bdbbc:
        FUN_00402650(*(undefined4 *)(local_c8 + 0x4e4));
        *(undefined4 *)(param_1 + 0x14) = *(undefined4 *)(param_1 + 0x2c);
        pppppppfStack_ec =
             (float *******)((int)pppppppfStack_ec + (uint)*(ushort *)((int)pppppppfVar13 + 2));
        FUN_00402650((int)*(short *)(pppppppfVar13 + 1));
        uVar15 = DAT_005aa690;
        *(float ********)(param_1 + 0x2c) = pppppppfStack_ec;
        *(undefined1 *)(param_1 + 0x4b4) = 1;
      }
      else {
        if (pppppppfStack_e4 != (float *******)0x0) {
          pppppppfStack_ec = pppppppfStack_f0;
          pppppppfVar13 = pppppppfStack_e4;
          goto LAB_004bdbbc;
        }
        if (*psVar17 == 3) {
          FUN_004d5d50();
LAB_004bd995:
          if ((*(uint *)(param_1 + 0x4bc) & 0x8000) != 0) {
            FUN_004bd4b0();
          }
          if (*(char *)(param_1 + 0x4c5) != '\0') {
            *(float *)(param_1 + 0x614) = *(float *)(param_1 + 0x614) + _DAT_0062c41c;
            *(float *)(param_1 + 0x618) = *(float *)(param_1 + 0x618) + _DAT_0062c420;
            *(float *)(param_1 + 0x61c) = *(float *)(param_1 + 0x61c) + _DAT_0062c424;
          }
          if ((*(byte *)(param_1 + 0x4bc) & 0x80) != 0) {
            iVar7 = 4;
            puVar11 = &uStack_34;
            do {
              puVar11[-2] = 0;
              puVar11[-1] = 0;
              *puVar11 = 0;
              iVar7 = iVar7 + -1;
              puVar11 = puVar11 + 3;
            } while (iVar7 != 0);
            FUN_004ca2e0(&fStack_3c);
            fVar5 = DAT_005aa464;
            fVar21 = DAT_005aa454;
            fStack_3c = fStack_3c / DAT_005aa464;
            fStack_38 = fStack_38 / DAT_005aa454;
            *(float *)(param_1 + 0x3ac) = fStack_3c;
            *(float *)(param_1 + 0x3b0) = fStack_38;
            if (fStack_3c < 0.0) {
              *(undefined4 *)(param_1 + 0x3ac) = 0;
            }
            if (fStack_38 < 0.0) {
              *(undefined4 *)(param_1 + 0x3b0) = 0;
            }
            *(float *)(param_1 + 0x3b4) = fStack_30 / fVar5;
            *(float *)(param_1 + 0x3b8) = fStack_2c / fVar21;
            if (fStack_30 / fVar5 < 0.0) {
              *(undefined4 *)(param_1 + 0x3b4) = 0;
            }
            if (fStack_2c / fVar21 < 0.0) {
              *(undefined4 *)(param_1 + 0x3b8) = 0;
            }
            *(float *)(param_1 + 0x3bc) = fStack_24 / fVar5;
            *(float *)(param_1 + 0x3c0) = fStack_20 / fVar21;
            if (fStack_24 / fVar5 < 0.0) {
              *(undefined4 *)(param_1 + 0x3bc) = 0;
            }
            if (fStack_20 / fVar21 < 0.0) {
              *(undefined4 *)(param_1 + 0x3c0) = 0;
            }
            *(float *)(param_1 + 0x3c4) = fStack_18 / fVar5;
            *(float *)(param_1 + 0x3c8) = fStack_14 / fVar21;
            if (fStack_18 / fVar5 < 0.0) {
              *(undefined4 *)(param_1 + 0x3c4) = 0;
            }
            if (fStack_14 / fVar21 < 0.0) {
              *(undefined4 *)(param_1 + 0x3c8) = 0;
            }
          }
          FUN_004bc9a0();
          FUN_004bcbc0();
          if ((*(code **)(&DAT_005d19b8 + *(int *)(param_1 + 0x5f4) * 4) != (code *)0x0) &&
             (iVar7 = (**(code **)(&DAT_005d19b8 + *(int *)(param_1 + 0x5f4) * 4))(param_1),
             iVar7 != 0)) {
            DAT_005c6418 = local_cc;
            return 1;
          }
          FUN_004d6970();
          DAT_005c6418 = local_cc;
          return 0;
        }
      }
      goto LAB_004bd7c5;
    case 6:
      if ((*(byte *)(psVar17 + 3) & 1) != 0) {
        FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      FUN_004d5d30();
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 7:
      FUN_004047c0(param_1);
      uVar15 = DAT_005aa690;
      *(undefined4 *)(param_1 + 0x2c) = *(undefined4 *)(param_1 + 0x14);
      goto LAB_004bd7c5;
    case 100:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x65:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x66:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      *piVar9 = *piVar9 + (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x67:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f4 = in_XMM0_Da;
      pfVar12 = (float *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      in_XMM0_Da = (float *******)((float)pppppppfStack_f4 + *pfVar12);
      *pfVar12 = (float)in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x68:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      *piVar9 = *piVar9 - (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x69:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f4 = in_XMM0_Da;
      pfVar12 = (float *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      in_XMM0_Da = (float *******)(*pfVar12 - (float)pppppppfStack_f4);
      *pfVar12 = (float)in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x6a:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      *piVar9 = *piVar9 * (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x6b:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f4 = in_XMM0_Da;
      pfVar12 = (float *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      in_XMM0_Da = (float *******)((float)pppppppfStack_f4 * *pfVar12);
      *pfVar12 = (float)in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x6c:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      *piVar9 = *piVar9 / (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x6d:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f4 = in_XMM0_Da;
      pfVar12 = (float *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      in_XMM0_Da = (float *******)(*pfVar12 / (float)pppppppfStack_f4);
      *pfVar12 = (float)in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x6e:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      uVar15 = DAT_005aa690;
      *piVar9 = *piVar9 % (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      goto LAB_004bd7c5;
    case 0x6f:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 4);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 1) != 0) {
        FUN_004c1760();
      }
      FUN_00404480();
      pppppppfStack_f4 = pppppppfVar13;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x70:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f0 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f0 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      pppppppfStack_f4 = *(float ********)(psVar17 + 8);
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        pppppppfStack_f4 = (float *******)FUN_004c1600(pppppppfStack_f4);
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4 + (int)pppppppfStack_f0;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x71:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 8);
      pppppppfStack_f4 = pppppppfVar13;
      pppppppfStack_f0 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        FUN_004c1760();
        pppppppfStack_f4 = pppppppfVar13;
      }
      puVar11 = (undefined4 *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      in_XMM0_Da = (float *******)((float)pppppppfStack_f4 + (float)pppppppfStack_f0);
      *puVar11 = in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x72:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      pppppppfStack_f0 = *(float ********)(psVar17 + 8);
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        pppppppfStack_f0 = (float *******)FUN_004c1600(pppppppfStack_f0);
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4 - (int)pppppppfStack_f0;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x73:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 8);
      pppppppfStack_f4 = in_XMM0_Da;
      pppppppfStack_f0 = pppppppfVar13;
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        FUN_004c1760();
        pppppppfStack_f0 = pppppppfVar13;
      }
      puVar11 = (undefined4 *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      in_XMM0_Da = (float *******)((float)pppppppfStack_f4 - (float)pppppppfStack_f0);
      *puVar11 = in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x74:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f0 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f0 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      pppppppfStack_f4 = *(float ********)(psVar17 + 8);
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        pppppppfStack_f4 = (float *******)FUN_004c1600(pppppppfStack_f4);
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4 * (int)pppppppfStack_f0;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x75:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 8);
      pppppppfStack_f4 = pppppppfVar13;
      pppppppfStack_f0 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        FUN_004c1760();
        pppppppfStack_f4 = pppppppfVar13;
      }
      puVar11 = (undefined4 *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      in_XMM0_Da = (float *******)((float)pppppppfStack_f4 * (float)pppppppfStack_f0);
      *puVar11 = in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x76:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      pppppppfStack_f0 = *(float ********)(psVar17 + 8);
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        pppppppfStack_f0 = (float *******)FUN_004c1600(pppppppfStack_f0);
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4 / (int)pppppppfStack_f0;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x77:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 8);
      pppppppfStack_f4 = in_XMM0_Da;
      pppppppfStack_f0 = pppppppfVar13;
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        FUN_004c1760();
        pppppppfStack_f0 = pppppppfVar13;
      }
      puVar11 = (undefined4 *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      in_XMM0_Da = (float *******)((float)pppppppfStack_f4 / (float)pppppppfStack_f0);
      *puVar11 = in_XMM0_Da;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x78:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      pppppppfStack_f0 = *(float ********)(psVar17 + 8);
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        pppppppfStack_f0 = (float *******)FUN_004c1600(pppppppfStack_f0);
      }
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4 % (int)pppppppfStack_f0;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x79:
      if ((*(byte *)(psVar17 + 3) & 4) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 8);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      FUN_00404480();
      pppppppfStack_f4 = pppppppfVar13;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x7a:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        uVar10 = *(undefined4 *)(psVar17 + 6);
      }
      else {
        uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 6));
      }
      uVar10 = FUN_00402630(uVar10);
      puVar11 = (undefined4 *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *puVar11 = uVar10;
      uVar15 = DAT_005aa690;
      *(int *)(local_c8 + 0x2c) = *(int *)(local_c8 + 0x2c) + (uint)(ushort)psVar17[1];
      param_1 = local_c8;
      goto LAB_004bd7c5;
    case 0x7b:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      FUN_00404c90();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x7c:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      FUN_00404450();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x7d:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      FUN_00404420();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x7e:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      FUN_004043f0();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x7f:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      FUN_004043c0();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x80:
      if ((*(byte *)(psVar17 + 3) & 2) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 6);
      }
      else {
        FUN_004c1760();
      }
      FUN_00404390();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x81:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      FUN_00404320();
      pppppppfStack_f4 = in_XMM0_Da;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *piVar9 = (int)pppppppfStack_f4;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_f4;
      goto LAB_004bd7c5;
    case 0x82:
      if ((*(byte *)(psVar17 + 3) & 8) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 10);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 8);
      pppppppfStack_ec = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 4) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      pppppppfStack_f4 = in_XMM0_Da;
      pppppppfStack_e4 = (float *******)FUN_004c14c0(psVar17 + 6,psVar17[3],1);
      pppppppfStack_f0 = (float *******)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      fVar3 = (float10)fcos((float10)(float)pppppppfStack_f4);
      fVar19 = (float10)fsin((float10)(float)pppppppfStack_f4);
      *pppppppfStack_f0 = (float ******)(float)(fVar3 * (float10)(float)pppppppfStack_ec);
      *pppppppfStack_e4 = (float ******)(float)(fVar19 * (float10)(float)pppppppfStack_ec);
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      goto LAB_004bd7c5;
    case 0x83:
      FUN_00402780();
      if ((*(byte *)(psVar17 + 3) & 4) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 8);
      }
      else {
        FUN_004c1760();
      }
      fVar21 = *(float *)(psVar17 + 10);
      pppppppfStack_f0 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 8) != 0) {
        pppppppfStack_f4 = (float *******)fVar21;
        FUN_004c1760();
      }
      pppppppfStack_f4 = (float *******)fVar21;
      FUN_00404b70();
      pppppppfVar13 =
           (float *******)
           (fVar21 * ((float)pppppppfStack_f4 - (float)pppppppfStack_f0) + (float)pppppppfStack_f0);
      pppppppfStack_f4 = pppppppfVar13;
      FUN_004042b0();
      pppppppfStack_f0 = &ppppppfStack_78;
      fVar3 = (float10)fcos((float10)(float)pppppppfVar13);
      fVar19 = (float10)fsin((float10)(float)pppppppfVar13);
      ppppppfStack_78 = (float ******)(float)(fVar3 * (float10)(float)pppppppfStack_f4);
      pppppppfStack_74 = (float *******)(float)(fVar19 * (float10)(float)pppppppfStack_f4);
      pppppppfStack_e4 = pppppppfVar13;
      pfVar12 = (float *)FUN_004c14c0(psVar17 + 4,psVar17[3],0);
      *pfVar12 = (float)ppppppfStack_78;
      piVar9 = (int *)FUN_004c14c0(psVar17 + 6,psVar17[3],1);
      *piVar9 = (int)pppppppfStack_74;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
      in_XMM0_Da = pppppppfStack_74;
      goto LAB_004bd7c5;
    case 200:
      FUN_00402650(*(undefined4 *)(psVar17 + 6));
      uVar15 = DAT_005aa690;
      *(undefined4 *)(param_1 + 0x2c) = *(undefined4 *)(psVar17 + 4);
      goto LAB_004bd7c5;
    case 0xc9:
      piVar9 = (int *)FUN_004c1400(psVar17 + 4,psVar17[3],0);
      *piVar9 = *piVar9 + -1;
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        iVar7 = *(int *)(psVar17 + 4);
      }
      else {
        iVar7 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      if (iVar7 < 1) goto switchD_004bd81e_caseD_0;
      FUN_00402650(*(undefined4 *)(psVar17 + 8));
      uVar15 = DAT_005aa690;
      *(undefined4 *)(param_1 + 0x2c) = *(undefined4 *)(psVar17 + 6);
      goto LAB_004bd7c5;
    case 0xca:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        pppppppfVar13 = (float *******)FUN_004c1600(pppppppfVar13);
      }
      if (pppppppfStack_f4 != pppppppfVar13) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xcb:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      if ((float)pppppppfStack_f4 != (float)in_XMM0_Da) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xcc:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        pppppppfVar13 = (float *******)FUN_004c1600(pppppppfVar13);
      }
      if (pppppppfStack_f4 == pppppppfVar13) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xcd:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      if ((float)pppppppfStack_f4 == (float)in_XMM0_Da) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xce:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      iVar7 = *(int *)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        iVar7 = FUN_004c1600(iVar7);
      }
      if (iVar7 <= (int)pppppppfStack_f4) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xcf:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      if ((float)in_XMM0_Da <= (float)pppppppfStack_f4) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xd0:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      iVar7 = *(int *)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        iVar7 = FUN_004c1600(iVar7);
      }
      if (iVar7 < (int)pppppppfStack_f4) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xd1:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      if ((float)in_XMM0_Da < (float)pppppppfStack_f4) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xd2:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      iVar7 = *(int *)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        iVar7 = FUN_004c1600(iVar7);
      }
      if ((int)pppppppfStack_f4 <= iVar7) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xd3:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      if ((float)pppppppfStack_f4 <= (float)in_XMM0_Da) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xd4:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f4 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      iVar7 = *(int *)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        iVar7 = FUN_004c1600(iVar7);
      }
      if ((int)pppppppfStack_f4 < iVar7) {
        *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
        uVar15 = DAT_005aa690;
        goto LAB_004bd7c5;
      }
      break;
    case 0xd5:
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 4);
      }
      else {
        FUN_004c1760();
      }
      pppppppfVar13 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
      }
      in_XMM0_Da = pppppppfVar13;
      if ((float)in_XMM0_Da <= (float)pppppppfStack_f4) break;
      goto switchD_004bd81e_caseD_0;
    case -1:
    case 1:
      *(undefined1 *)(param_1 + 0x4b4) = 0;
      *(undefined4 *)(param_1 + 0x2c) = 0xffffffff;
      DAT_005c6418 = local_cc;
      return 1;
    }
    FUN_00402650(*(undefined4 *)(psVar17 + 10));
    uVar15 = DAT_005aa690;
    *(undefined4 *)(param_1 + 0x2c) = *(undefined4 *)(psVar17 + 8);
    goto LAB_004bd7c5;
  }
  if (0x14b < iVar8 - 0x12dU) goto switchD_004bd81e_caseD_0;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)((int)&PTR_caseD_1d_004c1138 + iVar8 + 3)) {
  case 0:
    *(undefined1 *)(param_1 + 0x4b4) = 1;
    if (*(int *)(param_1 + 0x610) == 0) {
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f0 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f0 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        pppppppfStack_f4 = (float *******)FUN_004c1600(pppppppfStack_f4);
      }
      uVar15 = FUN_00402540();
      pppppppfStack_f4 = (float *******)(uVar15 % (uint)pppppppfStack_f4 + (int)pppppppfStack_f0);
    }
    else {
      pppppppfStack_e4 = *(float ********)(&DAT_00596438 + *(int *)(param_1 + 0x610) * 4);
      if ((*(byte *)(psVar17 + 3) & 1) == 0) {
        pppppppfStack_f0 = *(float ********)(psVar17 + 4);
      }
      else {
        pppppppfStack_f0 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      }
      pppppppfStack_f4 = *(float ********)(psVar17 + 6);
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        pppppppfStack_f4 = (float *******)FUN_004c1600(pppppppfStack_f4);
      }
      uVar15 = FUN_00402540();
      pppppppfStack_f4 =
           (float *******)
           (*(code *)pppppppfStack_e4)
                     (param_1,uVar15 % (uint)pppppppfStack_f4 + (int)pppppppfStack_f0);
    }
    if (-1 < (int)pppppppfStack_f4) {
      FUN_004049c0();
      pppppppfVar13 = pppppppfStack_f4;
      goto LAB_004bf1e1;
    }
    goto LAB_004bf1bc;
  case 1:
    sVar1 = psVar17[4];
    *(char *)(param_1 + 0x4b7) = (char)sVar1;
    *(undefined1 *)(param_1 + 0x4c2) = 0;
    if ((char)sVar1 != '\n') goto switchD_004bd81e_caseD_0;
    FUN_004ca6f0();
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 2:
    *(char *)(param_1 + 0x4bb) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 3:
    FUN_004d0b30((char)psVar17[4]);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 4:
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = (*(byte *)(psVar17 + 4) ^ uVar16) & 1;
    break;
  case 5:
    *(char *)(param_1 + 0x4c5) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 6:
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uint)*(byte *)(psVar17 + 4) << 5 ^ uVar16) & 0x20;
    break;
  case 7:
    in_XMM0_Da = (float *******)(*(float *)(param_1 + 0x54) * DAT_005aa4d0);
    *(byte *)(param_1 + 0x4c3) = *(byte *)(param_1 + 0x4c3) ^ 1;
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(float ********)(param_1 + 0x54) = in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 8:
    in_XMM0_Da = (float *******)(*(float *)(param_1 + 0x58) * DAT_005aa4d0);
    *(byte *)(param_1 + 0x4c3) = *(byte *)(param_1 + 0x4c3) ^ 2;
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(float ********)(param_1 + 0x58) = in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 9:
    *(char *)(param_1 + 0x4b4) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 10:
    *(char *)(param_1 + 0x4c1) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0xb:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar6 = (undefined1)*(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined1 *)(param_1 + 0x4ca) = uVar6;
    uVar10 = *(undefined4 *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    uVar15 = DAT_005aa690;
    *(char *)(param_1 + 0x4c9) = (char)uVar10;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0xc:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar6 = (undefined1)*(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(uint *)(param_1 + 0x4bc) = *(uint *)(param_1 + 0x4bc) | 0x80000;
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4ce) = uVar6;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0xd:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar15 = *(uint *)(psVar17 + 4);
    }
    else {
      uVar15 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uVar15 & 0xff) * 2 ^ uVar16) & 2;
    break;
  case 0xe:
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uint)*(byte *)(psVar17 + 4) << 0x14 ^ uVar16) & 0x100000;
    break;
  case 0xf:
    *(undefined1 *)(param_1 + 0x4b5) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x10:
    *(undefined1 *)(param_1 + 0x4b5) = 0;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x11:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar15 = *(uint *)(psVar17 + 4);
    }
    else {
      uVar15 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uVar15 & 0xff) << 0x12 ^ uVar16) & 0x40000;
    break;
  case 0x12:
    fVar21 = DAT_005aa2bc / DAT_005aa2d4;
    *(undefined1 *)(param_1 + 0x4b4) = 1;
    in_XMM0_Da = (float *******)((uint)fVar21 ^ uVar15);
    if ((*(float *)(param_1 + 0x44) <= (float)in_XMM0_Da) ||
       (FUN_00404990(), DAT_005aa2bc / DAT_005aa2d4 <= (float)in_XMM0_Da)) {
      FUN_00404990();
      if (((float)in_XMM0_Da < DAT_005aa2bc / DAT_005aa2d4) ||
         (FUN_00404990(),
         DAT_005aa2bc / DAT_005aa280 + DAT_005aa2bc / DAT_005aa2d4 <= (float)in_XMM0_Da)) {
        FUN_00404990();
        if (((float)((uint)(DAT_005aa2bc / DAT_005aa2d4) ^ DAT_005aa690) < (float)in_XMM0_Da) ||
           (FUN_00404990(),
           (float)in_XMM0_Da <=
           (float)((uint)(DAT_005aa2bc / DAT_005aa280) ^ DAT_005aa690) - DAT_005aa2bc / DAT_005aa2d4
           )) {
          pppppppfStack_ec = *(float ********)(psVar17 + 6);
          if ((*(byte *)(psVar17 + 3) & 2) != 0) {
            pppppppfStack_ec = (float *******)FUN_004c1600(pppppppfStack_ec);
          }
          goto LAB_004bf19a;
        }
        pppppppfStack_ec = *(float ********)(psVar17 + 8);
        bVar2 = *(byte *)(psVar17 + 3) & 4;
      }
      else {
        pppppppfStack_ec = *(float ********)(psVar17 + 10);
        bVar2 = *(byte *)(psVar17 + 3) & 8;
      }
      if (bVar2 != 0) {
        pppppppfStack_ec = (float *******)FUN_004c1600(pppppppfStack_ec);
      }
    }
    else if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      pppppppfStack_ec = *(float ********)(psVar17 + 4);
    }
    else {
      pppppppfStack_ec = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
LAB_004bf19a:
    if (*(int *)(param_1 + 0x610) != 0) {
      pppppppfStack_ec =
           (float *******)
           (**(code **)(&DAT_00596438 + *(int *)(param_1 + 0x610) * 4))(param_1,pppppppfStack_ec);
    }
    if (-1 < (int)pppppppfStack_ec) {
      FUN_004049c0();
      pppppppfVar13 = pppppppfStack_ec;
      goto LAB_004bf1e1;
    }
LAB_004bf1bc:
    FUN_00404980();
    pppppppfVar13 = (float *******)0x120;
LAB_004bf1e1:
    FUN_004c1c20(param_1,pppppppfVar13);
    uVar15 = DAT_005aa690;
    *(undefined4 *)(param_1 + 0x458) = *(undefined4 *)(param_1 + 0x4e4);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x13:
    if ((*(byte *)(param_1 + 0x4bc) & 4) == 0) {
      if ((*(byte *)(psVar17 + 3) & 4) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 8);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f0 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
        pppppppfStack_f0 = in_XMM0_Da;
      }
      if ((*(byte *)(psVar17 + 3) & 1) != 0) {
        FUN_004c1760();
      }
      piVar9 = (int *)FUN_004026a0();
      uVar15 = DAT_005aa690;
      in_XMM0_Da = (float *******)*piVar9;
      iVar7 = piVar9[1];
      *(float ********)(param_1 + 0x30) = in_XMM0_Da;
      *(int *)(param_1 + 0x34) = iVar7;
      *(int *)(param_1 + 0x38) = piVar9[2];
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    else {
      if ((*(byte *)(psVar17 + 3) & 4) == 0) {
        in_XMM0_Da = *(float ********)(psVar17 + 8);
      }
      else {
        FUN_004c1760();
      }
      pppppppfStack_f0 = *(float ********)(psVar17 + 6);
      pppppppfStack_f4 = in_XMM0_Da;
      if ((*(byte *)(psVar17 + 3) & 2) != 0) {
        FUN_004c1760();
        pppppppfStack_f0 = in_XMM0_Da;
      }
      if ((*(byte *)(psVar17 + 3) & 1) != 0) {
        FUN_004c1760();
      }
      piVar9 = (int *)FUN_004026a0();
      uVar15 = DAT_005aa690;
      in_XMM0_Da = (float *******)*piVar9;
      iVar7 = piVar9[1];
      *(float ********)(param_1 + 0x4a0) = in_XMM0_Da;
      *(int *)(param_1 + 0x4a4) = iVar7;
      *(int *)(param_1 + 0x4a8) = piVar9[2];
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    goto LAB_004bd7c5;
  case 0x14:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x3c) = in_XMM0_Da;
    uVar10 = *(undefined4 *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    *(undefined4 *)(param_1 + 0x40) = uVar10;
    in_XMM0_Da = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x44) = in_XMM0_Da;
    *(undefined1 *)(param_1 + 0x4b8) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x15:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x54) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x58) = in_XMM0_Da;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x16:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      *(char *)(param_1 + 0x4af) = (char)*(undefined4 *)(psVar17 + 4);
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = uVar16;
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      uVar15 = DAT_005aa690;
      *(undefined1 *)(param_1 + 0x4af) = uVar6;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    goto LAB_004bd7c5;
  case 0x17:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar6 = (undefined1)*(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined1 *)(param_1 + 0x4ae) = uVar6;
    uVar10 = *(undefined4 *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    *(char *)(param_1 + 0x4ad) = (char)uVar10;
    uVar10 = *(undefined4 *)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    uVar15 = DAT_005aa690;
    *(char *)(param_1 + 0x4ac) = (char)uVar10;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x18:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      *(char *)(param_1 + 0x4b3) = (char)*(undefined4 *)(psVar17 + 4);
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = uVar16;
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      uVar15 = DAT_005aa690;
      *(undefined1 *)(param_1 + 0x4b3) = uVar6;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    goto LAB_004bd7c5;
  case 0x19:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar6 = (undefined1)*(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined1 *)(param_1 + 0x4b2) = uVar6;
    uVar10 = *(undefined4 *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    *(char *)(param_1 + 0x4b1) = (char)uVar10;
    uVar10 = *(undefined4 *)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    uVar15 = DAT_005aa690;
    *(char *)(param_1 + 0x4b0) = (char)uVar10;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x1a:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined4 *)(param_1 + 0xe0) = uVar10;
    *(undefined8 *)(param_1 + 0xa8) = DAT_00596334;
    *(undefined4 *)(param_1 + 0xb0) = DAT_0059633c;
    *(undefined8 *)(param_1 + 0xb4) = DAT_00596334;
    *(undefined4 *)(param_1 + 0xbc) = DAT_0059633c;
    *(undefined4 *)(param_1 + 0xe4) = *(undefined4 *)(psVar17 + 6);
    iVar7 = param_1 + 0x30;
    if ((*(byte *)(param_1 + 0x4bc) & 4) != 0) {
      iVar7 = param_1 + 0x4a0;
    }
    FUN_004047a0(iVar7);
    pppppppfVar13 = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      pppppppfStack_f4 = pppppppfVar13;
      FUN_004c1760();
    }
    pppppppfStack_f0 = *(float ********)(psVar17 + 10);
    pppppppfStack_f4 = pppppppfVar13;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
      pppppppfStack_f0 = pppppppfVar13;
    }
    in_XMM0_Da = *(float ********)(psVar17 + 0xc);
    bVar2 = *(byte *)(psVar17 + 3) & 0x10;
    goto LAB_004bfbee;
  case 0x1b:
    uStack_d8 = *(undefined1 *)(param_1 + 0x4ac);
    uStack_d7 = *(undefined1 *)(param_1 + 0x4ad);
    uStack_d6 = *(undefined1 *)(param_1 + 0x4ae);
    if ((*(byte *)(psVar17 + 3) & 0x10) == 0) {
      pppppppfStack_f4 = *(float ********)(psVar17 + 0xc);
    }
    else {
      pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 0xc));
    }
    pppppppfStack_f0 = *(float ********)(psVar17 + 10);
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      pppppppfStack_f0 = (float *******)FUN_004c1600(pppppppfStack_f0);
    }
    uVar10 = *(undefined4 *)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    uStack_da = (undefined1)uVar10;
    uStack_dc = SUB41(pppppppfStack_f4,0);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    uStack_db = SUB41(pppppppfStack_f0,0);
    pppppppfStack_f4 = (float *******)(uint)*(byte *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    FUN_00404690(uVar10,pppppppfStack_f4,&uStack_d8,&uStack_dc);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x1c:
    if ((*(byte *)(psVar17 + 3) & 4) == 0) {
      pppppppfStack_f4 = *(float ********)(psVar17 + 8);
    }
    else {
      pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 8));
    }
    bStack_e5 = *(byte *)(param_1 + 0x4af);
    pppppppfStack_f0 = (float *******)(uint)*(byte *)(psVar17 + 6);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    *(undefined4 *)(param_1 + 0x168) = uVar10;
    *(float ********)(param_1 + 0x16c) = pppppppfStack_f0;
    *(uint *)(param_1 + 0x140) = (uint)bStack_e5;
    *(uint *)(param_1 + 0x150) = (uint)bStack_e5;
    *(uint *)(param_1 + 0x144) = (uint)pppppppfStack_f4 & 0xff;
    goto LAB_004bfc24;
  case 0x1d:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined4 *)(param_1 + 0x1c0) = uVar10;
    *(undefined8 *)(param_1 + 0x188) = DAT_00596334;
    *(undefined4 *)(param_1 + 400) = DAT_0059633c;
    *(undefined8 *)(param_1 + 0x194) = DAT_00596334;
    *(undefined4 *)(param_1 + 0x19c) = DAT_0059633c;
    *(undefined4 *)(param_1 + 0x1c4) = *(undefined4 *)(psVar17 + 6);
    *(undefined8 *)(param_1 + 0x170) = *(undefined8 *)(param_1 + 0x3c);
    *(undefined4 *)(param_1 + 0x178) = *(undefined4 *)(param_1 + 0x44);
    pppppppfVar13 = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      pppppppfStack_f4 = pppppppfVar13;
      FUN_004c1760();
    }
    pppppppfStack_f0 = *(float ********)(psVar17 + 10);
    pppppppfStack_f4 = pppppppfVar13;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
      pppppppfStack_f0 = pppppppfVar13;
    }
    in_XMM0_Da = *(float ********)(psVar17 + 0xc);
    if ((*(byte *)(psVar17 + 3) & 0x10) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x17c) = pppppppfStack_f4;
    *(float ********)(param_1 + 0x180) = pppppppfStack_f0;
    *(float ********)(param_1 + 0x184) = in_XMM0_Da;
    FUN_00402650(0);
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4b8) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x1e:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined4 *)(param_1 + 0x1f0) = uVar10;
    FUN_00404530();
    *(undefined4 *)(param_1 + 0x1d0) = uStack_94;
    FUN_00404530();
    *(undefined4 *)(param_1 + 0x1d4) = uStack_90;
    *(undefined4 *)(param_1 + 500) = *(undefined4 *)(psVar17 + 6);
    FUN_00404530();
    *(undefined4 *)(param_1 + 0x1c8) = uStack_8c;
    in_XMM0_Da = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    FUN_00404530();
    *(undefined4 *)(param_1 + 0x1cc) = uStack_88;
    FUN_00402650(0);
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4b8) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x1f:
    if ((*(byte *)(psVar17 + 3) & 4) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 8);
    }
    else {
      FUN_004c1760();
    }
    pppppppfVar13 = *(float ********)(psVar17 + 10);
    pppppppfStack_f4 = in_XMM0_Da;
    pppppppfStack_f0 = pppppppfVar13;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
      pppppppfStack_f0 = pppppppfVar13;
    }
    pppppppfStack_e4 = (float *******)(uint)*(byte *)(psVar17 + 6);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    *(undefined4 *)(param_1 + 0x234) = uVar10;
    *(float ********)(param_1 + 0x238) = pppppppfStack_e4;
    *(float ********)(param_1 + 0x200) = pppppppfStack_f4;
    *(undefined4 *)(param_1 + 0x1fc) = *(undefined4 *)(param_1 + 0x58);
    *(undefined4 *)(param_1 + 0x1f8) = *(undefined4 *)(param_1 + 0x54);
    *(float ********)(param_1 + 0x204) = pppppppfStack_f0;
    *(undefined4 *)(param_1 + 0x21c) = *(undefined4 *)(param_1 + 0x58);
    *(undefined4 *)(param_1 + 0x218) = *(undefined4 *)(param_1 + 0x54);
    in_XMM0_Da = pppppppfStack_f0;
    FUN_00402650(0);
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x20:
    uStack_d0 = *(undefined1 *)(param_1 + 0x4b0);
    uStack_cf = *(undefined1 *)(param_1 + 0x4b1);
    uStack_ce = *(undefined1 *)(param_1 + 0x4b2);
    if ((*(byte *)(psVar17 + 3) & 0x10) == 0) {
      pppppppfStack_f4 = *(float ********)(psVar17 + 0xc);
    }
    else {
      pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 0xc));
    }
    pppppppfStack_f0 = *(float ********)(psVar17 + 10);
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      pppppppfStack_f0 = (float *******)FUN_004c1600(pppppppfStack_f0);
    }
    uVar10 = *(undefined4 *)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    uStack_d2 = (undefined1)uVar10;
    uStack_d4 = SUB41(pppppppfStack_f4,0);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    uStack_d3 = SUB41(pppppppfStack_f0,0);
    pppppppfStack_f4 = (float *******)(uint)*(byte *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    FUN_004045a0(uVar10,pppppppfStack_f4,&uStack_d0,&uStack_d4);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x21:
    if ((*(byte *)(psVar17 + 3) & 4) == 0) {
      pppppppfStack_f4 = *(float ********)(psVar17 + 8);
    }
    else {
      pppppppfStack_f4 = (float *******)FUN_004c1600(*(undefined4 *)(psVar17 + 8));
    }
    pppppppfStack_f0 = (float *******)(uint)*(byte *)(psVar17 + 6);
    bStack_e5 = *(byte *)(param_1 + 0x4b3);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    uStack_7c = (uint)bStack_e5;
    uStack_80 = (uint)pppppppfStack_f4 & 0xff;
    FUN_00404890(uVar10,pppppppfStack_f0,&uStack_7c,&uStack_80);
    uVar15 = DAT_005aa690;
    if (*(char *)(param_1 + 0x4c6) != '\0') goto switchD_004bd81e_caseD_0;
    *(undefined1 *)(param_1 + 0x4c6) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x22:
    if ((*(byte *)(psVar17 + 3) & 4) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 8);
    }
    else {
      FUN_004c1760();
    }
    pppppppfStack_f4 = *(float ********)(psVar17 + 6);
    pppppppfStack_f0 = in_XMM0_Da;
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
      pppppppfStack_f4 = in_XMM0_Da;
    }
    uVar10 = *(undefined4 *)(psVar17 + 4);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      FUN_004c1760();
    }
    *(uint *)(param_1 + 0x4bc) = *(uint *)(param_1 + 0x4bc) | 0x8000;
    uVar15 = DAT_005aa690;
    *(undefined4 *)(param_1 + 0x48) = uVar10;
    *(float ********)(param_1 + 0x4c) = pppppppfStack_f4;
    *(float ********)(param_1 + 0x50) = pppppppfStack_f0;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    in_XMM0_Da = pppppppfStack_f0;
    goto LAB_004bd7c5;
  case 0x23:
    if ((*(byte *)(psVar17 + 3) & 2) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 6);
    }
    else {
      FUN_004c1760();
    }
    uVar10 = *(undefined4 *)(psVar17 + 4);
    pppppppfStack_f4 = in_XMM0_Da;
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      FUN_004c1760();
    }
    *(uint *)(param_1 + 0x4bc) = *(uint *)(param_1 + 0x4bc) | 0x8000;
    uVar15 = DAT_005aa690;
    *(undefined4 *)(param_1 + 100) = uVar10;
    *(float ********)(param_1 + 0x68) = pppppppfStack_f4;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    in_XMM0_Da = pppppppfStack_f4;
    goto LAB_004bd7c5;
  case 0x24:
    bStack_e5 = *(byte *)(psVar17 + 4);
    bStack_dd = *(byte *)(param_1 + 0x4af);
    if ((*(byte *)(psVar17 + 3) & 2) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 6);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 6));
    }
    *(undefined4 *)(param_1 + 0x168) = uVar10;
    *(uint *)(param_1 + 0x140) = (uint)bStack_dd;
    *(uint *)(param_1 + 0x150) = (uint)bStack_dd;
    *(undefined4 *)(param_1 + 0x16c) = 0;
    *(uint *)(param_1 + 0x144) = (uint)bStack_e5;
    FUN_00402650(0);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x25:
    iVar7 = 4;
    puVar11 = &uStack_34;
    do {
      puVar11[-2] = 0;
      puVar11[-1] = 0;
      *puVar11 = 0;
      iVar7 = iVar7 + -1;
      puVar11 = puVar11 + 3;
    } while (iVar7 != 0);
    FUN_004ca2e0(&fStack_3c);
    fVar5 = DAT_005aa464;
    fVar21 = DAT_005aa454;
    fVar22 = fStack_3c / DAT_005aa464;
    fVar23 = fStack_38 / DAT_005aa454;
    *(float *)(param_1 + 0x3ac) = fVar22;
    *(float *)(param_1 + 0x3b0) = fVar23;
    if (fVar22 < 0.0) {
      *(undefined4 *)(param_1 + 0x3ac) = 0;
    }
    if (fVar23 < 0.0) {
      *(undefined4 *)(param_1 + 0x3b0) = 0;
    }
    *(float *)(param_1 + 0x3b4) = fStack_30 / fVar5;
    *(float *)(param_1 + 0x3b8) = fStack_2c / fVar21;
    if (fStack_30 / fVar5 < 0.0) {
      *(undefined4 *)(param_1 + 0x3b4) = 0;
    }
    if (fStack_2c / fVar21 < 0.0) {
      *(undefined4 *)(param_1 + 0x3b8) = 0;
    }
    *(float *)(param_1 + 0x3bc) = fStack_24 / fVar5;
    *(float *)(param_1 + 0x3c0) = fStack_20 / fVar21;
    if (fStack_24 / fVar5 < 0.0) {
      *(undefined4 *)(param_1 + 0x3bc) = 0;
    }
    if (fStack_20 / fVar21 < 0.0) {
      *(undefined4 *)(param_1 + 0x3c0) = 0;
    }
    in_XMM0_Da = (float *******)(fStack_18 / fVar5);
    *(float ********)(param_1 + 0x3c4) = in_XMM0_Da;
    *(float *)(param_1 + 0x3c8) = fStack_14 / fVar21;
    if ((float)in_XMM0_Da < 0.0) {
      *(undefined4 *)(param_1 + 0x3c4) = 0;
    }
    uVar15 = DAT_005aa690;
    if (0.0 <= fStack_14 / fVar21) goto switchD_004bd81e_caseD_0;
    *(undefined4 *)(param_1 + 0x3c8) = 0;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x26:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar15 = *(uint *)(psVar17 + 4);
    }
    else {
      uVar15 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uVar15 & 0xff) << 7 ^ uVar16) & 0x80;
    break;
  case 0x27:
    if ((*(byte *)(psVar17 + 3) & 2) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 6);
    }
    else {
      FUN_004c1760();
    }
    uVar10 = *(undefined4 *)(psVar17 + 8);
    pppppppfStack_b8 = in_XMM0_Da;
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    uVar14 = *(undefined4 *)(psVar17 + 10);
    uStack_b4 = uVar10;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
    }
    uVar10 = *(undefined4 *)(psVar17 + 0x12);
    uStack_b0 = uVar14;
    if ((*(byte *)(psVar17 + 3) & 0x80) != 0) {
      FUN_004c1760();
    }
    uVar14 = *(undefined4 *)(psVar17 + 0x14);
    uStack_ac = uVar10;
    if ((psVar17[3] & 0x100U) != 0) {
      FUN_004c1760();
    }
    uVar10 = *(undefined4 *)(psVar17 + 0x16);
    uStack_a8 = uVar14;
    if ((psVar17[3] & 0x200U) != 0) {
      FUN_004c1760();
    }
    uVar14 = *(undefined4 *)(psVar17 + 4);
    uStack_a4 = uVar10;
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar14 = FUN_004c1600(uVar14);
    }
    *(undefined4 *)(param_1 + 0xe0) = uVar14;
    *(ulonglong *)(param_1 + 0xa8) = CONCAT44(uStack_b4,pppppppfStack_b8);
    *(undefined4 *)(param_1 + 0xb0) = uStack_b0;
    *(ulonglong *)(param_1 + 0xb4) = CONCAT44(uStack_a8,uStack_ac);
    *(undefined4 *)(param_1 + 0xbc) = uStack_a4;
    iVar7 = param_1 + 0x30;
    *(undefined4 *)(param_1 + 0xe4) = 8;
    if ((*(byte *)(param_1 + 0x4bc) & 4) != 0) {
      iVar7 = param_1 + 0x4a0;
    }
    FUN_004047a0(iVar7);
    pppppppfVar13 = *(float ********)(psVar17 + 0xc);
    if ((*(byte *)(psVar17 + 3) & 0x10) != 0) {
      pppppppfStack_f4 = pppppppfVar13;
      FUN_004c1760();
    }
    pppppppfStack_f0 = *(float ********)(psVar17 + 0xe);
    pppppppfStack_f4 = pppppppfVar13;
    if ((*(byte *)(psVar17 + 3) & 0x20) != 0) {
      FUN_004c1760();
      pppppppfStack_f0 = pppppppfVar13;
    }
    in_XMM0_Da = *(float ********)(psVar17 + 0x10);
    bVar2 = *(byte *)(psVar17 + 3) & 0x40;
LAB_004bfbee:
    if (bVar2 != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x9c) = pppppppfStack_f4;
    *(float ********)(param_1 + 0xa0) = pppppppfStack_f0;
    *(float ********)(param_1 + 0xa4) = in_XMM0_Da;
    goto LAB_004bfc24;
  case 0x28:
    *(char *)(param_1 + 0x4c7) = (char)psVar17[4];
    *(char *)(param_1 + 0x4c8) = (char)psVar17[5];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x29:
    in_XMM0_Da = *(float ********)(param_1 + 0x614);
    *(float ********)(param_1 + 0x30) = in_XMM0_Da;
    *(undefined4 *)(param_1 + 0x34) = *(undefined4 *)(param_1 + 0x618);
    *(undefined4 *)(param_1 + 0x38) = *(undefined4 *)(param_1 + 0x61c);
    *(undefined4 *)(param_1 + 0x614) = 0;
    *(undefined4 *)(param_1 + 0x618) = 0;
    *(undefined4 *)(param_1 + 0x61c) = 0;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x2a:
    *(char *)(param_1 + 0x4c6) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x2b:
    *(char *)(param_1 + 0x4cc) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x2c:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(uint *)(param_1 + 0x4bc) = *(uint *)(param_1 + 0x4bc) | 0x8000;
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x3cc) = in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x2d:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(uint *)(param_1 + 0x4bc) = *(uint *)(param_1 + 0x4bc) | 0x8000;
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x3d0) = in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x2e:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_00402b90(uVar10);
    *(undefined4 *)(param_1 + 0x354) = 0;
    *(undefined4 *)(param_1 + 0x358) = 0;
    *(undefined4 *)(param_1 + 0x378) = *(undefined4 *)(psVar17 + 6);
    *(undefined4 *)(param_1 + 0x34c) = *(undefined4 *)(param_1 + 0x3cc);
    in_XMM0_Da = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x350) = in_XMM0_Da;
    goto LAB_004bfc24;
  case 0x2f:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined4 *)(param_1 + 0x3a4) = uVar10;
    *(undefined4 *)(param_1 + 900) = 0;
    *(undefined4 *)(param_1 + 0x388) = 0;
    *(undefined4 *)(param_1 + 0x3a8) = *(undefined4 *)(psVar17 + 6);
    *(undefined4 *)(param_1 + 0x37c) = *(undefined4 *)(param_1 + 0x3d0);
    in_XMM0_Da = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x380) = in_XMM0_Da;
LAB_004bfc24:
    FUN_00402650(0);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x30:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x6c) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x70) = in_XMM0_Da;
    *(undefined1 *)(param_1 + 0x4ba) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x31:
    if ((*(byte *)(psVar17 + 3) & 4) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 8);
    }
    else {
      FUN_004c1760();
    }
    pppppppfVar13 = *(float ********)(psVar17 + 10);
    pppppppfStack_f4 = in_XMM0_Da;
    pppppppfStack_f0 = pppppppfVar13;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
      pppppppfStack_f0 = pppppppfVar13;
    }
    pppppppfStack_e4 = (float *******)(uint)*(byte *)(psVar17 + 6);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    *(undefined4 *)(param_1 + 700) = uVar10;
    *(float ********)(param_1 + 0x2c0) = pppppppfStack_e4;
    *(float ********)(param_1 + 0x288) = pppppppfStack_f4;
    *(undefined4 *)(param_1 + 0x284) = *(undefined4 *)(param_1 + 0x70);
    *(undefined4 *)(param_1 + 0x280) = *(undefined4 *)(param_1 + 0x6c);
    *(float ********)(param_1 + 0x28c) = pppppppfStack_f0;
    *(undefined4 *)(param_1 + 0x2a4) = *(undefined4 *)(param_1 + 0x70);
    *(undefined4 *)(param_1 + 0x2a0) = *(undefined4 *)(param_1 + 0x6c);
    in_XMM0_Da = pppppppfStack_f0;
    FUN_00402650(0);
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4ba) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x32:
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uint)*(byte *)(psVar17 + 4) << 3 ^ uVar16) & 8;
    break;
  case 0x33:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar15 = *(uint *)(psVar17 + 4);
    }
    else {
      uVar15 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uVar15 & 0xff) << 4 ^ uVar16) & 0x10;
    break;
  case 0x34:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    *(undefined4 *)(param_1 + 0xe0) = uVar10;
    *(undefined8 *)(param_1 + 0xa8) = DAT_00596334;
    *(undefined4 *)(param_1 + 0xb0) = DAT_0059633c;
    *(undefined8 *)(param_1 + 0xb4) = DAT_00596334;
    *(undefined4 *)(param_1 + 0xbc) = DAT_0059633c;
    *(undefined4 *)(param_1 + 0xe4) = *(undefined4 *)(psVar17 + 6);
    iVar7 = param_1 + 0x30;
    if ((*(byte *)(param_1 + 0x4bc) & 4) != 0) {
      iVar7 = param_1 + 0x4a0;
    }
    FUN_004047a0(iVar7);
    fVar21 = *(float *)(psVar17 + 10);
    pppppppfStack_a0 = (float *******)0x0;
    fStack_9c = 0.0;
    uStack_98 = 0;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
    }
    pppppppfVar13 = *(float ********)(psVar17 + 8);
    pppppppfStack_ec = (float *******)fVar21;
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    pppppppfStack_f0 = (float *******)&pppppppfStack_a0;
    fVar3 = (float10)fcos((float10)(float)pppppppfVar13);
    fVar19 = (float10)fsin((float10)(float)pppppppfVar13);
    in_XMM0_Da = (float *******)(float)(fVar3 * (float10)(float)pppppppfStack_ec);
    fStack_9c = (float)(fVar19 * (float10)(float)pppppppfStack_ec);
    uStack_98 = 0;
    *(float ********)(param_1 + 0x9c) = in_XMM0_Da;
    *(float *)(param_1 + 0xa0) = fStack_9c;
    *(undefined4 *)(param_1 + 0xa4) = 0;
    pppppppfStack_f4 = pppppppfVar13;
    pppppppfStack_a0 = in_XMM0_Da;
    FUN_00402650(0);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x35:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x5c) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x60) = in_XMM0_Da;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x36:
    if ((*(byte *)(psVar17 + 3) & 4) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 8);
    }
    else {
      FUN_004c1760();
    }
    pppppppfVar13 = *(float ********)(psVar17 + 10);
    pppppppfStack_f4 = in_XMM0_Da;
    pppppppfStack_f0 = pppppppfVar13;
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
      pppppppfStack_f0 = pppppppfVar13;
    }
    pppppppfStack_e4 = (float *******)(uint)*(byte *)(psVar17 + 6);
    uVar10 = *(undefined4 *)(psVar17 + 4);
    if ((*(byte *)(psVar17 + 3) & 1) != 0) {
      uVar10 = FUN_004c1600(uVar10);
    }
    *(undefined4 *)(param_1 + 0x278) = uVar10;
    *(float ********)(param_1 + 0x27c) = pppppppfStack_e4;
    *(float ********)(param_1 + 0x244) = pppppppfStack_f4;
    *(undefined4 *)(param_1 + 0x240) = *(undefined4 *)(param_1 + 0x60);
    *(undefined4 *)(param_1 + 0x23c) = *(undefined4 *)(param_1 + 0x5c);
    *(float ********)(param_1 + 0x248) = pppppppfStack_f0;
    *(undefined4 *)(param_1 + 0x260) = *(undefined4 *)(param_1 + 0x60);
    *(undefined4 *)(param_1 + 0x25c) = *(undefined4 *)(param_1 + 0x5c);
    in_XMM0_Da = pppppppfStack_f0;
    FUN_00402650(0);
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x37:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x84) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x88) = in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x38:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      *(char *)(param_1 + 0x4cb) = (char)*(undefined4 *)(psVar17 + 4);
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = uVar16;
    }
    else {
      uVar6 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      uVar15 = DAT_005aa690;
      *(undefined1 *)(param_1 + 0x4cb) = uVar6;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    goto LAB_004bd7c5;
  case 0x39:
    *(char *)(param_1 + 0x4cd) = (char)psVar17[4];
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x3a:
    *(uint *)(param_1 + 0x4bc) =
         ((uint)*(byte *)(psVar17 + 4) << 0x10 ^ *(uint *)(param_1 + 0x4bc)) & 0x10000 ^
         *(uint *)(param_1 + 0x4bc);
    *(undefined4 *)(param_1 + 0x4d0) = *(undefined4 *)(psVar17 + 6);
    *(undefined4 *)(param_1 + 0x4d4) = *(undefined4 *)(psVar17 + 8);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x3b:
    if ((*(byte *)(param_1 + 0x4c3) & 1) != 0) {
      in_XMM0_Da = (float *******)(*(float *)(param_1 + 0x54) * DAT_005aa4d0);
      *(float ********)(param_1 + 0x54) = in_XMM0_Da;
    }
    if ((*(byte *)(param_1 + 0x4c3) & 2) != 0) {
      in_XMM0_Da = (float *******)(*(float *)(param_1 + 0x58) * DAT_005aa4d0);
      *(float ********)(param_1 + 0x58) = in_XMM0_Da;
    }
    uVar15 = DAT_005aa690;
    *(undefined1 *)(param_1 + 0x4c3) = 0;
    *(undefined1 *)(param_1 + 0x4b9) = 1;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x3c:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d0770(auStack_60,uVar10,param_1,0);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x3d:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d0770(auStack_58,uVar10,param_1,4);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x3e:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d0770(auStack_5c,uVar10,param_1,2);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x3f:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d0770(auStack_6c,uVar10,param_1,6);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x40:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d09b0(auStack_68,uVar10,param_1,0);
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = DAT_005aa690;
    goto LAB_004bd7c5;
  case 0x41:
    pppppppfStack_f4 = (float *******)((*(uint *)(param_1 + 0x4bc) >> 0xe & 1) << 2);
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d0770(&uStack_c0,uVar10,param_1,pppppppfStack_f4);
    pppppppfStack_f4 = (float *******)FUN_004cb8b0(uStack_c0);
    if (pppppppfStack_f4 == (float *******)0x0) {
      uStack_c0 = 0;
    }
    goto LAB_004bed7b;
  case 0x42:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d09b0(&uStack_bc,uVar10,param_1,0);
    pppppppfStack_f4 = (float *******)FUN_004cb8b0(uStack_bc);
    if (pppppppfStack_f4 == (float *******)0x0) {
      uStack_bc = 0;
    }
    goto LAB_004bed7b;
  case 0x43:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar15 = *(uint *)(psVar17 + 4);
    }
    else {
      uVar15 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    uVar16 = *(uint *)(param_1 + 0x4bc);
    uVar15 = ((uVar15 & 0xff) << 8 ^ uVar16) & 0x100;
    break;
  case 0x44:
    iVar7 = 0;
    if (*(char *)(param_1 + 0x4cf) != '\0') {
      iVar7 = 0x3c;
    }
    pppppppfStack_f4 = *(float ********)((int)&DAT_005d1a88 + iVar7);
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      FUN_00506940(auStack_64,*(undefined4 *)(psVar17 + 4),param_1,param_1);
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      FUN_00506940(auStack_64,uVar10,param_1,param_1);
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
      uVar15 = DAT_005aa690;
    }
    goto LAB_004bd7c5;
  case 0x45:
    iVar7 = *(int *)(param_1 + 0x568);
    if (iVar7 == 0) goto switchD_004bd81e_caseD_0;
    uVar10 = *(undefined4 *)(iVar7 + 0x464);
    uVar14 = *(undefined4 *)(iVar7 + 0x468);
    uVar4 = *(undefined4 *)(iVar7 + 0x46c);
    *(undefined4 *)(param_1 + 0x460) = *(undefined4 *)(iVar7 + 0x460);
    *(undefined4 *)(param_1 + 0x464) = uVar10;
    *(undefined4 *)(param_1 + 0x468) = uVar14;
    *(undefined4 *)(param_1 + 0x46c) = uVar4;
    uVar10 = *(undefined4 *)(iVar7 + 0x474);
    uVar14 = *(undefined4 *)(iVar7 + 0x478);
    uVar4 = *(undefined4 *)(iVar7 + 0x47c);
    *(undefined4 *)(param_1 + 0x470) = *(undefined4 *)(iVar7 + 0x470);
    *(undefined4 *)(param_1 + 0x474) = uVar10;
    *(undefined4 *)(param_1 + 0x478) = uVar14;
    *(undefined4 *)(param_1 + 0x47c) = uVar4;
    uVar10 = *(undefined4 *)(iVar7 + 0x484);
    uVar14 = *(undefined4 *)(iVar7 + 0x488);
    uVar4 = *(undefined4 *)(iVar7 + 0x48c);
    *(undefined4 *)(param_1 + 0x480) = *(undefined4 *)(iVar7 + 0x480);
    *(undefined4 *)(param_1 + 0x484) = uVar10;
    *(undefined4 *)(param_1 + 0x488) = uVar14;
    *(undefined4 *)(param_1 + 0x48c) = uVar4;
    in_XMM0_Da = *(float ********)(iVar7 + 0x490);
    uVar10 = *(undefined4 *)(iVar7 + 0x494);
    uVar14 = *(undefined4 *)(iVar7 + 0x498);
    uVar4 = *(undefined4 *)(iVar7 + 0x49c);
    *(float ********)(param_1 + 0x490) = in_XMM0_Da;
    *(undefined4 *)(param_1 + 0x494) = uVar10;
    *(undefined4 *)(param_1 + 0x498) = uVar14;
    *(undefined4 *)(param_1 + 0x49c) = uVar4;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x46:
    pppppppfStack_f0 = (float *******)FUN_004049a0(iVar7);
    pppppppfStack_f4 = (float *******)((*(uint *)(param_1 + 0x4bc) >> 0xe & 1) << 2 | 2);
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      uVar10 = *(undefined4 *)(psVar17 + 4);
    }
    else {
      uVar10 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    FUN_004d0770(auStack_84,uVar10,param_1,pppppppfStack_f4);
    pppppppfStack_f4 = (float *******)FUN_004d0990();
LAB_004bed7b:
    ppppppfVar20 = *(float *******)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    pppppppfStack_f4[0x128] = ppppppfVar20;
    in_XMM0_Da = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    pppppppfStack_f4[0x129] = (float ******)in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x47:
    *(undefined1 *)(param_1 + 0x4b7) = 9;
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
LAB_004c070a:
      iVar7 = *(int *)(psVar17 + 4);
    }
    else {
      iVar7 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    }
    goto LAB_004c070d;
  case 0x48:
    *(undefined1 *)(param_1 + 0x4b7) = 0xd;
    if ((*(byte *)(psVar17 + 3) & 1) == 0) goto LAB_004c070a;
    iVar7 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    goto LAB_004c070d;
  case 0x49:
    *(undefined1 *)(param_1 + 0x4b7) = 0xe;
    if ((*(byte *)(psVar17 + 3) & 1) == 0) goto LAB_004c070a;
    iVar7 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
LAB_004c070d:
    iVar7 = iVar7 * 0x38;
LAB_004c0719:
    *(int *)(param_1 + 0x574) = iVar7;
    uVar10 = FUN_004d7440(iVar7,"D:\\cygwin\\home\\zun\\prog\\th19\\src\\core\\sprtlib.h:909 void");
    uVar15 = DAT_005aa690;
    *(undefined4 *)(param_1 + 0x570) = uVar10;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x4a:
    *(undefined1 *)(param_1 + 0x4b7) = 0x10;
    goto LAB_004c098e;
  case 0x4b:
    *(undefined1 *)(param_1 + 0x4b7) = 0x11;
    goto LAB_004c0a0e;
  case 0x4c:
    *(undefined1 *)(param_1 + 0x4b7) = 0x12;
    goto LAB_004c0a0e;
  case 0x4d:
    *(undefined1 *)(param_1 + 0x4b7) = 0x14;
    goto LAB_004c098e;
  case 0x4e:
    *(undefined1 *)(param_1 + 0x4b7) = 0x15;
    goto LAB_004c098e;
  case 0x4f:
    *(undefined1 *)(param_1 + 0x4b7) = 0x16;
    goto LAB_004c098e;
  case 0x50:
    *(undefined1 *)(param_1 + 0x4b7) = 0x18;
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
LAB_004c07a2:
      iVar7 = *(int *)(psVar17 + 4) * 0x30;
    }
    else {
      iVar7 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
      iVar7 = iVar7 * 0x30;
    }
    goto LAB_004c0719;
  case 0x51:
    *(undefined1 *)(param_1 + 0x4b7) = 0x19;
    if ((*(byte *)(psVar17 + 3) & 1) == 0) goto LAB_004c07a2;
    iVar7 = FUN_004c1600(*(undefined4 *)(psVar17 + 4));
    iVar7 = iVar7 * 0x30;
    goto LAB_004c0719;
  case 0x52:
    *(undefined1 *)(param_1 + 0x4b7) = 0x13;
    goto LAB_004c0b9a;
  case 0x53:
    *(undefined1 *)(param_1 + 0x4b7) = 0x1b;
    goto LAB_004c098e;
  case 0x54:
    *(undefined1 *)(param_1 + 0x4b7) = 0x1a;
    goto LAB_004c098e;
  case 0x55:
    *(undefined1 *)(param_1 + 0x4b7) = 0x1c;
LAB_004c098e:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    uVar15 = DAT_005aa690;
    *(float ********)(param_1 + 0x78) = in_XMM0_Da;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    goto LAB_004bd7c5;
  case 0x56:
    *(undefined1 *)(param_1 + 0x4b7) = 0x1d;
    goto LAB_004c0b9a;
  case 0x57:
    *(undefined1 *)(param_1 + 0x4b7) = 0x1e;
    goto LAB_004c0b9a;
  case 0x58:
    *(undefined1 *)(param_1 + 0x4b7) = 0x1f;
    goto LAB_004c0a0e;
  case 0x59:
    *(undefined1 *)(param_1 + 0x4b7) = 0x20;
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    uVar15 = uVar16;
    goto LAB_004bd7c5;
  case 0x5a:
    *(undefined1 *)(param_1 + 0x4b7) = 0x21;
LAB_004c0a0e:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    iVar7 = *(int *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      iVar7 = FUN_004c1600(iVar7);
    }
    goto LAB_004c0a3d;
  case 0x5b:
    *(undefined1 *)(param_1 + 0x4b7) = 0x22;
    goto LAB_004c0b9a;
  case 0x5c:
    *(undefined1 *)(param_1 + 0x4b7) = 0x23;
    goto LAB_004c0b9a;
  case 0x5d:
    *(undefined1 *)(param_1 + 0x4b7) = 0x24;
LAB_004c0b9a:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x78) = in_XMM0_Da;
    iVar7 = *(int *)(psVar17 + 8);
    bVar18 = (*(byte *)(psVar17 + 3) & 4) == 0;
LAB_004c0aba:
    if (bVar18) {
LAB_004c0a3d:
      uVar15 = DAT_005aa690;
      *(int *)(param_1 + 0x460) = iVar7;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    else {
      uVar10 = FUN_004c1600(iVar7);
      uVar15 = DAT_005aa690;
      *(undefined4 *)(param_1 + 0x460) = uVar10;
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
    }
    goto LAB_004bd7c5;
  case 0x5e:
    *(undefined1 *)(param_1 + 0x4b7) = 0x25;
    goto LAB_004c0a5e;
  case 0x5f:
    *(undefined1 *)(param_1 + 0x4b7) = 0x26;
LAB_004c0a5e:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    uVar10 = *(undefined4 *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    *(undefined4 *)(param_1 + 0x78) = uVar10;
    in_XMM0_Da = *(float ********)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x470) = in_XMM0_Da;
    iVar7 = *(int *)(psVar17 + 10);
    bVar18 = (*(byte *)(psVar17 + 3) & 8) == 0;
    goto LAB_004c0aba;
  case 0x60:
    *(undefined1 *)(param_1 + 0x4b7) = 0x27;
    goto LAB_004c0af1;
  case 0x61:
    *(undefined1 *)(param_1 + 0x4b7) = 0x28;
    goto LAB_004c0af1;
  case 0x62:
    *(undefined1 *)(param_1 + 0x4b7) = 0x29;
LAB_004c0af1:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    uVar10 = *(undefined4 *)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    *(undefined4 *)(param_1 + 0x78) = uVar10;
    uVar10 = *(undefined4 *)(psVar17 + 8);
    if ((*(byte *)(psVar17 + 3) & 4) != 0) {
      FUN_004c1760();
    }
    *(undefined4 *)(param_1 + 0x470) = uVar10;
    in_XMM0_Da = *(float ********)(psVar17 + 10);
    if ((*(byte *)(psVar17 + 3) & 8) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x474) = in_XMM0_Da;
    iVar7 = *(int *)(psVar17 + 0xc);
    bVar18 = (*(byte *)(psVar17 + 3) & 0x10) == 0;
    goto LAB_004c0aba;
  case 99:
    *(undefined1 *)(param_1 + 0x4b7) = 0x2a;
    goto LAB_004c0c2d;
  case 100:
    *(undefined1 *)(param_1 + 0x4b7) = 0x2b;
LAB_004c0c2d:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    iVar7 = *(int *)(psVar17 + 6);
    bVar2 = *(byte *)(psVar17 + 3) & 2;
    goto LAB_004c0c52;
  case 0x65:
    *(undefined1 *)(param_1 + 0x4b7) = 0x2c;
    goto LAB_004c0c73;
  case 0x66:
    *(undefined1 *)(param_1 + 0x4b7) = 0x2d;
    goto LAB_004c0c73;
  case 0x67:
    *(undefined1 *)(param_1 + 0x4b7) = 0x2e;
LAB_004c0c73:
    if ((*(byte *)(psVar17 + 3) & 1) == 0) {
      in_XMM0_Da = *(float ********)(psVar17 + 4);
    }
    else {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x74) = in_XMM0_Da;
    in_XMM0_Da = *(float ********)(psVar17 + 6);
    if ((*(byte *)(psVar17 + 3) & 2) != 0) {
      FUN_004c1760();
    }
    *(float ********)(param_1 + 0x78) = in_XMM0_Da;
    iVar7 = *(int *)(psVar17 + 8);
    bVar2 = *(byte *)(psVar17 + 3) & 4;
LAB_004c0c52:
    if (bVar2 != 0) {
      iVar7 = FUN_004c1600(iVar7);
    }
    iVar7 = iVar7 * 2;
    goto LAB_004c0a3d;
  case 0x68:
    goto switchD_004bd81e_caseD_0;
  }
  *(uint *)(param_1 + 0x4bc) = uVar15 ^ uVar16;
switchD_004bd81e_caseD_0:
  *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + (uint)(ushort)psVar17[1];
  uVar15 = DAT_005aa690;
  goto LAB_004bd7c5;
}


// Reference 004c11d8
// Reference 004becaf
// Address 4cb830

void FUN_004cb830(undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;

  iVar2 = FUN_004cb8b0(param_1);
  if (iVar2 != 0) {
    if (*(int *)(iVar2 + 0x604) != 0) {
      (**(code **)(&DAT_00596420 + *(int *)(iVar2 + 0x604) * 4))(iVar2,param_2);
    }
    *(undefined4 *)(iVar2 + 0x454) = param_2;
    for (piVar1 = *(int **)(iVar2 + 0x540); piVar1 != (int *)0x0; piVar1 = (int *)piVar1[1]) {
      iVar2 = *piVar1;
      if (*(int *)(iVar2 + 0x604) != 0) {
        (**(code **)(&DAT_00596420 + *(int *)(iVar2 + 0x604) * 4))(iVar2,param_2);
      }
      *(undefined4 *)(iVar2 + 0x454) = param_2;
    }
  }
  return;
}


// Reference 00410803
// Reference 00412b49
// Reference 00412b52
// Reference 00412b75
// Reference 00412b7e
// Reference 004181f8
// Reference 00418202
// Reference 004080e4
// Reference 0040a464
// Reference 00418428
// Reference 00418432
// Reference 00544d52
// Reference 00419068
// Reference 00419072
// Reference 00548684
// Reference 00548a85
// Reference 0041c27a
// Reference 0041c284
// Reference 0041d93f
// Reference 0041d949
// Reference 0041f06e
// Reference 0041f078
// Reference 0041f3e8
// Reference 0041f3f2
// Reference 0041f3ff
// Reference 0041f40c
// Reference 00420fbf
// Reference 00420fc9
// Reference 00422d37
// Reference 00422d41
// Reference 0042475d
// Reference 00424767
// Reference 00425d4d
// Reference 00425d57
// Reference 00425d64
// Reference 00425d71
// Reference 004260b8
// Reference 004260c2
// Reference 004260cf
// Reference 004260dc
// Reference 004273bd
// Reference 004273c7
// Reference 00429187
// Reference 00429191
// Reference 004298c8
// Reference 004298d2
// Reference 004298df
// Reference 004298ec
// Reference 0042acb7
// Reference 0042acc1
// Reference 0042c0f7
// Reference 0042c101
// Reference 0042d3e1
// Reference 0042d3eb
// Reference 0042fe99
// Reference 0042fea2
// Reference 0042fec5
// Reference 0042fece
// Reference 0055b85c
// Reference 004315c7
// Reference 004cb528
// Reference 004dd0dd
// Reference 004e8038
// Reference 004f533c
// Reference 004f53a4
// Reference 004f5523
// Reference 004f6f94
// Reference 004f6fe6
// Reference 00506b04
// Reference 0050f167
// Reference 0050f1a7
// Reference 005131fa
// Reference 0051337c
// Reference 00513464
// Reference 00513514
// Reference 005136aa
// Reference 005136b4
// Reference 0051370b
// Reference 00513715
// Reference 0051377d
// Reference 00513787
// Reference 005137a8
// Reference 005137b2
// Reference 005137d3
// Reference 005137dd
// Reference 0053ab01
// Reference 00514136
// Reference 00515d09
// Reference 00515e6c
// Reference 005161f0
// Reference 00516203
// Reference 0051621a
// Reference 00516629
// Reference 0051663b
// Reference 00516a8d
// Reference 0053a985
// Reference 0051c8e1
// Reference 005346f4
// Reference 00534701
// Reference 0053470e
// Reference 00534754
// Reference 00534761
// Reference 0053476e
// Reference 00539502
// Reference 0053b3ea
// Reference 0053c488
// Reference 0053c492
// Reference 0053c578
// Reference 0053c582
// Reference 0053d928
// Reference 0053d932
// Reference 0053da18
// Reference 0053da22
// Reference 0053fa38
// Reference 0053fa42
// Reference 0053fb28
// Reference 0053fb32
// Reference 00541b78
// Reference 00541b82
// Reference 0054df6d
// Reference 00546dfd
// Reference 00546e0a
// Reference 00547432
// Reference 00549ade
// Reference 00549ae8
// Reference 00549b1e
// Reference 00549b28
// Reference 0054a89e
// Reference 0054ab37
// Reference 0054c1a0
// Reference 0054c374
// Reference 0054c6de
// Reference 0054c7db
// Reference 0054cc41
// Reference 0054ccbc
// Reference 0054cdc9
// Reference 0054d841
// Reference 0054d92b
// Reference 00555d82
// Reference 00555d99
// Reference 005566a1
// Reference 005566c6
// Reference 005566ea
// Reference 00510273
// Reference 00510393
// Reference 0051040a
// Reference 00510481
// Reference 005105fd
// Reference 00510606
// Reference 00510ac3
// Reference 00510d75
// Reference 00510d7e
// Reference 00510ffe
// Reference 00511008
// Reference 00511017
// Reference 00511021
// Reference 00511045
// Reference 00511063
// Reference 00511087
// Reference 005111d6
// Reference 0051132d
// Reference 005115a1
// Reference 00511674
// Reference 00511e00
// Reference 005120cd
// Reference 00409d6c
// Reference 00409d86
// Reference 004e158f
// Reference 004e16fc
// Reference 004e219e
// Reference 004e2e99
// Reference 004e2ec3
// Reference 004e2ed9
// Reference 004e2eed
// Reference 004e2f07
// Reference 004e18bf
// Reference 004e2220
// Reference 004e2241
// Reference 004e2279
// Reference 004e2298
// Reference 004e22ac
// Reference 004e22c4
// Reference 004e22df
// Reference 004e231c
// Reference 004e2330
// Reference 004e2348
// Reference 004e2363
// Reference 004e2395
// Reference 004e23b6
// Reference 004e23ea
// Reference 004e2402
// Reference 004e2416
// Reference 004e2435
// Reference 004e2454
// Reference 004e2486
// Reference 004e249a
// Reference 004e24b9
// Reference 004e24d8
// Reference 004e25ca
// Reference 004e2903
// Reference 004e2939
// Reference 004e3576
// Reference 00554ad5
// Reference 00554aec
// Reference 00554b03
// Reference 00554b1a
// Reference 00554b31
// Reference 00554e4d
// Reference 00554f8d
// Reference 00554f9a
// Reference 00554fa7
// Reference 00554fb4
// Reference 00555048
// Reference 005550fb
// Reference 0051dd84
// Reference 0051df44
// Reference 0051dfe6
// Reference 00519bcc
// Reference 00519be7
// Reference 00519fb9
// Reference 00519fd4
// Reference 0051a154
// Reference 0051a169
// Reference 0051a398
// Reference 0051a3ad
// Reference 0051a50a
// Reference 0051a525
// Reference 005597af
// Reference 0055b5ae
// Reference 0055b5c5
// Reference 00555c7b
// Reference 00555ca4
// Reference 00555b09
// Reference 00555ce2
// Reference 00555cf9
// Reference 0055a706
// Reference 0055a48b
// Reference 0055a4a2
// Reference 00537ebc
// Reference 00537f43
// Reference 0053814c
// Reference 005383bd
// Reference 00538602
// Reference 0053860f
// Reference 0053861c
// Reference 005383fb
// Reference 0053841c
// Reference 0053847e
// Reference 005384de
// Reference 00538521
// Reference 00538597
// Reference 005385c1
// Reference 0053865b
// Reference 0053866f
// Reference 00538d25
// Reference 005390f3
// Reference 00539100
// Reference 0053910d
// Reference 0053913b
// Reference 00539148
// Reference 00539155
// Reference 005381ad
// Reference 005381ba
// Reference 005381c7
// Reference 005381ea
// Reference 00538206
// Reference 00538222
// Reference 0053823e
// Reference 0053825a
// Reference 0053829d
// Reference 005382b9
// Reference 005382d9
// Reference 005382f5
// Reference 00538315
// Reference 00538331
// Reference 0053628c
// Reference 00536299
// Reference 00536461
// Reference 00553c21
// Reference 00558800
// Reference 00558882
// Reference 005588c6
// Reference 00558e11
// Reference 00558e55
// Reference 005589a8
// Reference 005589ed
// Reference 00558a34
// Reference 00558d06
// Reference 00558d4a
// Reference 005591e3
// Reference 00559216
// Reference 0055925f
// Reference 005592a3
// Reference 00556e6e
// Reference 00557498
// Reference 005575f7
// Reference 00557641
// Reference 00557c9e
// Reference 00557e07
// Reference 00557e53
// Reference 00557f70
// Reference 0055832b
// Reference 005584d9
// Reference 005584fe
// Reference 00558522
// Reference 00558539
// Reference 004f674c
// Reference 004f6756
// Reference 004f6760
// Reference 005593fe
// Reference 00559443
// Reference 00559507
// Reference 00559551
// Reference 0055957b
// Reference 00559838
// Reference 0055986c
// Reference 005598c4
// Reference 00559973
// Reference 005599a1
// Reference 005599c7
// Reference 005599e3
// Reference 00540b1a
// Reference 00540b24
// Reference 00518c4b
// Reference 00518c58
// Reference 00518c65
// Reference 00518df3
// Reference 00518e00
// Reference 00518e0d
// Reference 00506c38
// Reference 00506c9f
// Reference 00506d18
// Reference 00506d54
// Reference 00506b5b
