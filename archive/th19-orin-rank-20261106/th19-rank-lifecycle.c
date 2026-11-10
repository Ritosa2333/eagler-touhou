// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 51b051

undefined4 __fastcall FUN_0051afe0(int param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int local_20 [7];
  
  FUN_004026c0(param_1);
  piVar3 = local_20;
  local_20[1] = 0xea24;
  if (0 < DAT_0062b26c) {
    piVar3 = &DAT_0062b26c;
  }
  local_20[0] = 1;
  piVar1 = local_20 + 1;
  if (DAT_0062b26c < 0xea25) {
    piVar1 = piVar3;
  }
  DAT_0062b26c = *piVar1;
  if ((*(int *)(param_1 + 0x28) % DAT_0062b26c == 0) && (*(int *)(DAT_005d1a50 + 0x310) == 0)) {
    piVar3 = local_20 + 1;
    if (-1 < DAT_0062b260 + 1) {
      piVar3 = &DAT_0062b260;
    }
    local_20[0] = DAT_0062b268;
    local_20[1] = 0;
    piVar1 = local_20;
    if (DAT_0062b260 + 1 <= DAT_0062b268) {
      piVar1 = piVar3;
    }
    DAT_0062b260 = *piVar1;
  }
  piVar3 = local_20 + 2;
  local_20[3] = 3;
  local_20[2] = 0;
  if (-1 < DAT_0062b254) {
    piVar3 = &DAT_0062b254;
  }
  piVar1 = local_20 + 3;
  if (DAT_0062b254 < 4) {
    piVar1 = piVar3;
  }
  DAT_0062b254 = *piVar1;
  if ((DAT_0062b254 == 2) && (0x2a2f < *(int *)(param_1 + 0x28))) {
    piVar3 = local_20 + 4;
    local_20[5] = 99999;
    local_20[4] = 0;
    if (-1 < DAT_0062b250) {
      piVar3 = &DAT_0062b250;
    }
    piVar1 = local_20 + 5;
    if (DAT_0062b250 < 100000) {
      piVar1 = piVar3;
    }
    iVar2 = *piVar1;
    if (iVar2 == 0) {
      DAT_0062b268 = 100;
      DAT_0062b250 = 0;
    }
    else {
      local_20[0] = 99999;
      local_20[1] = 0;
      piVar3 = local_20 + 1;
      if (-1 < iVar2) {
        piVar3 = &DAT_0062b250;
      }
      piVar1 = local_20;
      if (iVar2 < 100000) {
        piVar1 = piVar3;
      }
      DAT_0062b250 = *piVar1;
      if (DAT_0062b250 == 0) {
        DAT_0062b268 = 0x78;
      }
      else {
        iVar2 = FUN_0040e4d0();
        DAT_0062b268 = (-(uint)(iVar2 != 0) & 0x28) + 0xa0;
      }
    }
    DAT_0062b26c = 0xf0;
  }
  local_20[0] = 3;
  local_20[1] = 0;
  piVar3 = local_20 + 1;
  if (-1 < DAT_0062b254) {
    piVar3 = &DAT_0062b254;
  }
  piVar1 = local_20;
  if (DAT_0062b254 < 4) {
    piVar1 = piVar3;
  }
  DAT_0062b254 = *piVar1;
  if (((DAT_0062b254 == 0) && (*(int *)(param_1 + 0x28) % 0x3c == 0x3b)) &&
     (*(int *)(DAT_005d1a50 + 0x310) == 0)) {
    FUN_0040b500(1);
  }
  iVar2 = *(int *)(param_1 + 0x5c);
  if (*(int *)(param_1 + 0x4c + iVar2 * 4) != 0) {
    iVar2 = iVar2 + 1;
    *(int *)(param_1 + 0x5c) = iVar2;
    if (3 < iVar2) {
      *(undefined4 *)(param_1 + 0x5c) = 3;
      iVar2 = 3;
    }
  }
  piVar3 = *(int **)(param_1 + 0x3c);
  iVar2 = param_1 + iVar2 * 4;
  iVar4 = 0;
  if (piVar3 == (int *)0x0) {
    *(undefined4 *)(iVar2 + 0x4c) = 0;
  }
  else {
    do {
      *(int *)(iVar2 + 0x4c) = piVar3[1];
      if (*piVar3 != 0) {
        iVar4 = iVar4 + 1;
        iVar2 = FUN_0051c8c0();
        if (iVar2 != 0) {
          FUN_0051c7f0();
        }
      }
      piVar3 = *(int **)(param_1 + 0x4c + *(int *)(param_1 + 0x5c) * 4);
      iVar2 = param_1 + *(int *)(param_1 + 0x5c) * 4;
    } while (piVar3 != (int *)0x0);
  }
  *(int *)(param_1 + 0xb0) = iVar4;
  iVar2 = *(int *)(param_1 + 0x84);
  if (*(int *)(param_1 + 0x74 + iVar2 * 4) != 0) {
    iVar2 = iVar2 + 1;
    *(int *)(param_1 + 0x84) = iVar2;
    if (3 < iVar2) {
      *(undefined4 *)(param_1 + 0x84) = 3;
      iVar2 = 3;
    }
  }
  piVar3 = *(int **)(param_1 + 100);
  iVar2 = param_1 + iVar2 * 4;
  iVar4 = 0;
  if (piVar3 == (int *)0x0) {
    *(undefined4 *)(iVar2 + 0x74) = 0;
  }
  else {
    do {
      *(int *)(iVar2 + 0x74) = piVar3[1];
      if (*piVar3 != 0) {
        iVar4 = iVar4 + 1;
        iVar2 = FUN_0051bd50();
        if (iVar2 != 0) {
          FUN_0051c150();
        }
      }
      piVar3 = *(int **)(param_1 + 0x74 + *(int *)(param_1 + 0x84) * 4);
      iVar2 = param_1 + *(int *)(param_1 + 0x84) * 4;
    } while (piVar3 != (int *)0x0);
  }
  *(int *)(param_1 + 0xb4) = iVar4;
  iVar2 = *(int *)(param_1 + 0xac);
  if (*(int *)(param_1 + 0x9c + iVar2 * 4) != 0) {
    iVar2 = iVar2 + 1;
    *(int *)(param_1 + 0xac) = iVar2;
    if (3 < iVar2) {
      *(undefined4 *)(param_1 + 0xac) = 3;
      iVar2 = 3;
    }
  }
  piVar3 = *(int **)(param_1 + 0x8c);
  iVar2 = param_1 + iVar2 * 4;
  iVar4 = 0;
  if (piVar3 == (int *)0x0) {
    *(undefined4 *)(iVar2 + 0x9c) = 0;
  }
  else {
    do {
      *(int *)(iVar2 + 0x9c) = piVar3[1];
      if (*piVar3 != 0) {
        iVar4 = iVar4 + 1;
        iVar2 = FUN_0051b480();
        if (iVar2 != 0) {
          FUN_0051b3d0();
        }
      }
      piVar3 = *(int **)(param_1 + 0x9c + *(int *)(param_1 + 0xac) * 4);
      iVar2 = param_1 + *(int *)(param_1 + 0xac) * 4;
    } while (piVar3 != (int *)0x0);
  }
  iVar2 = DAT_005d19bc;
  *(int *)(param_1 + 0xb8) = iVar4;
  *(undefined4 *)(iVar2 + 0x710) = 0;
  iVar2 = *(int *)(param_1 + 0x14);
  if (iVar2 != 0xe10) {
    if (iVar2 != 0x1c20) {
      if (iVar2 == 0x2a30) {
        *(undefined4 *)(param_1 + 0xc4) = 999;
      }
      FUN_004026c0(0);
      return 1;
    }
    *(undefined4 *)(param_1 + 0xc4) = 0x50;
    FUN_004026c0(0);
    return 1;
  }
  *(undefined4 *)(param_1 + 0xc4) = 0x3c;
  FUN_004026c0(0);
  return 1;
}


