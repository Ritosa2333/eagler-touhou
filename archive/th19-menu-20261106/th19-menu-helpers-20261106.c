// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 5593b0

undefined4 __fastcall FUN_005593b0(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 extraout_ECX;
  int iVar4;
  undefined4 *local_10 [3];

  iVar4 = DAT_005d19a8;
  switch(*(undefined4 *)(param_1 + 0x20)) {
  case 0:
    FUN_0055bb60(9);
    uVar1 = DAT_005d1c1c;
    *(undefined4 *)(param_1 + 0x34) = 5;
    *(undefined4 *)(param_1 + 0xfc) = 0;
    FUN_0040a670(uVar1);
    FUN_004cb830(*(undefined4 *)(param_1 + 0x920),1);
    *(undefined4 *)(param_1 + 0x920) = 0;
    puVar2 = (undefined4 *)FUN_00405530(local_10,extraout_ECX,0xbe,0xffffffff,extraout_ECX);
    uVar1 = *puVar2;
    *(undefined4 *)(param_1 + 0x920) = uVar1;
    FUN_004cb7a0(uVar1,3);
    FUN_004cb830(*(undefined4 *)(param_1 + 0x920),(int)(short)(*(short *)(param_1 + 0x2c) + 7));
    FUN_00431620(1);
  case 1:
    if (6 < *(int *)(param_1 + 0x618)) {
      FUN_00431620(2);
      return 1;
    }
    break;
  case 2:
    *(undefined4 *)(param_1 + 0x30) = *(undefined4 *)(param_1 + 0x2c);
    if (iVar4 != 0) {
      iVar3 = FUN_00401a60(0x10);
      if ((iVar3 != 0) || (iVar3 = FUN_00401a60(0x40), iVar3 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      iVar3 = FUN_00401a60(0x20);
      if ((iVar3 != 0) || (iVar3 = FUN_00401a60(0x80), iVar3 != 0)) {
        FUN_004d4fa0(1);
      }
    }
    if (*(int *)(param_1 + 0x30) != *(int *)(param_1 + 0x2c)) {
      FUN_004bb8d0(10,0);
      FUN_004cb7a0(*(undefined4 *)(param_1 + 0x920),3);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x920),(int)(short)(*(short *)(param_1 + 0x2c) + 7));
      iVar4 = DAT_005d19a8;
    }
    local_10[0] = (undefined4 *)(param_1 + 0x920);
    if (iVar4 != 0) {
      if ((*(uint *)(iVar4 + 0x10) & 0x106) != 0) {
        FUN_00431620(4);
        FUN_004bb8d0(9,0);
        FUN_004cb830(*local_10[0],1);
        *(undefined4 *)(param_1 + 0x920) = 0;
        return 1;
      }
      if ((*(uint *)(iVar4 + 0x10) & 0x80001) != 0) {
        FUN_004cb830(*local_10[0],6);
        FUN_00431620(3);
        FUN_004bb8d0(7,0);
        return 1;
      }
    }
    break;
  case 3:
    if (0xd < *(int *)(param_1 + 0x618)) {
      if (*(int *)(param_1 + 0x2c) == 4) {
        FUN_004316b0(4);
        DAT_005d1c1c = *(undefined4 *)(param_1 + 0x2c);
        return 1;
      }
      if (*(int *)(param_1 + 0x2c) != 3) {
        FUN_004316b0(8);
        FUN_00431570(*(undefined4 *)(param_1 + 0x2c));
        DAT_005d1c1c = FUN_00414960();
        DAT_0062bb10 = DAT_0062be04;
        DAT_0062bb14 = DAT_0062be08;
        DAT_0062bb18 = DAT_0062be0c;
        FUN_004d5090();
        *(undefined4 *)(param_1 + 0xfc) = 1;
        *(undefined4 *)(param_1 + 0x34) = 0x13;
        FUN_0040a670(0);
        return 1;
      }
      FUN_004316b0(5);
      DAT_005d1c1c = *(undefined4 *)(param_1 + 0x2c);
      return 1;
    }
    break;
  case 4:
    if (5 < *(int *)(param_1 + 0x618)) {
      DAT_005d1c1c = *(undefined4 *)(param_1 + 0x2c);
      FUN_004316b0(1);
      FUN_0040a670(1);
    }
  }
  return 1;
}


// Reference 0055b561
// Address 4d4fa0

int __thiscall FUN_004d4fa0(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;

  iVar1 = param_1[2];
  if (iVar1 < 1) {
    return *param_1;
  }
  do {
    iVar2 = *param_1 + param_2;
    *param_1 = iVar2;
    if (iVar1 <= iVar2) {
      do {
        if (param_1[0x34] == 0) {
          iVar2 = param_1[2] + -1;
        }
        else {
          iVar2 = iVar2 - param_1[2];
        }
        *param_1 = iVar2;
      } while (iVar1 <= iVar2);
    }
    piVar4 = param_1 + 0x24;
    if (iVar2 < 0) {
      if (param_1[0x34] == 0) {
        iVar2 = 0;
        *param_1 = 0;
      }
      else {
        do {
          iVar2 = iVar2 + iVar1;
          *param_1 = iVar2;
        } while (iVar2 < 0);
      }
    }
    iVar3 = 0;
    while( true ) {
      if (param_1[0x35] <= iVar3) {
        return iVar2;
      }
      if (*piVar4 == iVar2) break;
      iVar3 = iVar3 + 1;
      piVar4 = piVar4 + 1;
    }
  } while( true );
}


// Reference 004e006e
// Reference 004e02ae
// Reference 005527d0
// Reference 005527f3
// Reference 00552871
// Reference 00552897
// Reference 00566f68
// Reference 00566fad
// Reference 00566fe8
// Reference 0056701d
// Reference 004e1856
// Reference 004e1885
// Reference 004e222d
// Reference 004e23a2
// Reference 004e26f6
// Reference 004e272f
// Reference 004e2802
// Reference 004e3171
// Reference 004e31f8
// Reference 00522b89
// Reference 00522bb5
// Reference 00522c9d
// Reference 00522cba
// Reference 00522dee
// Reference 0055497c
// Reference 0055499e
// Reference 005549d7
// Reference 00554a06
// Reference 0051dcca
// Reference 0051dce4
// Reference 0051dee2
// Reference 0051df9b
// Reference 00519ce0
// Reference 00519d02
// Reference 00519d0b
// Reference 00519daf
// Reference 00519dc6
// Reference 00519e91
// Reference 00519eb0
// Reference 00519ee6
// Reference 00519f11
// Reference 00519f1a
// Reference 00519f31
// Reference 0051a293
// Reference 0051a2aa
// Reference 00555812
// Reference 00555832
// Reference 0055a396
// Reference 0055a3c4
// Reference 00536de0
// Reference 00536dfb
// Reference 00538114
// Reference 00538134
// Reference 00538446
// Reference 00538466
// Reference 0053875c
// Reference 0053877f
// Reference 005387b5
// Reference 005387be
// Reference 005387fc
// Reference 00538b84
// Reference 00538ba4
// Reference 00538c99
// Reference 0053627f
// Reference 00536454
// Reference 00567cbc
// Reference 00567ce9
// Reference 00567d21
// Reference 00567d4c
// Reference 00558941
// Reference 00558968
// Reference 0052357e
// Reference 00523597
// Reference 005594a0
// Reference 005594c7
// Reference 00559917
// Reference 0055993d
// Address 4d5090

void __fastcall FUN_004d5090(undefined4 *param_1)

{
  param_1[param_1[0x23] + 3] = *param_1;
  param_1[param_1[0x23] + 0x13] = param_1[2];
  param_1[0x23] = param_1[0x23] + 1;
  param_1[0x35] = 0;
  if (0xf < (int)param_1[0x23]) {
    param_1[0x23] = 0xf;
  }
  return;
}


// Reference 0055b015
// Reference 0055b062
// Reference 0055b07b
// Reference 0055b363
// Reference 0055a717
// Reference 0055a52d
// Reference 0055a56d
// Reference 0055a5ad
// Reference 0055a5e3
// Reference 0055a619
// Reference 0055a64f
// Reference 0055a685
// Reference 005383d6
// Reference 005386b4
// Reference 005591d6
// Reference 00523716
// Reference 00558340
// Reference 00559633
// Reference 00559a3a
// Address 4d5040

void __fastcall FUN_004d5040(undefined4 *param_1)

{
  int iVar1;

  iVar1 = param_1[0x23];
  param_1[0x23] = iVar1 + -1;
  if (iVar1 + -1 < 0) {
    param_1[0x23] = 0;
    *param_1 = 0;
    param_1[0x35] = 0;
    return;
  }
  *param_1 = param_1[iVar1 + 2];
  param_1[2] = param_1[iVar1 + 0x12];
  param_1[0x35] = 0;
  return;
}


// Reference 00556705
// Reference 00522fd2
// Reference 00554e74
// Reference 00555d28
// Reference 00538646
// Reference 00538677
// Reference 00538cff
// Reference 00558554
// Reference 00559ac9
// Address 556340

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00556340(int param_1)