// Address 51bd92

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0051bd50(int param_1)

{
  float *pfVar1;
  int iVar2;
  undefined8 *puVar3;
  float *pfVar4;
  float fVar5;
  float fVar6;
  double dVar7;
  undefined1 auStack_4f4 [16];
  float local_4e4;
  float local_4e0 [4];
  undefined4 local_4d0;
  undefined4 local_4cc;
  undefined8 local_4c8;
  undefined4 local_4c0;
  float local_4bc;
  undefined4 local_4b8;
  float local_4b4;
  undefined4 local_4b0;
  undefined4 local_4ac;
  undefined4 local_64;
  undefined2 local_60;
  undefined4 local_54;
  undefined1 local_4b;
  uint local_44;
  
  local_44 = DAT_005c3d80 ^ (uint)auStack_4f4;
  if (*(int *)(param_1 + 4) < *(int *)(param_1 + 0x14)) {
    puVar3 = (undefined8 *)FUN_004037e0();
    *(undefined8 *)(param_1 + 0x18) = *puVar3;
    *(undefined4 *)(param_1 + 0x20) = *(undefined4 *)(puVar3 + 1);
    *(undefined8 *)(param_1 + 0x6c4) = *puVar3;
    *(undefined4 *)(param_1 + 0x6cc) = *(undefined4 *)(puVar3 + 1);
    iVar2 = FUN_004bd660();
    fVar5 = DAT_0062eeb0;
    if (iVar2 != 0) {
      return 1;
    }
    *(undefined1 *)(param_1 + 0x569) = 1;
    fVar6 = fVar5 * _DAT_005aa21c;
    fVar5 = fVar5 * _DAT_005aa21c;
    *(undefined1 *)(param_1 + 0x55f) = 0xc0;
    *(float *)(param_1 + 0x104) = fVar6;
    *(float *)(param_1 + 0x108) = fVar5;
    FUN_004026c0();
    return 0;
  }
  if ((*(uint *)(DAT_005d1a54 + 0x140) & 0x80000) != 0) {
    return 1;
  }
  pfVar4 = local_4e0;
  local_4e0[0] = 0.0;
  if (-1 < (int)DAT_0062b260) {
    pfVar4 = &DAT_0062b260;
  }
  local_4e4 = DAT_0062b268;
  local_4c8 = 0;
  pfVar1 = &local_4e4;
  if ((int)DAT_0062b260 <= (int)DAT_0062b268) {
    pfVar1 = pfVar4;
  }
  local_4c0 = 0;
  DAT_0062b260 = *pfVar1;
  local_4e0[0] = (float)(int)DAT_0062b260 / DAT_005aa3e0;
  FUN_0040df60();
  _memset(&local_4d0,0,0x488);
  local_4c8 = *(undefined8 *)(param_1 + 0x30);
  local_4d0 = *(undefined4 *)(param_1 + 0xa4);
  local_4cc = *(undefined4 *)(param_1 + 0xa8);
  local_4c0 = *(undefined4 *)(param_1 + 0x38);
  local_54 = 0xffffffff;
  dVar7 = _fabs((double)(float)local_4c8);
  local_4e4 = (float)dVar7;
  if (DAT_005aa368 <= local_4e4) {
    fVar5 = 0.0;
    if ((float)local_4c8 < 0.0) {
      fVar5 = DAT_005aa2bc / DAT_005aa280;
      local_4e4 = fVar5;
      FUN_00404c90();
      local_4bc = local_4e4 - fVar5;
      goto LAB_0051bf5e;
    }
    FUN_00404c90();
    local_4bc = DAT_005aa2bc / DAT_005aa280;
  }
  else {
    local_4e4 = DAT_005aa2bc / _DAT_005aa2e8;
    iVar2 = FUN_00402540();
    fVar5 = DAT_005aa2bc / DAT_005aa280;
    local_4bc = ((float)((double)iVar2 + (double)(&DAT_005aa660)[-(iVar2 >> 0x1f)]) /
                 ((float)((double)DAT_005c6630 + (double)(&DAT_005aa660)[-(DAT_005c6630 >> 0x1f)]) /
                  DAT_005aa280 - DAT_005aa230) - DAT_005aa230) * local_4e4;
  }
  local_4bc = fVar5 + local_4bc;
LAB_0051bf5e:
  local_4b8 = 0;
  iVar2 = FUN_00402540();
  local_4b0 = 0x3f800000;
  local_4ac = 0;
  local_64 = 0x10001;
  local_60 = 1;
  local_4b = 1;
  local_4b4 = ((float)((double)iVar2 + (double)(&DAT_005aa660)[-(iVar2 >> 0x1f)]) /
              ((float)((double)DAT_005c6630 + (double)(&DAT_005aa660)[-(DAT_005c6630 >> 0x1f)]) -
              DAT_005aa230)) * DAT_005aa1f4 + *(float *)(param_1 + 0xac) +
              local_4e0[0] * DAT_005aa280;
  *(undefined4 *)(*(int *)(&DAT_005d1a9c + *(int *)(param_1 + 0x6e4) * -0x3c) + 0x48) = 0;
  *(int *)(DAT_005d19bc + 0x710) = 1 - *(int *)(param_1 + 0x6e4);
  FUN_004f1340();
  *(undefined4 *)(*(int *)(&DAT_005d1a9c + *(int *)(param_1 + 0x6e4) * -0x3c) + 0x48) = 0;
  return 1;
}


// Address 5450e8

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005450e0(int *param_1)

{
  int *piVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  longlong lVar4;
  longlong lVar5;
  float10 fVar6;
  undefined *puVar7;
  float *pfVar8;
  int iVar9;
  int iVar10;
  undefined4 uVar11;
  int *piVar12;
  uint uVar13;
  int *piVar14;
  uint uVar15;
  float *pfVar16;
  float10 fVar17;
  float fVar18;
  float fVar19;
  int local_80;
  undefined8 local_7c;
  int local_74;
  float local_70;
  float local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  float local_5c;
  float local_58;
  undefined4 local_54;
  float local_50;
  float local_4c;
  float local_48;
  int local_3c;
  int local_38;
  int *local_34;
  int local_30;
  undefined4 local_28;
  float local_20;
  float *local_1c;
  float local_18;
  float local_14;
  float local_10;
  int *local_c;
  bool local_5;
  
  local_14 = 0.0;
  pfVar16 = &local_14;
  if (-1 < (int)DAT_0062b260) {
    pfVar16 = &DAT_0062b260;
  }
  local_34 = param_1;
  local_10 = DAT_0062b268;
  pfVar8 = &local_10;
  if ((int)DAT_0062b260 <= (int)DAT_0062b268) {
    pfVar8 = pfVar16;
  }
  DAT_0062b260 = *pfVar8;
  local_10 = (float)(int)DAT_0062b260 / DAT_005aa3e0;
  if (0 < param_1[6]) {
    if (param_1[8] != 0) {
      param_1[8] = 0;
    }
    puVar7 = PTR_DAT_00596da0;
    param_1[5] = param_1[6];
    if ((puVar7 == (undefined *)0x0) ||
       ((_DAT_005aa22c < *(float *)puVar7 &&
        (*(float *)puVar7 <= DAT_005aa234 && DAT_005aa234 != *(float *)puVar7)))) {
      fVar18 = (float)param_1[7] - DAT_005aa230;
    }
    else {
      fVar18 = *(float *)puVar7 * DAT_005aa4d0 + (float)param_1[7];
    }
    param_1[7] = (int)fVar18;
    param_1[6] = (int)fVar18;
    if ((int)fVar18 < 1) {
      param_1[0x10] = 0;
    }
  }
  local_50 = 0.0;
  local_4c = 0.0;
  local_48 = 0.0;
  if (param_1[0xb] == 0) {
    iVar9 = param_1[0x14];
    if (0x96 - (int)(local_10 * DAT_005aa390) <= iVar9) {
      local_54 = 0;
      local_1c = &local_50;
      local_3c = (int)(local_10 * DAT_005aa520);
      local_18 = DAT_005aa2bc * DAT_005aa280;
      local_38 = 0x96 - (int)(local_10 * DAT_005aa390);
      local_c = (int *)DAT_005aa230;
      do {
        param_1[0x13] = param_1[0x13] + 1;
        param_1[0x14] = (iVar9 - local_3c) + -0x96;
        if ((param_1[0xe] & 1U) == 0) {
          param_1[0xd] = 0;
          param_1[0xe] = param_1[0xe] | 1;
        }
        param_1[0xb] = 4;
        param_1[0xc] = 0x40800000;
        param_1[10] = 3;
        iVar9 = FUN_00402540();
        local_14 = ((float)((double)iVar9 + (double)(&DAT_005aa660)[-(iVar9 >> 0x1f)]) /
                   ((float)((double)(int)DAT_005c6630 +
                           (double)(&DAT_005aa660)[-((int)DAT_005c6630 >> 0x1f)]) - (float)local_c))
                   * DAT_005aa320;
        iVar9 = FUN_00402540();
        pfVar16 = local_1c;
        local_20 = (float)((double)iVar9 + (double)(&DAT_005aa660)[-(iVar9 >> 0x1f)]) /
                   ((float)((double)(int)DAT_005c6630 +
                           (double)(&DAT_005aa660)[-((int)DAT_005c6630 >> 0x1f)]) / local_18 -
                   DAT_005aa2bc) - DAT_005aa2bc;
        fVar6 = (float10)fcos((float10)local_20);
        fVar17 = (float10)fsin((float10)local_20);
        *local_1c = (float)(fVar6 * (float10)local_14);
        pfVar16[1] = (float)(fVar17 * (float10)local_14);
        local_50 = local_50 + (float)param_1[0x16];
        local_4c = local_4c + (float)param_1[0x17];
        local_48 = local_48 + (float)param_1[0x18];
        iVar9 = FUN_00402540();
        local_5c = (((float)((double)iVar9 + (double)(&DAT_005aa660)[-(iVar9 >> 0x1f)]) /
                     ((float)((double)(int)DAT_005c6630 +
                             (double)(&DAT_005aa660)[-((int)DAT_005c6630 >> 0x1f)]) / DAT_005aa280 -
                     (float)local_c) - (float)local_c) * DAT_005aa404) / DAT_005aa280;
        iVar10 = FUN_00402540();
        iVar9 = *(int *)(DAT_005d1bd8 + 0x28);
        local_58 = ((float)((double)iVar10 + (double)(&DAT_005aa660)[-(iVar10 >> 0x1f)]) /
                   ((float)((double)(int)DAT_005c6630 +
                           (double)(&DAT_005aa660)[-((int)DAT_005c6630 >> 0x1f)]) - (float)local_c))
                   * DAT_005aa30c;
        if (iVar9 < 0x2a30) {
          if (iVar9 < 0x1c20) {
            if (iVar9 < 0xe10) {
              local_5 = false;
            }
            else {
              uVar13 = FUN_00402540();
              local_5 = 3 < uVar13 % 5;
            }
          }
          else {
            iVar9 = FUN_0046230c(&DAT_0062aef8);
            if (iVar9 != 0) goto LAB_00545a82;
            if (DAT_0062af24 == 0x7fffffff) goto LAB_00545a71;
            uVar3 = (ulonglong)DAT_005c6624 * 0xbc8f;
            local_14 = (float)(uVar3 >> 0x20);
            local_20 = (float)uVar3;
            lVar4 = (uVar3 & 0xffffffff) * 2;
            local_30 = (int)((ulonglong)lVar4 >> 0x20);
            lVar5 = (ulonglong)(uint)local_14 * 2;
            local_28 = (undefined4)lVar5;
            uVar13 = local_30 +
                     (uint)CARRY4((uint)lVar4,
                                  (uint)((ulonglong)(uint)local_14 * 5 +
                                        ((uVar3 & 0xffffffff) * 5 >> 0x20)));
            lVar4 = uVar3 - (lVar5 + (ulonglong)uVar13);
            uVar15 = (uint)((ulonglong)lVar4 >> 0x20);
            lVar4 = lVar5 + (ulonglong)uVar13 +
                    CONCAT44(uVar15 >> 1,(uint)lVar4 >> 1 | uVar15 * -0x80000000);
            DAT_005c6624 = (int)local_20 +
                           ((uint)lVar4 >> 0x1e | (int)((ulonglong)lVar4 >> 0x20) * 4) * -0x7fffffff
            ;
            uVar3 = (ulonglong)DAT_005c6630;
            uVar2 = (ulonglong)DAT_005c6624;
            DAT_005c6634 = DAT_005c6624;
            FUN_0046231d(&DAT_0062aef8);
            local_5 = 2 < (uint)((int)(uVar2 % uVar3) + (int)((uVar2 % uVar3) / 5) * -5);
          }
        }
        else {
          iVar9 = FUN_0046230c(&DAT_0062aef8);
          if (iVar9 != 0) goto LAB_00545a82;
          if (DAT_0062af24 == 0x7fffffff) goto LAB_00545a71;
          uVar3 = (ulonglong)DAT_005c6624 * 0xbc8f;
          local_14 = (float)(uVar3 >> 0x20);
          local_30 = (int)uVar3;
          lVar4 = (uVar3 & 0xffffffff) * 2;
          local_20 = (float)((ulonglong)lVar4 >> 0x20);
          lVar5 = (ulonglong)(uint)local_14 * 2;
          local_28 = (undefined4)lVar5;
          uVar13 = (int)local_20 +
                   (uint)CARRY4((uint)lVar4,
                                (uint)((ulonglong)(uint)local_14 * 5 +
                                      ((uVar3 & 0xffffffff) * 5 >> 0x20)));
          lVar4 = uVar3 - (lVar5 + (ulonglong)uVar13);
          uVar15 = (uint)((ulonglong)lVar4 >> 0x20);
          lVar4 = lVar5 + (ulonglong)uVar13 +
                  CONCAT44(uVar15 >> 1,(uint)lVar4 >> 1 | uVar15 * -0x80000000);
          DAT_005c6624 = local_30 +
                         ((uint)lVar4 >> 0x1e | (int)((ulonglong)lVar4 >> 0x20) * 4) * -0x7fffffff;
          uVar3 = (ulonglong)DAT_005c6630;
          uVar2 = (ulonglong)DAT_005c6624;
          DAT_005c6634 = DAT_005c6624;
          FUN_0046231d(&DAT_0062aef8);
          local_5 = 2 < (uint)((int)(uVar2 % uVar3) + (int)((uVar2 % uVar3) / 6) * -6);
        }
        iVar9 = FUN_0046230c(&DAT_0062aef8);
        if (iVar9 != 0) {
LAB_00545a82:
                    /* WARNING: Subroutine does not return */
          FUN_004627e2(5);
        }
        if (DAT_0062af24 == 0x7fffffff) {
LAB_00545a71:
          DAT_0062af24 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
          FUN_004627e2(6);
        }
        uVar3 = (ulonglong)DAT_005c6624 * 0xbc8f;
        local_14 = (float)(uVar3 >> 0x20);
        local_20 = (float)uVar3;
        lVar4 = (uVar3 & 0xffffffff) * 2;
        local_30 = (int)((ulonglong)lVar4 >> 0x20);
        lVar5 = (ulonglong)(uint)local_14 * 2;
        local_28 = (undefined4)lVar5;
        uVar13 = local_30 +
                 (uint)CARRY4((uint)lVar4,
                              (uint)((ulonglong)(uint)local_14 * 5 +
                                    ((uVar3 & 0xffffffff) * 5 >> 0x20)));
        lVar4 = uVar3 - (lVar5 + (ulonglong)uVar13);
        uVar15 = (uint)((ulonglong)lVar4 >> 0x20);
        lVar4 = lVar5 + (ulonglong)uVar13 +
                CONCAT44(uVar15 >> 1,(uint)lVar4 >> 1 | uVar15 * -0x80000000);
        uVar13 = (int)local_20 +
                 ((uint)lVar4 >> 0x1e | (int)((ulonglong)lVar4 >> 0x20) * 4) * -0x7fffffff;
        DAT_005c6624 = uVar13;
        DAT_005c6634 = uVar13;
        FUN_0046231d(&DAT_0062aef8);
        param_1 = local_34;
        uVar11 = 1;
        if (local_5 != false) {
          uVar11 = 4;
        }
        FUN_0051c650(local_34[0x19],&local_50,&local_5c,uVar11,uVar13 & 0xf,0x3f800000);
        iVar9 = param_1[0x14];
      } while (local_38 <= iVar9);
    }
    iVar10 = 0x78 - (int)(local_10 * DAT_005aa344);
    iVar9 = param_1[0x15];
    if (iVar10 <= iVar9) {
      fVar19 = local_10 * _DAT_005aa508;
      fVar18 = local_10 * DAT_005aa2ec + DAT_005aa2e4;
      local_10 = fVar18;
      do {
        param_1[0x15] = (iVar9 - (int)fVar19) + -0x78;
        if ((param_1[0xe] & 1U) == 0) {
          param_1[0xb] = 0;
          param_1[10] = -999999;
          param_1[0xc] = 0;
          param_1[0xd] = 0;
          param_1[0xe] = param_1[0xe] | 1;
        }
        iVar9 = DAT_005d1bd8;
        param_1[0xb] = 10;
        param_1[0xc] = 0x41200000;
        param_1[10] = 9;
        local_80 = param_1[0x19];
        if ((float)*(int *)(iVar9 + 0xbc + local_80 * 4) < fVar18) {
          local_74 = param_1[0x18];
          local_7c = *(undefined8 *)(param_1 + 0x16);
          local_64 = 0;
          local_60 = 1;
          iVar9 = FUN_00402540();
          local_70 = (((float)((double)iVar9 + (double)(&DAT_005aa660)[-(iVar9 >> 0x1f)]) /
                       ((float)((double)(int)DAT_005c6630 +
                               (double)(&DAT_005aa660)[-((int)DAT_005c6630 >> 0x1f)]) / DAT_005aa280
                       - DAT_005aa230) - DAT_005aa230) * DAT_005aa404) / DAT_005aa280;
          iVar9 = FUN_00402540();
          local_68 = 0;
          local_6c = ((float)((double)iVar9 + (double)(&DAT_005aa660)[-(iVar9 >> 0x1f)]) /
                     ((float)((double)(int)DAT_005c6630 +
                             (double)(&DAT_005aa660)[-((int)DAT_005c6630 >> 0x1f)]) - DAT_005aa230))
                     * DAT_005aa368;
          FUN_0051bb00(&local_80);
          fVar18 = local_10;
        }
        iVar9 = param_1[0x15];
      } while (iVar10 <= iVar9);
    }
  }
  else {
    if (param_1[0xd] != 0) {
      param_1[0xd] = 0;
    }
    puVar7 = PTR_DAT_00596da0;
    param_1[10] = param_1[0xb];
    if ((puVar7 == (undefined *)0x0) ||
       ((_DAT_005aa22c < *(float *)puVar7 &&
        (*(float *)puVar7 <= DAT_005aa234 && DAT_005aa234 != *(float *)puVar7)))) {
      fVar18 = (float)param_1[0xc] - DAT_005aa230;
    }
    else {
      fVar18 = *(float *)puVar7 * DAT_005aa4d0 + (float)param_1[0xc];
    }
    param_1[0xc] = (int)fVar18;
    param_1[0xb] = (int)fVar18;
  }
  local_c = param_1 + 1;
  if (param_1[1] < 1) {
    if (99 < param_1[0x14]) {
      return;
    }
    param_1[0x13] = 0;
    return;
  }
  if (param_1[3] != 0) {
    param_1[3] = 0;
    local_c = param_1 + 1;
  }
  puVar7 = PTR_DAT_00596da0;
  local_c = param_1 + 1;
  *param_1 = param_1[1];
  if (puVar7 == (undefined *)0x0) {
  }
  else if ((*(float *)puVar7 <= _DAT_005aa22c) ||
          (local_c = param_1 + 1,
          DAT_005aa234 < *(float *)puVar7 || DAT_005aa234 == *(float *)puVar7)) {
    fVar18 = *(float *)puVar7 * DAT_005aa4d0 + (float)param_1[2];
    goto LAB_005459e5;
  }
  fVar18 = (float)param_1[2] - DAT_005aa230;
LAB_005459e5:
  param_1[2] = (int)fVar18;
  param_1[1] = (int)fVar18;
  if ((int)fVar18 < 1) {
    if (0x13 < param_1[0x11]) {
      iVar9 = *(int *)(param_1[0x1a] + 0x2c);
      piVar1 = (int *)(iVar9 + 0x44);
      iVar10 = *(int *)(iVar9 + 0x44) + 1;
      *piVar1 = iVar10;
      piVar14 = &local_3c;
      if (-1 < iVar10) {
        piVar14 = piVar1;
      }
      local_38 = *(int *)(iVar9 + 0x40);
      piVar12 = &local_38;
      if (iVar10 <= *(int *)(iVar9 + 0x40)) {
        piVar12 = piVar14;
      }
      *piVar1 = *piVar12;
    }
    param_1[0x11] = 0;
    uVar13 = param_1[4];
    if ((uVar13 & 1) == 0) {
      *local_c = 0;
      param_1[2] = 0;
      param_1[3] = 0;
      param_1[4] = uVar13 | 1;
    }
    *local_c = 0;
    param_1[2] = 0;
    *param_1 = -1;
  }
  return;
}