{
  int iVar1;
  uint ****ppppuVar2;
  int iVar3;
  char cVar4;
  int iVar5;
  short **ppsVar6;
  int *piVar7;
  uint uVar8;
  uint *puVar9;
  int iVar10;
  uint ******ppppppuVar11;
  undefined4 *puVar12;
  uint *******pppppppuVar13;
  uint ******ppppppuVar14;
  uint ****ppppuVar15;
  uint ******ppppppuVar16;
  uint ***pppuVar17;
  undefined4 *puVar18;
  uint *******pppppppuVar19;
  int *extraout_ECX;
  int *extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  uint *******extraout_ECX_03;
  undefined4 extraout_ECX_04;
  uint ***pppuVar20;
  int *piVar21;
  short **ppsVar22;
  uint *****pppppuVar23;
  uint ****ppppuVar24;
  uint ******ppppppuVar25;
  uint ****ppppuVar26;
  bool bVar27;
  float fVar28;
  undefined4 in_stack_fffffeb8;
  undefined4 uVar29;
  undefined *puVar30;
  undefined1 local_130 [4];
  undefined *local_12c;
  undefined4 local_128;
  short *local_124;
  undefined4 local_120;
  undefined4 local_11c;
  int local_118;
  undefined1 local_114 [4];
  undefined4 local_110;
  undefined4 local_10c;
  int local_108 [7];
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  int local_e0 [30];
  short *local_68;
  float local_64;
  float local_60;
  undefined4 local_5c;
  undefined8 local_58;
  uint *****local_50;
  int local_4c [5];
  uint *****local_38;
  undefined8 local_34;
  uint ***local_2c;
  uint ******local_28;
  uint *****local_24;
  uint ******local_20;
  uint ******local_1c;
  uint ******local_18;
  uint *******local_14;
  void *local_10;
  undefined1 *puStack_c;
  int local_8;

  local_8 = 0xffffffff;
  puStack_c = &LAB_0057891a;
  local_10 = ExceptionList;
  ExceptionList = &local_10;
  puVar30 = &DAT_0062b018;
  local_12c = &DAT_0062b018;
  uVar29 = 0x556383;
  local_4c[2] = param_1;
  local_108[6] = FUN_0046230c(&DAT_0062b018,DAT_005c3d80 ^ (uint)&stack0xfffffffc);
  if (local_108[6] != 0) {
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(5);
  }
  if (DAT_0062b044 == 0x7fffffff) {
    DAT_0062b044 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(6);
  }
  local_8 = local_108[6];
  piVar21 = local_108 + 6;
  if (-1 < (int)DAT_0062b254) {
    piVar21 = (int *)&DAT_0062b254;
  }
  local_108[1] = 3;
  local_118 = 3;
  local_108[0] = 0;
  piVar7 = local_108 + 1;
  if ((int)DAT_0062b254 < 4) {
    piVar7 = piVar21;
  }
  local_120 = 3;
  local_11c = 0;
  local_4c[0] = 0x4e;
  local_4c[1] = 0x4f;
  iVar5 = *piVar7;
  local_1c = (uint ******)&DAT_005a9c18;
  local_4c[3] = 0x15;
  if (iVar5 != 0) {
    local_1c = (uint ******)&DAT_005a9ad8;
  }
  local_4c[4] = 0x16;
  piVar21 = local_108;
  if (-1 < iVar5) {
    piVar21 = (int *)&DAT_0062b254;
  }
  local_34 = 0x200000002;
  piVar7 = &local_118;
  if (iVar5 < 4) {
    piVar7 = piVar21;
  }
  puVar18 = &local_11c;
  iVar5 = *piVar7;
  local_18 = (uint ******)((iVar5 != 0) + 0x13);
  if (-1 < iVar5) {
    puVar18 = &DAT_0062b254;
  }
  puVar12 = &local_120;
  if (iVar5 < 4) {
    puVar12 = puVar18;
  }
  ppppppuVar11 = (uint ******)*puVar12;
  local_28 = ppppppuVar11;
  local_24 = (uint *****)((ppppppuVar11 != (uint ******)0x0) + 0x4c);
  DAT_0062b254 = ppppppuVar11;
  if ((ppppppuVar11 == (uint ******)0x2) && (DAT_0062b258 == (uint *******)0x3)) {
    uVar29 = 0x5564c0;
    iVar5 = FUN_00563750(2);
    if (iVar5 != 0) {
      if ((*(uint *)(param_1 + 0x624) & 1) == 0) {
        *(undefined4 *)(param_1 + 0x618) = 0;
        *(undefined4 *)(param_1 + 0x61c) = 0;
        *(undefined4 *)(param_1 + 0x620) = 0;
        *(uint *)(param_1 + 0x624) = *(uint *)(param_1 + 0x624) | 1;
      }
      *(undefined4 *)(param_1 + 0x618) = 0;
      *(undefined4 *)(param_1 + 0x61c) = 0;
      *(undefined4 *)(param_1 + 0x614) = 0xffffffff;
      goto switchD_005567a0_caseD_1;
    }
    puVar9 = (uint *)(DAT_005d1c28 + 0xee0);
    if (*(int *)(DAT_005d1c28 + 0x11fc) != 0) {
      puVar9 = (uint *)(DAT_005d1c28 + 0xf00);
    }
    DAT_0062bb10 = *puVar9;
    puVar9 = (uint *)(DAT_005d1c28 + 0xee4);
    if (*(int *)(DAT_005d1c28 + 0x11fc) != 0) {
      puVar9 = (uint *)(DAT_005d1c28 + 0xf04);
    }
    DAT_0062bb14 = *puVar9;
    puVar9 = (uint *)(DAT_005d1c28 + 0xee8);
    if (*(int *)(DAT_005d1c28 + 0x11fc) != 0) {
      puVar9 = (uint *)(DAT_005d1c28 + 0xf08);
    }
    DAT_0062bb18 = *puVar9;
    cVar4 = FUN_004493b0();
    if (cVar4 != '\0') {
      piVar21 = (int *)FUN_00448e70();
      puVar30 = (undefined *)0x5565a0;
      iVar5 = (**(code **)(*piVar21 + 0x14))();
      if (1 < iVar5) goto LAB_005565af;
    }
    iVar5 = DAT_005d1c28;
    DAT_005d19a0 = &DAT_00623e40;
    DAT_005d19a4 = &DAT_00624100;
    DAT_005d19a8 = &DAT_006243c0;
    *(int *)(DAT_005d1c28 + 0x34) = *(int *)(DAT_005d1c28 + 0x34) + 1;
    DAT_005d19ac = &DAT_00624680;
    if ((0x77 < *(int *)(iVar5 + 0x34)) && ((DAT_006243c0 & 0x80001) != 0)) {
      FUN_004315b0(local_24);
      iVar5 = 0;
      do {
        if (-1 < *(int *)((int)local_4c + iVar5)) {
          FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + *(int *)((int)local_4c + iVar5) * 4),1);
          *(undefined4 *)(param_1 + 0x628 + *(int *)((int)local_4c + iVar5) * 4) = 0;
        }
        iVar10 = *(int *)((int)local_4c + iVar5 + 0xc);
        if (-1 < iVar10) {
          FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + iVar10 * 4),1);
          *(undefined4 *)(param_1 + 0x628 + *(int *)((int)local_4c + iVar5 + 0xc) * 4) = 0;
        }
        iVar5 = iVar5 + 4;
      } while (iVar5 < 8);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x96c),1);
      *(undefined4 *)(param_1 + 0x96c) = 0;
      FUN_004316b0(10);
      FUN_004d5040();
      FUN_004e3740();
      FUN_004e3740();
      FUN_00409f40();
      *(uint *******)(param_1 + 0x34) = local_18;
      *(undefined4 *)(param_1 + 0xfc) = 0;
      FUN_0040a670(0);
    }
    goto switchD_005567a0_caseD_1;
  }
LAB_005565af:
  local_20 = (uint ******)0x3;
  local_14 = (uint *******)0x0;
  pppppppuVar19 = (uint *******)&local_14;
  if (-1 < (int)DAT_0062b254) {
    pppppppuVar19 = &DAT_0062b254;
  }
  pppppppuVar13 = &local_20;
  if ((int)DAT_0062b254 < 4) {
    pppppppuVar13 = pppppppuVar19;
  }
  DAT_0062b254 = *pppppppuVar13;
  if ((DAT_0062b254 == (uint ******)0x0) || (DAT_0062b254 == (uint ******)0x1)) {
    local_4c[4] = 0xffffffff;
    local_4c[0] = 0x50;
    local_4c[1] = 0xffffffff;
    goto LAB_00556780;
  }
  pppppppuVar13 = DAT_0062b258;
  if ((short *)((int)DAT_0062b254 + -2) == (short *)0x0) {
    ppsVar22 = &local_124;
    local_124 = (short *)((int)DAT_0062b254 + -2);
    local_68 = (short *)0x4;
    if (-1 < (int)DAT_0062b258) {
      ppsVar22 = (short **)&DAT_0062b258;
    }
    ppsVar6 = (short **)(local_e0 + 0x1e);
    if ((int)DAT_0062b258 < 5) {
      ppsVar6 = ppsVar22;
    }
    pppppppuVar13 = (uint *******)*ppsVar6;
    pppppppuVar19 = DAT_0062b258;
    DAT_0062b258 = pppppppuVar13;
    switch(pppppppuVar13) {
    case (uint *******)0x1:
      local_4c[4] = 0x18;
      break;
    case (uint *******)0x2:
      local_4c[3] = 0x17;
      local_4c[4] = 0x18;
      break;
    case (uint *******)0x3:
    case (uint *******)0x0:
      local_34._4_4_ = 1;
      goto LAB_00556787;
    default:
      goto switchD_00556631_default;
    }
LAB_00556780:
    local_34._4_4_ = 0;
LAB_00556787:
    local_34 = (ulonglong)local_34._4_4_ << 0x20;
    pppppppuVar13 = DAT_0062b258;
  }
switchD_00556631_default:
  DAT_0062b258 = pppppppuVar13;
  *(int *)(param_1 + 0x5d8c) = *(int *)(param_1 + 0x5d8c) + 1;
  switch(*(undefined4 *)(param_1 + 0x20)) {
  case 0:
    FUN_004132c0(0x13,pppppppuVar19);
    FUN_00522180();
    iVar5 = FUN_0040a540();
    if ((iVar5 == 0) || (iVar5 == 1)) {
      *(undefined4 *)(param_1 + 0x24) = 0;
      *(undefined4 *)(param_1 + 0x28) = 1;
      DAT_0062bb10 = DAT_0062bb10 & 0xffffff40 | 0x40;
      DAT_0062bb14 = DAT_0062bb14 & 0xffffffcc | 0xc;
      DAT_0062bb18 = DAT_0062bb18 & 0xffffffcc | 0xc;
    }
    else if (iVar5 == 2) {
      *(undefined4 *)(param_1 + 0x24) = 0;
      *(undefined4 *)(param_1 + 0x28) = 0;
      iVar5 = FUN_00414960();
      if (iVar5 == 3) {
        *(undefined4 *)(param_1 + 0x61d8) = 1;
      }
      else {
        DAT_0062bb10 = DAT_0062be04;
        DAT_0062bb14 = DAT_0062be08;
        DAT_0062bb18 = DAT_0062be0c;
      }
    }
    pppppppuVar19 = &local_18;
    *(uint *******)(param_1 + 0x10c) = local_18;
    *(undefined4 *)(param_1 + 0x1d4) = 1;
    *(uint *******)(param_1 + 0x1e4) = local_18;
    *(undefined4 *)(param_1 + 0x2ac) = 1;
    local_14 = (uint *******)0x13;
    if (-1 < (int)DAT_0062b0cc) {
      pppppppuVar19 = &DAT_0062b0cc;
    }
    local_18 = (uint ******)0x0;
    pppppppuVar13 = (uint *******)&local_14;
    if ((int)DAT_0062b0cc < 0x14) {
      pppppppuVar13 = pppppppuVar19;
    }
    DAT_0062b0cc = *pppppppuVar13;
    FUN_0040a670(DAT_0062b0cc);
    pppppppuVar19 = &local_18;
    local_14 = (uint *******)0x3;
    local_18 = (uint ******)0x0;
    if (-1 < (int)DAT_0062b254) {
      pppppppuVar19 = &DAT_0062b254;
    }
    pppppppuVar13 = (uint *******)&local_14;
    if ((int)DAT_0062b254 < 4) {
      pppppppuVar13 = pppppppuVar19;
    }
    DAT_0062b254 = *pppppppuVar13;
    if ((DAT_0062b254 != (uint ******)0x0) && (DAT_0062b0d4 == 1)) {
      FUN_0040a670(0x13);
    }
    local_e0[0x1c] = 3;
    local_e0[0x1d] = 0;
    piVar21 = local_e0 + 0x1d;
    if (-1 < (int)DAT_0062b254) {
      piVar21 = (int *)&DAT_0062b254;
    }
    piVar7 = local_e0 + 0x1c;
    if ((int)DAT_0062b254 < 4) {
      piVar7 = piVar21;
    }
    DAT_0062b254 = (uint ******)*piVar7;
    if ((DAT_0062b254 == (uint ******)0x2) &&
       (cVar4 = FUN_0040a500(*(undefined4 *)(param_1 + 0x104)), cVar4 == '\0')) {
LAB_0055698a:
      FUN_0040a670(0);
    }
    else {
      local_e0[0x1a] = 3;
      local_e0[0x1b] = 0;
      piVar21 = local_e0 + 0x1b;
      if (-1 < (int)DAT_0062b254) {
        piVar21 = (int *)&DAT_0062b254;
      }
      piVar7 = local_e0 + 0x1a;
      if ((int)DAT_0062b254 < 4) {
        piVar7 = piVar21;
      }
      DAT_0062b254 = (uint ******)*piVar7;
      if ((DAT_0062b254 != (uint ******)0x2) &&
         (cVar4 = FUN_0040a4c0(*(undefined4 *)(param_1 + 0x104)), cVar4 == '\0')) goto LAB_0055698a;
    }
    pppppppuVar19 = &local_18;
    local_14 = (uint *******)0x13;
    local_18 = (uint ******)0x0;
    if (-1 < (int)DAT_0062b194) {
      pppppppuVar19 = &DAT_0062b194;
    }
    pppppppuVar13 = (uint *******)&local_14;
    if ((int)DAT_0062b194 < 0x14) {
      pppppppuVar13 = pppppppuVar19;
    }
    DAT_0062b194 = *pppppppuVar13;
    FUN_0040a670(DAT_0062b194);
    pppppppuVar19 = &local_18;
    local_14 = (uint *******)0x3;
    local_18 = (uint ******)0x0;
    if (-1 < (int)DAT_0062b254) {
      pppppppuVar19 = &DAT_0062b254;
    }
    pppppppuVar13 = (uint *******)&local_14;
    if ((int)DAT_0062b254 < 4) {
      pppppppuVar13 = pppppppuVar19;
    }
    DAT_0062b254 = *pppppppuVar13;
    if ((DAT_0062b254 != (uint ******)0x0) && (DAT_0062b19c == 1)) {
      FUN_0040a670(0x13);
    }
    local_e0[0x18] = 3;
    local_e0[0x19] = 0;
    piVar21 = local_e0 + 0x19;
    if (-1 < (int)DAT_0062b254) {
      piVar21 = (int *)&DAT_0062b254;
    }
    piVar7 = local_e0 + 0x18;
    if ((int)DAT_0062b254 < 4) {
      piVar7 = piVar21;
    }
    DAT_0062b254 = (uint ******)*piVar7;
    if ((DAT_0062b254 == (uint ******)0x2) &&
       (cVar4 = FUN_0040a500(*(undefined4 *)(param_1 + 0x1dc)), cVar4 == '\0')) {
LAB_00556ab1:
      FUN_0040a670(1);
      piVar21 = extraout_ECX_00;
    }
    else {
      local_e0[0x16] = 3;
      local_e0[0x17] = 0;
      piVar21 = local_e0 + 0x17;
      if (-1 < (int)DAT_0062b254) {
        piVar21 = (int *)&DAT_0062b254;
      }
      piVar7 = local_e0 + 0x16;
      if ((int)DAT_0062b254 < 4) {
        piVar7 = piVar21;
      }
      DAT_0062b254 = (uint ******)*piVar7;
      if ((DAT_0062b254 != (uint ******)0x2) &&
         (cVar4 = FUN_0040a4c0(*(undefined4 *)(param_1 + 0x1dc)), piVar21 = extraout_ECX,
         cVar4 == '\0')) goto LAB_00556ab1;
    }
    puVar18 = (undefined4 *)FUN_00405530(&local_2c,piVar21,local_24,0xffffffff,piVar21);
    iVar10 = 0;
    iVar5 = (uint)(ppppppuVar11 != (uint ******)0x0) * 4 + 0x758;
    *(undefined4 *)(iVar5 + param_1) = *puVar18;
    do {
      if (-1 < local_4c[iVar10]) {
        puVar18 = (undefined4 *)FUN_00405530(&local_2c,iVar5,local_4c[iVar10],0xffffffff,iVar5);
        iVar5 = local_4c[iVar10];
        *(undefined4 *)(param_1 + 0x628 + iVar5 * 4) = *puVar18;
      }
      iVar10 = iVar10 + 1;
    } while (iVar10 < 2);
    puVar18 = (undefined4 *)FUN_00405530(&local_2c,iVar5,0xd1,0xffffffff,iVar5);
    local_e0[0x14] = 3;
    local_24 = (uint *****)0x0;
    local_e0[0x15] = 0;
    *(undefined4 *)(param_1 + 0x96c) = *puVar18;
    local_14 = (uint *******)((uint)(local_28 != (uint ******)0x0) * 4 + 0x758 + param_1);
    iVar5 = DAT_005d1c00;
    do {
      piVar21 = local_e0 + 0x15;
      if (-1 < (int)DAT_0062b254) {
        piVar21 = (int *)&DAT_0062b254;
      }
      piVar7 = local_e0 + 0x14;
      if ((int)DAT_0062b254 < 4) {
        piVar7 = piVar21;
      }
      DAT_0062b254 = (uint ******)*piVar7;
      local_18 = DAT_0062b254;
      if (DAT_0062b254 == (uint ******)0x2) {
        cVar4 = FUN_00408130();
        if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
          DAT_0062cdf8 = 0xfffffffe;
        }
        if (*(char *)(iVar5 + 0x2eaf6 + (int)local_24) != '\0') goto LAB_00556bca;
LAB_00556c30:
        pppppuVar23 = local_24;
        local_18 = (uint ******)FUN_004caed0(*(undefined4 *)(&DAT_005a1e40 + (int)local_24 * 4),0);
        if (local_18[0x181] != (uint *****)0x0) {
          (**(code **)(&DAT_00596420 + (int)local_18[0x181] * 4))(local_18,2);
        }
        local_18[0x115] = (uint *****)0x2;
        FUN_004bd660();
        iVar5 = DAT_005d1c00;
      }
      else {
LAB_00556bca:
        local_e0[0x12] = 3;
        local_e0[0x13] = 0;
        piVar21 = local_e0 + 0x13;
        if (-1 < (int)local_18) {
          piVar21 = (int *)&DAT_0062b254;
        }
        piVar7 = local_e0 + 0x12;
        if ((int)local_18 < 4) {
          piVar7 = piVar21;
        }
        DAT_0062b254 = (uint ******)*piVar7;
        pppppuVar23 = local_24;
        if (DAT_0062b254 != (uint ******)0x2) {
          cVar4 = FUN_00408130();
          if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
            DAT_0062cdf8 = 0xfffffffe;
          }
          pppppuVar23 = local_24;
          if (*(char *)(iVar5 + 0x2ead6 + (int)local_24) == '\0') goto LAB_00556c30;
        }
      }
      param_1 = local_4c[2];
      local_24 = (uint *****)((int)pppppuVar23 + 1);
    } while ((int)pppppuVar23 + 1 < 0x13);
    FUN_00431620(2);
    FUN_0055bb60(8);
    ppppppuVar11 = local_28;
  case 2:
    if (*(int *)(param_1 + 0x618) == 0xf) {
      local_58 = 0;
      local_50 = (uint *****)0x0;
      local_18 = (uint ******)0x0;
      local_4c[2] = (uint)(ppppppuVar11 != (uint ******)0x0) * 4 + 0x758 + param_1;
      local_24 = (uint *****)(param_1 + 0x104);
      do {
        iVar5 = local_4c[(int)((int)local_18 + 3)];
        if (-1 < iVar5) {
          FUN_004cb960(&local_14,local_18,iVar5,0,0,0xffffffff,2,0);
          *(uint ********)(param_1 + 0x628 + iVar5 * 4) = local_14;
        }
        local_1c = (uint ******)(iVar5 + 0x18a);
        puVar18 = (undefined4 *)
                  FUN_004cafa0(&local_2c,*(undefined4 *)(&DAT_005a1e40 + (int)*local_24 * 4),
                               local_1c);
        FUN_004cb8b0(*puVar18);
        FUN_00404230(&local_58);
        ppppppuVar11 = local_1c;
        local_58 = CONCAT44(local_58._4_4_ - DAT_005aa2f0,(undefined4)local_58);
        if (-1 < iVar5) {
          iVar5 = FUN_004cb8b0(*(undefined4 *)(param_1 + (int)local_1c * 4));
          if (iVar5 == 0) {
            *(undefined4 *)(param_1 + (int)ppppppuVar11 * 4) = 0;
          }
          *(undefined8 *)(iVar5 + 0x30) = local_58;
          *(uint ******)(iVar5 + 0x38) = local_50;
        }
        local_18 = (uint ******)((int)local_18 + 1);
        local_24 = local_24 + 0x36;
      } while ((int)local_18 < 2);
    }
    else if (0x1e < *(int *)(param_1 + 0x618)) {
      FUN_00431620(3);
      local_24 = (uint *****)(param_1 + 0x104);
      iVar5 = 0;
      do {
        if (-1 < local_4c[iVar5]) {
          FUN_004cb7a0(*(undefined4 *)(param_1 + 0x628 + local_4c[iVar5] * 4),
                       (int)(short)(*(short *)local_24 + 7));
          FUN_004cb7a0(*(undefined4 *)((uint)(local_28 != (uint ******)0x0) * 4 + 0x758 + param_1),
                       (int)(short)(*(short *)local_24 + 7));
        }
        iVar5 = iVar5 + 1;
        local_24 = local_24 + 0x36;
      } while (iVar5 < 2);
      pppppppuVar19 = &local_18;
      local_14 = (uint *******)0x3;
      local_18 = (uint ******)0x0;
      if (-1 < (int)DAT_0062b254) {
        pppppppuVar19 = &DAT_0062b254;
      }
      pppppppuVar13 = (uint *******)&local_14;
      if ((int)DAT_0062b254 < 4) {
        pppppppuVar13 = pppppppuVar19;
      }
      DAT_0062b254 = *pppppppuVar13;
      if (DAT_0062b254 == (uint ******)0x0) {
        FUN_004cb830(*(undefined4 *)(param_1 + 0x76c),1);
        iVar5 = DAT_005d1c00;
        local_18 = *(uint *******)(param_1 + 0x104);
        *(undefined4 *)(param_1 + 0x76c) = 0;
        cVar4 = FUN_00408130();
        if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
          DAT_0062cdf8 = 0xfffffffe;
        }
        if (*(char *)((int)local_18 + DAT_005d1c00 + 0x2eb16) != '\0') {
          puVar18 = (undefined4 *)FUN_00405530(&local_2c,local_18,0x51,0xffffffff,local_18);
          *(undefined4 *)(param_1 + 0x76c) = *puVar18;
        }
      }
      piVar21 = local_e0 + 0x11;
      local_e0[0x10] = 3;
      local_e0[0x11] = 0;
      if (-1 < (int)DAT_0062b254) {
        piVar21 = (int *)&DAT_0062b254;
      }
      piVar7 = local_e0 + 0x10;
      if ((int)DAT_0062b254 < 4) {
        piVar7 = piVar21;
      }
      DAT_0062b254 = (uint ******)*piVar7;
      if (DAT_0062b254 == (uint ******)0x2) {
        iVar5 = 0;
        local_58 = 0x43c8000043a00000;
        local_50 = (uint *****)0x0;
        do {
          FUN_004e37a0((DAT_0062bb10 >> 6 & 3) + 3);
          iVar10 = DAT_005d1a30;
          iVar5 = iVar5 + 1;
        } while (iVar5 < 2);
        FUN_004cb960(&local_18,extraout_ECX_01,0xc,0,0,0xffffffff,4,0);
        *(uint *******)(iVar10 + 0x5a00) = local_18;
        FUN_004cb960(&local_18,extraout_ECX_02,0xd,0,0,0xffffffff,4,0);
        puVar18 = (undefined4 *)(param_1 + 0x104);
        *(uint *******)(iVar10 + 0x5a04) = local_18;
        iVar5 = 0;
        do {
          FUN_004dfce0(*puVar18);
          iVar3 = DAT_005d19b0;
          iVar10 = *(int *)((int)&DAT_005d1a28 + iVar5);
          puVar18 = puVar18 + 0x36;
          iVar1 = *(int *)((int)&local_34 + iVar5);
          iVar5 = iVar5 + 4;
          *(undefined4 *)(iVar10 + 0x17cc) = 1;
          *(undefined4 *)(iVar10 + 0x17c8) = *(undefined4 *)(iVar3 + 0x2e30 + iVar1 * 4);
        } while (iVar5 < 8);
      }
    }
    break;
  case 3:
    iVar5 = FUN_0055a7c0();
    if (iVar5 == 0) {
      iVar5 = FUN_0040a540();
      if ((((iVar5 == 2) && (iVar5 = FUN_00414960(), iVar5 != 0)) &&
          (iVar5 = FUN_00414960(), iVar5 != 3)) &&
         ((uVar8 = DAT_0062bb10 >> 6 & 3, uVar8 != 0 && (uVar8 != 1)))) {
        if (*(int *)(param_1 + 0x24) == 0) {
          *(undefined4 *)(DAT_005d1a28 + 0x17cc) = 1;
          *(undefined4 *)(DAT_005d1a2c + 0x17cc) = 0;
        }
        else {
          *(undefined4 *)(DAT_005d1a28 + 0x17cc) = 0;
          *(undefined4 *)(DAT_005d1a2c + 0x17cc) = 1;
        }
      }
      iVar5 = 0;
      local_e0[0xe] = 3;
      local_e0[0xf] = 0;
      do {
        piVar21 = local_e0 + 0xf;
        local_18 = *(uint *******)(DAT_005d19b0 + 0x2e30 + *(int *)((int)&local_34 + iVar5 * 4) * 4)
        ;
        if (-1 < (int)DAT_0062b254) {
          piVar21 = (int *)&DAT_0062b254;
        }
        piVar7 = local_e0 + 0xe;
        if ((int)DAT_0062b254 < 4) {
          piVar7 = piVar21;
        }
        DAT_0062b254 = (uint ******)*piVar7;
        if (DAT_0062b254 == (uint ******)0x2) {
          piVar21 = local_e0 + 0xd;
          local_e0[0xc] = 4;
          local_e0[0xd] = 0;
          if (-1 < (int)DAT_0062b258) {
            piVar21 = (int *)&DAT_0062b258;
          }
          piVar7 = local_e0 + 0xc;
          if ((int)DAT_0062b258 < 5) {
            piVar7 = piVar21;
          }
          DAT_0062b258 = (uint *******)*piVar7;
          if (DAT_0062b258 == (uint *******)0x0) goto LAB_00557193;
          local_e0[10] = 4;
          piVar21 = local_e0 + 0xb;
          local_e0[0xb] = 0;
          if (-1 < (int)DAT_0062b258) {
            piVar21 = (int *)&DAT_0062b258;
          }
          piVar7 = local_e0 + 10;
          if ((int)DAT_0062b258 < 5) {
            piVar7 = piVar21;
          }
          DAT_0062b258 = (uint *******)*piVar7;
          if (DAT_0062b258 == (uint *******)0x3) goto LAB_00557193;
          if (iVar5 != 0) {
            if (iVar5 == 1) {
              bVar27 = *(int *)(param_1 + 0x24) == 0;
              goto LAB_0055718d;
            }
            goto LAB_00557193;
          }
          if (*(int *)(param_1 + 0x24) == 0) goto LAB_00557193;
        }
        else {
          bVar27 = iVar5 == 1;
LAB_0055718d:
          if (bVar27) break;
LAB_00557193:
          if ((*(int *)(param_1 + 0x24 + iVar5 * 4) == 0) &&
             (*(undefined4 *)(iVar5 * 0xd8 + 0x108 + param_1) =
                   *(undefined4 *)(iVar5 * 0xd8 + 0x104 + param_1), -1 < (int)local_18)) {
            if ((&DAT_005d19a0)[(int)local_18] == (uint *)0x0) {
LAB_0055720d:
              iVar10 = (int)(&DAT_005d19a0)[(int)local_18];
              if (iVar10 == 0) {
                if ((int)local_18 < 0) goto LAB_005572e1;
              }
              else {
                if (((*(byte *)(iVar10 + 0x10) & 0x20) == 0) &&
                   ((*(byte *)(iVar10 + 8) & 0x20) == 0)) {
                  bVar27 = false;
                }
                else {
                  bVar27 = true;
                }
                if (bVar27) {
                  pppppuVar23 = local_1c[*(int *)(iVar5 * 0xd8 + 0x104 + param_1) * 4 + 1];
                  goto LAB_005572ce;
                }
              }
              iVar10 = (int)(&DAT_005d19a0)[(int)local_18];
              if (iVar10 == 0) {
                if ((int)local_18 < 0) goto LAB_005572e1;
              }
              else {
                if (((*(byte *)(iVar10 + 0x10) & 0x40) == 0) &&
                   ((*(byte *)(iVar10 + 8) & 0x40) == 0)) {
                  bVar27 = false;
                }
                else {
                  bVar27 = true;
                }
                if (bVar27) {
                  pppppuVar23 = local_1c[*(int *)(iVar5 * 0xd8 + 0x104 + param_1) * 4 + 2];
                  goto LAB_005572ce;
                }
              }
              iVar10 = (int)(&DAT_005d19a0)[(int)local_18];
              if (iVar10 != 0) {
                if (((*(byte *)(iVar10 + 0x10) & 0x80) == 0) &&
                   ((*(byte *)(iVar10 + 8) & 0x80) == 0)) {
                  bVar27 = false;
                }
                else {
                  bVar27 = true;
                }
                if (bVar27) {
                  pppppuVar23 = local_1c[*(int *)(iVar5 * 0xd8 + 0x104 + param_1) * 4 + 3];
                  goto LAB_005572ce;
                }
              }
            }
            else if ((*(&DAT_005d19a0)[(int)local_18] & 8) == 0) {
              if (((*(byte *)((int)(&DAT_005d19a0)[(int)local_18] + 0x10) & 0x10) == 0) &&
                 ((*(byte *)((int)(&DAT_005d19a0)[(int)local_18] + 8) & 0x10) == 0)) {
                bVar27 = false;
              }
              else {
                bVar27 = true;
              }
              if (!bVar27) goto LAB_0055720d;
              pppppuVar23 = local_1c[*(int *)(iVar5 * 0xd8 + 0x104 + param_1) * 4];
LAB_005572ce:
              FUN_0040a670(pppppuVar23);
            }
          }
        }
LAB_005572e1:
        iVar5 = iVar5 + 1;
      } while (iVar5 < 2);
      local_e0[8] = 3;
      local_20 = (uint ******)0x0;
      local_e0[9] = 0;
      do {
        ppppppuVar11 = local_20;
        piVar21 = local_e0 + 9;
        local_18 = *(uint *******)
                    (DAT_005d19b0 + 0x2e30 + *(int *)((int)&local_34 + (int)local_20 * 4) * 4);
        if (-1 < (int)DAT_0062b254) {
          piVar21 = (int *)&DAT_0062b254;
        }
        piVar7 = local_e0 + 8;
        if ((int)DAT_0062b254 < 4) {
          piVar7 = piVar21;
        }
        DAT_0062b254 = (uint ******)*piVar7;
        if (DAT_0062b254 == (uint ******)0x2) {
          piVar21 = local_e0 + 7;
          local_e0[6] = 4;
          local_e0[7] = 0;
          if (-1 < (int)DAT_0062b258) {
            piVar21 = (int *)&DAT_0062b258;
          }
          piVar7 = local_e0 + 6;
          if ((int)DAT_0062b258 < 5) {
            piVar7 = piVar21;
          }
          DAT_0062b258 = (uint *******)*piVar7;
          if (DAT_0062b258 == (uint *******)0x0) goto LAB_005573e3;
          local_e0[4] = 4;
          piVar21 = local_e0 + 5;
          local_e0[5] = 0;
          if (-1 < (int)DAT_0062b258) {
            piVar21 = (int *)&DAT_0062b258;
          }
          piVar7 = local_e0 + 4;
          if ((int)DAT_0062b258 < 5) {
            piVar7 = piVar21;
          }
          DAT_0062b258 = (uint *******)*piVar7;
          if (DAT_0062b258 == (uint *******)0x3) goto LAB_005573e3;
          if (local_20 != (uint ******)0x0) {
            if (local_20 == (uint ******)0x1) {
              bVar27 = *(int *)(param_1 + 0x24) == 0;
              goto LAB_005573dd;
            }
            goto LAB_005573e3;
          }
          if (*(int *)(param_1 + 0x24) == 0) goto LAB_005573e3;
        }
        else {
          bVar27 = local_20 == (uint ******)0x1;
LAB_005573dd:
          if (bVar27) break;
LAB_005573e3:
          if (*(int *)(param_1 + 0x24 + (int)local_20 * 4) == 0) {
            iVar5 = (int)local_20 * 0xd8;
            if (*(int *)(iVar5 + 0x108 + param_1) != *(int *)(iVar5 + 0x104 + param_1)) {
              FUN_004bb8d0(10,0);
              if ((&DAT_005d1a28)[(int)local_20] != 0) {
                FUN_004dfce0(*(undefined4 *)(iVar5 + 0x104 + param_1));
              }
              if ((local_4c[1 - (int)local_20] < 0) ||
                 (*(int *)(iVar5 + 0x108 + param_1) !=
                  *(int *)(param_1 + (int)ppppppuVar11 * -0xd8 + 0x1dc))) {
                FUN_004cb7a0(*(undefined4 *)
                              ((uint)(local_28 != (uint ******)0x0) * 4 + 0x758 + param_1),
                             (int)(short)(*(short *)(iVar5 + 0x108 + param_1) + 0x25));
              }
              FUN_004cb830(*(undefined4 *)
                            ((uint)(local_28 != (uint ******)0x0) * 4 + 0x758 + param_1),
                           (int)(short)(*(short *)(iVar5 + 0x104 + param_1) + 7));
              ppppppuVar11 = local_20;
              if (-1 < local_4c[(int)local_20]) {
                FUN_004cb7a0(*(undefined4 *)(param_1 + 0x628 + local_4c[(int)local_20] * 4),3);
              }
              iVar5 = DAT_005d1c00;
              piVar21 = local_e0 + 3;
              local_e0[2] = 3;
              local_e0[3] = 0;
              if (-1 < (int)DAT_0062b254) {
                piVar21 = (int *)&DAT_0062b254;
              }
              piVar7 = local_e0 + 2;
              if ((int)DAT_0062b254 < 4) {
                piVar7 = piVar21;
              }
              local_1c = (uint ******)*piVar7;
              if (local_1c == (uint ******)0x2) {
                local_14 = *(uint ********)((int)ppppppuVar11 * 0xd8 + 0x104 + param_1);
                DAT_0062b254 = local_1c;
                cVar4 = FUN_00408130();
                if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
                  DAT_0062cdf8 = 0xfffffffe;
                }
                if (*(char *)(DAT_005d1c00 + 0x2eaf6 + (int)local_14) == '\0') goto LAB_00557551;
LAB_005575d1:
                ppppppuVar25 = local_20;
                ppppppuVar11 = local_1c;
                if (-1 < local_4c[(int)local_20]) {
                  FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + local_4c[(int)local_20] * 4),
                               (int)(short)(*(short *)((int)local_20 * 0xd8 + 0x104 + param_1) + 7))
                  ;
                  ppppppuVar11 = DAT_0062b254;
                }
              }
              else {
LAB_00557551:
                iVar5 = DAT_005d1c00;
                local_e0[0] = 3;
                local_e0[1] = 0;
                piVar21 = local_e0 + 1;
                if (-1 < (int)local_1c) {
                  piVar21 = (int *)&DAT_0062b254;
                }
                piVar7 = local_e0;
                if ((int)local_1c < 4) {
                  piVar7 = piVar21;
                }
                ppppppuVar11 = (uint ******)*piVar7;
                ppppppuVar25 = local_20;
                if (ppppppuVar11 != (uint ******)0x2) {
                  local_14 = *(uint ********)((int)local_20 * 0xd8 + 0x104 + param_1);
                  DAT_0062b254 = ppppppuVar11;
                  local_1c = ppppppuVar11;
                  cVar4 = FUN_00408130();
                  if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
                    DAT_0062cdf8 = 0xfffffffe;
                  }
                  ppppppuVar11 = local_1c;
                  ppppppuVar25 = local_20;
                  if (*(char *)((int)local_14 + DAT_005d1c00 + 0x2ead6) != '\0') goto LAB_005575d1;
                }
              }
              local_1c = (uint ******)0x3;
              local_14 = (uint *******)0x0;
              pppppppuVar19 = (uint *******)&local_14;
              if (-1 < (int)ppppppuVar11) {
                pppppppuVar19 = &DAT_0062b254;
              }
              pppppppuVar13 = &local_1c;
              if ((int)ppppppuVar11 < 4) {
                pppppppuVar13 = pppppppuVar19;
              }
              DAT_0062b254 = *pppppppuVar13;
              if (DAT_0062b254 == (uint ******)0x0) {
                FUN_004cb830(*(undefined4 *)(param_1 + 0x76c),1);
                iVar5 = DAT_005d1c00;
                *(undefined4 *)(param_1 + 0x76c) = 0;
                local_14 = *(uint ********)((int)ppppppuVar25 * 0xd8 + 0x104 + param_1);
                cVar4 = FUN_00408130();
                if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
                  DAT_0062cdf8 = 0xfffffffe;
                }
                pppppppuVar19 = local_14;
                if (*(char *)(DAT_005d1c00 + 0x2eb16 + (int)local_14) != '\0') {
                  puVar18 = (undefined4 *)FUN_00405530(&local_2c,local_14,0x51,0xffffffff,local_14);
                  *(undefined4 *)(param_1 + 0x76c) = *puVar18;
                  pppppppuVar19 = extraout_ECX_03;
                }
              }
              local_64 = 0.0;
              local_60 = 0.0;
              local_5c = 0;
              puVar9 = (uint *)FUN_004cafa0(local_4c + 5,
                                            *(undefined4 *)
                                             (&DAT_005a1e40 +
                                             *(int *)((int)local_20 * 0xd8 + 0x104 + param_1) * 4),
                                            pppppppuVar19);
              uVar8 = *puVar9;
              if ((uVar8 != 0) && ((uVar8 & 0xffff) == 0xffff)) {
                puVar18 = (undefined4 *)(DAT_005d19bc + 0x71c);
                local_1c = (uint ******)0x0;
                do {
                  for (piVar21 = (int *)puVar18[-2]; piVar21 != (int *)0x0;
                      piVar21 = (int *)piVar21[1]) {
                    if (*(uint *)(*piVar21 + 0x4d8) == uVar8) goto LAB_005577a3;
                  }
                  for (piVar21 = (int *)*puVar18; piVar21 != (int *)0x0; piVar21 = (int *)piVar21[1]
                      ) {
                    if (*(uint *)(*piVar21 + 0x4d8) == uVar8) goto LAB_005577a3;
                  }
                  local_1c = (uint ******)((int)local_1c + 1);
                  puVar18 = puVar18 + 8;
                } while ((int)local_1c < 2);
              }