// Address 548d08

void __thiscall FUN_00548d00(int param_1,int param_2,undefined8 *param_3)

{
  int iVar1;
  int iVar2;
  undefined8 *puVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  float in_XMM0_Da;
  int local_c [2];
  
  local_c[1] = 0;
  piVar5 = local_c + 1;
  if (-1 < DAT_0062b260) {
    piVar5 = &DAT_0062b260;
  }
  local_c[0] = DAT_0062b268;
  piVar4 = local_c;
  if (DAT_0062b260 <= DAT_0062b268) {
    piVar4 = piVar5;
  }
  iVar6 = *piVar4;
  DAT_0062b260 = iVar6;
  FUN_0042fb80();
  puVar3 = param_3;
  iVar2 = param_2;
  iVar6 = iVar6 * param_2;
  param_2 = 0;
  iVar1 = *(int *)(*(int *)(param_1 + 0x68) + 0x38);
  param_3 = (undefined8 *)0x3;
  *(int *)(iVar1 + 0x1c) =
       *(int *)(iVar1 + 0x1c) + (int)(in_XMM0_Da * (float)iVar2) + (iVar6 / 100) * 2;
  *(undefined8 *)(iVar1 + 0x1d4) = *puVar3;
  *(undefined4 *)(iVar1 + 0x1dc) = *(undefined4 *)(puVar3 + 1);
  piVar5 = &param_2;
  if (-1 < DAT_0062b254) {
    piVar5 = &DAT_0062b254;
  }
  piVar4 = (int *)&param_3;
  if (DAT_0062b254 < 4) {
    piVar4 = piVar5;
  }
  DAT_0062b254 = *piVar4;
  if ((DAT_0062b254 == 0) && (*(int *)(param_1 + 100) == 0)) {
    FUN_0040b500(10);
  }
  return;
}