LAB_005577a3:
              FUN_00404230(&local_64);
              local_60 = local_60 - DAT_005aa2d4;
              iVar5 = local_4c[(int)local_20 + 3];
              fVar28 = DAT_005aa2d4;
              if (iVar5 < 0) {
                iVar10 = 0;
              }
              else {
                iVar10 = FUN_004cb8b0(*(undefined4 *)(param_1 + 0x628 + iVar5 * 4));
                if (iVar10 == 0) {
                  *(undefined4 *)(param_1 + 0x628 + iVar5 * 4) = 0;
                }
              }
              local_50 = *(uint ******)(iVar10 + 0x38);
              local_58 = *(undefined8 *)(iVar10 + 0x30);
              iVar5 = param_1 + (int)local_20 * -0xd8;
              if (*(int *)((int)local_20 * 0xd8 + 0x104 + param_1) == *(int *)(iVar5 + 0x1dc)) {
                if (local_20 == (uint ******)0x0) {
                  fVar28 = DAT_005aa4e0;
                }
                local_64 = local_64 + fVar28;
              }
              FUN_0040e160(8,4,&local_58,&local_64);
              iVar5 = *(int *)(iVar5 + 0x1dc);
              ppppppuVar11 = local_20;
              if (*(int *)((int)local_20 * 0xd8 + 0x104 + param_1) == iVar5) {
                iVar5 = local_4c[4 - (int)local_20];
                if (-1 < iVar5) {
                  ppppppuVar11 = (uint ******)
                                 FUN_004cb8b0(*(undefined4 *)(param_1 + 0x628 + iVar5 * 4));
                  local_1c = ppppppuVar11;
                  if (ppppppuVar11 == (uint ******)0x0) {
                    *(undefined4 *)(param_1 + 0x628 + iVar5 * 4) = 0;
                    ppppppuVar11 = local_20;
                  }
                  else {
                    puVar9 = (uint *)FUN_004cafa0(local_130,
                                                  *(undefined4 *)
                                                   (&DAT_005a1e40 +
                                                   *(int *)((int)local_20 * 0xd8 + 0x104 + param_1)
                                                   * 4),extraout_ECX_04);
                    uVar8 = *puVar9;
                    ppppppuVar25 = local_1c;
                    if ((uVar8 != 0) && (ppppppuVar25 = ppppppuVar11, (uVar8 & 0xffff) == 0xffff)) {
                      puVar18 = (undefined4 *)(DAT_005d19bc + 0x71c);
                      local_14 = (uint *******)0x0;
                      do {
                        for (piVar21 = (int *)puVar18[-2]; piVar21 != (int *)0x0;
                            piVar21 = (int *)piVar21[1]) {
                          ppppppuVar25 = local_1c;
                          if (*(uint *)(*piVar21 + 0x4d8) == uVar8) goto LAB_0055798c;
                        }
                        for (piVar21 = (int *)*puVar18; piVar21 != (int *)0x0;
                            piVar21 = (int *)piVar21[1]) {
                          ppppppuVar25 = local_1c;
                          if (*(uint *)(*piVar21 + 0x4d8) == uVar8) goto LAB_0055798c;
                        }
                        local_14 = (uint *******)((int)local_14 + 1);
                        puVar18 = puVar18 + 8;
                        ppppppuVar25 = local_1c;
                      } while ((int)local_14 < 2);
                    }
LAB_0055798c:
                    local_1c = ppppppuVar25;
                    FUN_00404230(&local_64);
                    ppppppuVar11 = local_20;
                    local_60 = local_60 - DAT_005aa2d4;
                    local_50 = local_1c[0xe];
                    local_58 = *(undefined8 *)(local_1c + 0xc);
                    fVar28 = DAT_005aa2d4;
                    if (local_20 != (uint ******)0x0) {
                      fVar28 = DAT_005aa4e0;
                    }
                    local_64 = local_64 + fVar28;
                    FUN_0040e160(8,4,&local_58,&local_64);
                  }
                }
              }
              else if (*(int *)((int)local_20 * 0xd8 + 0x108 + param_1) == iVar5) {
                puVar9 = (uint *)FUN_004cafa0(local_114,*(undefined4 *)(&DAT_005a1e40 + iVar5 * 4),
                                              iVar5);
                uVar8 = *puVar9;
                if ((uVar8 != 0) && ((uVar8 & 0xffff) == 0xffff)) {
                  puVar18 = (undefined4 *)(DAT_005d19bc + 0x71c);
                  local_14 = (uint *******)0x0;
                  do {
                    for (piVar21 = (int *)puVar18[-2]; piVar21 != (int *)0x0;
                        piVar21 = (int *)piVar21[1]) {
                      if (*(uint *)(*piVar21 + 0x4d8) == uVar8) goto LAB_00557ad3;
                    }
                    for (piVar21 = (int *)*puVar18; piVar21 != (int *)0x0;
                        piVar21 = (int *)piVar21[1]) {
                      if (*(uint *)(*piVar21 + 0x4d8) == uVar8) goto LAB_00557ad3;
                    }
                    local_14 = (uint *******)((int)local_14 + 1);
                    puVar18 = puVar18 + 8;
                  } while ((int)local_14 < 2);
                }
LAB_00557ad3:
                FUN_00404230(&local_64);
                local_60 = local_60 - DAT_005aa2d4;
                iVar5 = local_4c[4 - (int)local_20];
                ppppppuVar11 = local_20;
                if (-1 < iVar5) {
                  iVar10 = FUN_004cb8b0(*(undefined4 *)(param_1 + 0x628 + iVar5 * 4));
                  if (iVar10 == 0) {
                    *(undefined4 *)(param_1 + 0x628 + iVar5 * 4) = 0;
                    ppppppuVar11 = local_20;
                  }
                  else {
                    local_50 = *(uint ******)(iVar10 + 0x38);
                    local_58 = *(undefined8 *)(iVar10 + 0x30);
                    FUN_0040e160(8,4,&local_58,&local_64);
                    ppppppuVar11 = local_20;
                  }
                }
              }
            }
            iVar5 = DAT_005d1c00;
            if (-1 < (int)local_18) {
              if (((&DAT_005d19a0)[(int)local_18] != (undefined4 *)0x0) &&
                 ((*(uint *)((int)(&DAT_005d19a0)[(int)local_18] + 0x10) & 0x80001) != 0)) {
                puVar18 = &local_e4;
                local_e8 = 3;
                local_e4 = 0;
                if (-1 < (int)DAT_0062b254) {
                  puVar18 = &DAT_0062b254;
                }
                puVar12 = &local_e8;
                if ((int)DAT_0062b254 < 4) {
                  puVar12 = puVar18;
                }
                DAT_0062b254 = (uint ******)*puVar12;
                local_1c = DAT_0062b254;
                if (DAT_0062b254 == (uint ******)0x2) {
                  local_14 = *(uint ********)((int)local_20 * 0xd8 + 0x104 + param_1);
                  cVar4 = FUN_00408130();
                  if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
                    DAT_0062cdf8 = 0xfffffffe;
                  }
                  if (*(char *)(DAT_005d1c00 + 0x2eaf6 + (int)local_14) == '\0') goto LAB_00557c04;
LAB_00557c8a:
                  ppppppuVar11 = local_20;
                  if (-1 < local_4c[(int)local_20 + 3]) {
                    FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + local_4c[(int)local_20 + 3] * 4),
                                 6);
                  }
                  *(undefined4 *)(param_1 + 0x24 + (int)ppppppuVar11 * 4) = 1;
                  FUN_004bb8d0(7,0);
                  if (*(int *)(param_1 + (10 - (int)ppppppuVar11) * 4) != 0) {
                    FUN_00431620(4);
                    DAT_0062b0d4 = 0;
                    ppppppuVar11 = (uint ******)(param_1 + 0x104);
                    DAT_0062b19c = 0;
                    local_18 = (uint ******)&DAT_0062b0d4;
                    local_1c = (uint ******)0x0;
                    do {
                      local_28 = ppppppuVar11;
                      if (*ppppppuVar11 == (uint *****)0x13) {
                        do {
                          uVar8 = FUN_00402540();
                          FUN_0040a670(uVar8 % 0x13);
                          iVar5 = DAT_005d1c00;
                          local_14 = (uint *******)*ppppppuVar11;
                          cVar4 = FUN_00408130();
                          if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
                            DAT_0062cdf8 = 0xfffffffe;
                          }
                        } while (*(char *)((int)local_14 + DAT_005d1c00 + 0x2eaf6) == '\0');
                        FUN_004bb8d0(10,0);
                        ppppppuVar11 = local_28;
                        param_1 = local_4c[2];
                        if (local_1c[0x17468a] != (uint *****)0x0) {
                          FUN_004dfce0(*local_28);
                        }
                        if (-1 < *(int *)((int)local_4c + (int)local_1c)) {
                          FUN_004cb7a0(*(undefined4 *)
                                        (param_1 + 0x628 +
                                        *(int *)((int)local_4c + (int)local_1c) * 4),3);
                          FUN_004cb830(*(undefined4 *)
                                        (param_1 +
                                        (*(int *)((int)local_4c + (int)local_1c) + 0x18a) * 4),
                                       (int)(short)(*(short *)ppppppuVar11 + 7));
                        }
                        *local_18 = (uint *****)0x1;
                        if ((*(uint *)(param_1 + 0x624) & 1) == 0) {
                          *(undefined4 *)(param_1 + 0x618) = 0;
                          *(undefined4 *)(param_1 + 0x61c) = 0;
                          *(undefined4 *)(param_1 + 0x620) = 0;
                          *(uint *)(param_1 + 0x624) = *(uint *)(param_1 + 0x624) | 1;
                        }
                        *(undefined4 *)(param_1 + 0x618) = 0xffffffc4;
                        *(undefined4 *)(param_1 + 0x61c) = 0xc2700000;
                        *(undefined4 *)(param_1 + 0x614) = 0xffffffc3;
                      }
                      local_18 = local_18 + 0x32;
                      ppppppuVar11 = ppppppuVar11 + 0x36;
                      local_1c = local_1c + 1;
                      local_28 = ppppppuVar11;
                    } while ((int)local_18 < 0x62b264);
                    piVar21 = local_108 + 3;
                    local_108[2] = 3;
                    local_108[3] = 0;
                    if (-1 < (int)DAT_0062b254) {
                      piVar21 = (int *)&DAT_0062b254;
                    }
                    piVar7 = local_108 + 2;
                    if ((int)DAT_0062b254 < 4) {
                      piVar7 = piVar21;
                    }
                    DAT_0062b254 = (uint ******)*piVar7;
                    if (DAT_0062b254 == (uint ******)0x2) {
                      local_34 = 0;
                      FUN_004de2f0(0,0);
                      FUN_004de2f0(0,0);
                      uVar8 = DAT_0062bb10 >> 6 & 3;
                      if (uVar8 == 1) {
                        local_38 = (uint *****)0xc8;
                        ppppppuVar11 = (uint ******)&local_34;
                        local_2c = (uint ***)0x0;
                        local_1c = (uint ******)0xc8;
                        local_24 = (uint *****)0x0;
                        local_14 = (uint *******)&DAT_005d1a8c;
                        do {
                          pppppppuVar19 = local_14;
                          local_18 = ppppppuVar11;
                          uVar8 = FUN_00402540();
                          pppppuVar23 = (uint *****)(uVar8 % 0x27);
                          *ppppppuVar11 = pppppuVar23;
                          ppppppuVar25 = *pppppppuVar19 + 0x25;
                          ppppppuVar11 = (uint ******)(local_4c + 5);
                          if (pppppuVar23 < (uint *****)0xc9) {
                            ppppppuVar11 = ppppppuVar25;
                          }
                          *ppppppuVar25 = pppppuVar23;
                          *ppppppuVar25 = *ppppppuVar11;
                          ppppppuVar11 = pppppppuVar19[-3];
                          ppppppuVar14 = *pppppppuVar19 + 0x25;
                          ppppppuVar25 = &local_24;
                          if (-1 < (int)*ppppppuVar14) {
                            ppppppuVar25 = ppppppuVar14;
                          }
                          ppppppuVar16 = (uint ******)&local_1c;
                          if ((int)*ppppppuVar14 < 0xc9) {
                            ppppppuVar16 = ppppppuVar25;
                          }
                          pppppuVar23 = *ppppppuVar16;
                          *ppppppuVar14 = pppppuVar23;
                          FUN_004deba0(pppppuVar23,ppppppuVar11);
                          local_14 = local_14 + 0xf;
                          ppppppuVar11 = local_18 + 1;
                          param_1 = local_4c[2];
                          local_18 = ppppppuVar11;
                        } while ((int)local_14 < 0x5d1b04);
                      }
                      else if (uVar8 == 2) {
                        local_2c = (uint ***)0xc8;
                        local_18 = (uint ******)&local_34;
                        local_38 = (uint *****)0x0;
                        ppppppuVar11 = (uint ******)(param_1 + 0x104);
                        local_24 = (uint *****)&DAT_005d1a8c;
                        local_14 = (uint *******)&DAT_0062b0d4;
                        do {
                          pppppuVar23 = local_24;
                          local_1c = ppppppuVar11;
                          uVar8 = FUN_00402540();
                          *local_18 = (uint *****)(uVar8 & 1);
                          if (*local_14 != (uint ******)0x0) {
                            ppppppuVar25 = (uint ******)(*pppppuVar23 + 0x25);
                            local_20 = (uint ******)0xc8;
                            local_28 = (uint ******)0x0;
                            pppppuVar23 = (uint *****)
                                          ((int)*ppppppuVar11 * 2 + 1 + (int)(uVar8 & 1));
                            *ppppppuVar25 = pppppuVar23;
                            ppppppuVar11 = (uint ******)&local_28;
                            if (-1 < (int)pppppuVar23) {
                              ppppppuVar11 = ppppppuVar25;
                            }
                            ppppppuVar14 = (uint ******)&local_20;
                            if ((int)pppppuVar23 < 0xc9) {
                              ppppppuVar14 = ppppppuVar11;
                            }
                            *ppppppuVar25 = *ppppppuVar14;
                            pppppuVar23 = local_24;
                          }
                          ppppuVar2 = pppppuVar23[-3];
                          ppppuVar26 = *pppppuVar23 + 0x25;
                          ppppuVar24 = (uint ****)(local_4c + 5);
                          if (-1 < (int)*ppppuVar26) {
                            ppppuVar24 = ppppuVar26;
                          }
                          ppppuVar15 = &local_2c;
                          if ((int)*ppppuVar26 < 0xc9) {
                            ppppuVar15 = ppppuVar24;
                          }
                          pppuVar20 = *ppppuVar15;
                          *ppppuVar26 = pppuVar20;
                          FUN_004deba0(pppuVar20,ppppuVar2);
                          local_14 = local_14 + 0x32;
                          ppppppuVar11 = local_1c + 0x36;
                          local_18 = local_18 + 1;
                          local_24 = local_24 + 0xf;
                          param_1 = local_4c[2];
                          local_1c = ppppppuVar11;
                        } while ((int)local_14 < 0x62b264);
                      }
                      else if ((uint *****)(uVar8 - 3) == (uint *****)0x0) {
                        local_1c = (uint ******)0xc8;
                        local_24 = (uint *****)(uVar8 - 3);
                        local_14 = (uint *******)&DAT_005d1a8c;
                        local_18 = (uint ******)0x0;
                        do {
                          pppppppuVar19 = local_14;
                          ppppppuVar14 = *local_14 + 0x25;
                          *(uint *****)((int)&local_34 + (int)local_18) = local_18[0x17468a][0x70];
                          ppppppuVar11 = pppppppuVar19[-3];
                          ppppppuVar25 = &local_24;
                          if (-1 < (int)*ppppppuVar14) {
                            ppppppuVar25 = ppppppuVar14;
                          }
                          ppppppuVar16 = (uint ******)&local_1c;
                          if ((int)*ppppppuVar14 < 0xc9) {
                            ppppppuVar16 = ppppppuVar25;
                          }
                          pppppuVar23 = *ppppppuVar16;
                          *ppppppuVar14 = pppppuVar23;
                          FUN_004deba0(pppppuVar23,ppppppuVar11);
                          local_14 = local_14 + 0xf;
                          local_18 = local_18 + 1;
                          param_1 = local_4c[2];
                        } while ((int)local_14 < 0x5d1b04);
                      }
                      FUN_004df950(*(undefined4 *)(param_1 + 0x104),(uint *****)local_34);
                      FUN_004df950(*(undefined4 *)(param_1 + 0x1dc),local_34._4_4_);
                      *(undefined4 *)(DAT_005d1a28 + 0x17cc) = 0;
                      *(undefined4 *)(DAT_005d1a2c + 0x17cc) = 0;
                    }
                    break;
                  }
                  piVar21 = local_108 + 5;
                  local_108[4] = 3;
                  local_108[5] = 0;
                  if (-1 < (int)DAT_0062b254) {
                    piVar21 = (int *)&DAT_0062b254;
                  }
                  piVar7 = local_108 + 4;
                  if ((int)DAT_0062b254 < 4) {
                    piVar7 = piVar21;
                  }
                  DAT_0062b254 = (uint ******)*piVar7;
                  if (DAT_0062b254 != (uint ******)0x2) break;
                  pppppppuVar19 = (uint *******)&local_14;
                  local_1c = (uint ******)0x4;
                  local_14 = (uint *******)0x0;
                  if (-1 < (int)DAT_0062b258) {
                    pppppppuVar19 = (uint *******)&DAT_0062b258;
                  }
                  pppppppuVar13 = &local_1c;
                  if ((int)DAT_0062b258 < 5) {
                    pppppppuVar13 = pppppppuVar19;
                  }
                  DAT_0062b258 = (uint *******)*pppppppuVar13;
                  if (DAT_0062b258 != (uint *******)0x0) break;
                }
                else {
LAB_00557c04:
                  iVar5 = DAT_005d1c00;
                  local_110 = 3;
                  local_ec = 0;
                  puVar18 = &local_ec;
                  if (-1 < (int)local_1c) {
                    puVar18 = &DAT_0062b254;
                  }
                  puVar12 = &local_110;
                  if ((int)local_1c < 4) {
                    puVar12 = puVar18;
                  }
                  DAT_0062b254 = (uint ******)*puVar12;
                  if (DAT_0062b254 != (uint ******)0x2) {
                    local_14 = *(uint ********)((int)local_20 * 0xd8 + 0x104 + param_1);
                    cVar4 = FUN_00408130();
                    if (*(char *)(iVar5 + 0x2eb58) != cVar4) {
                      DAT_0062cdf8 = 0xfffffffe;
                    }
                    if (*(char *)((int)local_14 + DAT_005d1c00 + 0x2ead6) != '\0')
                    goto LAB_00557c8a;
                  }
                  FUN_004bb8d0(0x10,0);
                  ppppppuVar11 = local_20;
                }
              }
              if (((&DAT_005d19a0)[(int)local_18] != (undefined4 *)0x0) &&
                 ((*(uint *)((int)(&DAT_005d19a0)[(int)local_18] + 0x10) & 0x106) != 0)) {
                puVar18 = &local_10c;
                local_128 = 3;
                local_10c = 0;
                if (-1 < (int)DAT_0062b254) {
                  puVar18 = &DAT_0062b254;
                }
                puVar12 = &local_128;
                if ((int)DAT_0062b254 < 4) {
                  puVar12 = puVar18;
                }
                DAT_0062b254 = (uint ******)*puVar12;
                if (((DAT_0062b254 == (uint ******)0x2) &&
                    ((iVar5 = FUN_00414960(), iVar5 == 0 || (iVar5 = FUN_00414960(), iVar5 == 3))))
                   || (ppppppuVar11 != (uint ******)0x1)) {
                  FUN_00431620(5);
                  FUN_004bb8d0(9,0);
                  FUN_004315b0(local_24);
                }
                else {
                  if (-1 < local_4c[3]) {
                    FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + local_4c[3] * 4),3);
                  }
                  *(undefined4 *)(param_1 + 0x24) = 0;
                  FUN_004bb8d0(9,0);
                }
                break;
              }
            }
          }
          else if (((-1 < (int)local_18) && ((&DAT_005d19a0)[(int)local_18] != (undefined4 *)0x0))
                  && ((*(uint *)((int)(&DAT_005d19a0)[(int)local_18] + 0x10) & 0x106) != 0)) {
            if (-1 < local_4c[(int)local_20 + 3]) {
              FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + local_4c[(int)local_20 + 3] * 4),3);
            }
            *(undefined4 *)(param_1 + 0x24 + (int)ppppppuVar11 * 4) = 0;
            FUN_004bb8d0(9,0);
          }
        }
        local_20 = (uint ******)((int)ppppppuVar11 + 1);
      } while ((int)ppppppuVar11 + 1 < 2);
    }
    break;
  case 4:
    if (*(int *)(param_1 + 0x618) == 10) {
      puVar18 = (undefined4 *)FUN_00505b30(local_114,in_stack_fffffeb8,uVar29,puVar30);
      DAT_0062bd10 = *puVar18;
      FUN_004cb830(DAT_0062bd10,7);
    }
    if (0x27 < *(int *)(param_1 + 0x618)) {
      FUN_004d5090();
      DAT_0062baa0 = 0xffffffff;
      FUN_004e3740();
      FUN_004e3740();
      FUN_00409f40();
      FUN_004316b0(2);
      FUN_00522180();
      iVar5 = FUN_0040a540();
      if ((iVar5 == 0) || (iVar5 == 1)) {
        _DAT_0062b0c8 = 0;
        _DAT_0062b190 = 1;
        DAT_0062ba80 = 1;
        DAT_0062ba84 = 1;
        FUN_004de2f0(0,0);
        FUN_004de2f0(0,0);
      }
      else {
        DAT_0062ba84 = iVar5 + -2;
        if (DAT_0062ba84 == 0) {
          DAT_0062ba80 = DAT_0062ba84;
          uVar29 = FUN_00414960();
          switch(uVar29) {
          case 0:
          case 3:
            _DAT_0062b0c8 = 0;
            _DAT_0062b190 = 0;
            break;
          case 1:
            _DAT_0062b0c8 = 0;
            _DAT_0062b190 = 1;
            break;
          case 2:
            _DAT_0062b0c8 = 1;
            _DAT_0062b190 = 1;
          }
        }
      }
      DAT_0062ba84 = 1;
      FUN_00412d50(*(undefined4 *)(param_1 + 0x104));
      FUN_00412d50(*(undefined4 *)(param_1 + 0x1dc));
      DAT_005d1c14 = 3;
      _memset(&DAT_0062babc,0,0x50);
      DAT_0062c450 = 7;
      if ((DAT_0062b0ac & 0x60) == 0) {
        FUN_00534820();
      }
    }
    break;
  case 5:
    if (5 < *(int *)(param_1 + 0x618)) {
      FUN_004315b0(local_24);
      iVar5 = 0;
      do {
        if (-1 < *(int *)((int)local_4c + iVar5)) {
          FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + *(int *)((int)local_4c + iVar5) * 4),1);
          *(undefined4 *)(param_1 + 0x628 + *(int *)((int)local_4c + iVar5) * 4) = 0;
        }
        iVar10 = *(int *)((int)local_4c + iVar5 + 0xc);
        if (-1 < iVar10) {
          FUN_004cb830(*(undefined4 *)(param_1 + 0x628 + iVar10 * 4),1);
          *(undefined4 *)(param_1 + 0x628 + *(int *)((int)local_4c + iVar5 + 0xc) * 4) = 0;
        }
        iVar5 = iVar5 + 4;
      } while (iVar5 < 8);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x96c),1);
      *(undefined4 *)(param_1 + 0x96c) = 0;
      FUN_004cb830(*(undefined4 *)(param_1 + 0x76c),1);
      *(undefined4 *)(param_1 + 0x76c) = 0;
      FUN_004316b0(8);
      FUN_004d5040();
      FUN_004e3740();
      FUN_004e3740();
      FUN_00409f40();
      iVar5 = DAT_005d1c28;
      pppuVar20 = (uint ***)&local_2c;
      local_38 = (uint *****)0x13;
      if (-1 < *(int *)(param_1 + 0x104)) {
        pppuVar20 = (uint ***)&DAT_0062b0cc;
      }
      local_2c = (uint ***)0x0;
      pppuVar17 = (uint ***)(local_4c + 5);
      if (*(int *)(param_1 + 0x104) < 0x14) {
        pppuVar17 = pppuVar20;
      }
      pppuVar20 = (uint ***)&local_2c;
      DAT_0062b0cc = (uint ******)*pppuVar17;
      if (-1 < *(int *)(param_1 + 0x1dc)) {
        pppuVar20 = (uint ***)&DAT_0062b194;
      }
      local_38 = (uint *****)0x13;
      local_2c = (uint ***)0x0;
      pppuVar17 = (uint ***)(local_4c + 5);
      if (*(int *)(param_1 + 0x1dc) < 0x14) {
        pppuVar17 = pppuVar20;
      }
      DAT_0062b194 = (uint ******)*pppuVar17;
      *(undefined4 *)(param_1 + 0x5d8c) = 0;
      if (iVar5 != 0) {
        *(uint *)(iVar5 + 0x70) = *(uint *)(iVar5 + 0x70) & 0xffffff1f;
        *(undefined4 *)(iVar5 + 0x8c) = 0;
      }
    }
  }
switchD_005567a0_caseD_1:
  FUN_0046231d(&DAT_0062b018);
  ExceptionList = local_10;
  return 1;
}


// Reference 0055b2da
// Reference 0055b38f
