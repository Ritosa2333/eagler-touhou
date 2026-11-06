// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 54fba0

undefined4 __fastcall FUN_0054fba0(int param_1)

{
  int iVar1;
  float fVar2;
  
  if (*(char *)(param_1 + 0x114) == '\0') {
    iVar1 = FUN_004cb8b0(*(undefined4 *)(param_1 + 0x14));
    if (iVar1 == 0) {
      *(undefined4 *)(param_1 + 0x14) = 0;
    }
    *(undefined4 *)(param_1 + 0x5c) = 0x3dcccccd;
    if (*(int *)(iVar1 + 0x604) != 0) {
      (**(code **)(&DAT_00596420 + *(int *)(iVar1 + 0x604) * 4))(iVar1,1);
    }
    *(undefined4 *)(iVar1 + 0x454) = 1;
    fVar2 = *(float *)(param_1 + 0x6c) / DAT_005aa2f0;
    *(undefined4 *)(param_1 + 0x9c) = 2;
    *(float *)(param_1 + 0x6c) = fVar2;
    *(undefined8 *)(iVar1 + 0x614) = *(undefined8 *)(param_1 + 0x54);
    *(undefined4 *)(iVar1 + 0x61c) = *(undefined4 *)(param_1 + 0x5c);
    FUN_00520240(*(undefined4 *)(param_1 + 0xdc));
    FUN_00520320();
    *(undefined4 *)(param_1 + 0xdc) = 0;
  }
  return *(undefined4 *)(param_1 + 0xac);
}


// Reference 0054bf34
// Reference 0054c069
// Reference 0054dc27
// Reference 0051e647
// Reference 0054b370
// Address 54df10

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 __fastcall FUN_0054df10(int param_1)

{
  char cVar1;
  float fVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined4 uVar7;
  float fVar8;
  double dVar9;
  undefined1 auStack_48 [4];
  float local_44;
  float local_40;
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
  
  local_c = DAT_005c3d80 ^ (uint)auStack_48;
  uVar3 = *(uint *)(param_1 + 0xd8) & 0xff;
  fVar2 = *(float *)(*(int *)(*(int *)(*(int *)(param_1 + 0x124) + 4) + 0x2238) + 0xf0 +
                    ((int)*(uint *)(param_1 + 0xd8) >> 8) * 4);
  local_40 = (float)(uVar3 * 0xf);
  iVar4 = *(int *)((int)fVar2 + 0x34 + uVar3 * 0x78);
  local_44 = fVar2;
  if ((0 < iVar4) && (iVar4 <= *(int *)(param_1 + 0x1c))) {
    FUN_004cb830(*(undefined4 *)(param_1 + 0x14),1);
    if ((*(uint *)(param_1 + 0x28) & 1) == 0) {
      *(undefined4 *)(param_1 + 0x1c) = 0;
      *(undefined4 *)(param_1 + 0x20) = 0;
      *(undefined4 *)(param_1 + 0x24) = 0;
      *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 1;
    }
    *(undefined4 *)(param_1 + 0x1c) = 0xfffffc19;
    *(undefined4 *)(param_1 + 0x20) = 0xc479c000;
    *(undefined4 *)(param_1 + 0x18) = 0xfffffc18;
  }
  if ((*(code **)(param_1 + 0x48) != (code *)0x0) &&
     (iVar4 = (**(code **)(param_1 + 0x48))(), iVar4 != 0)) {
LAB_0054e06d:
    FUN_0054f020();
    return 0xffffffff;
  }
  if (*(int *)(param_1 + 0x9c) != 2) {
    fVar8 = *(float *)((int)fVar2 + 0x18 + (int)local_40 * 8) + *(float *)(param_1 + 0x70);
    if (fVar8 <= DAT_005aa2bc) {
      if (fVar8 < DAT_005aa4d4) {
        iVar4 = 0;
        do {
          fVar8 = fVar8 + DAT_005aa2bc * DAT_005aa280;
          if (0x20 < iVar4) break;
          iVar4 = iVar4 + 1;
        } while (fVar8 < DAT_005aa4d4);
      }
    }
    else {
      iVar4 = 0;
      do {
        fVar8 = fVar8 - DAT_005aa2bc * DAT_005aa280;
        if (0x20 < iVar4) break;
        iVar4 = iVar4 + 1;
      } while (DAT_005aa2bc < fVar8);
    }
    *(float *)(param_1 + 0x70) = fVar8;
    *(float *)(param_1 + 0x6c) =
         *(float *)((int)fVar2 + 0x20 + (int)local_40 * 8) + *(float *)(param_1 + 0x6c);
  }
  FUN_004d4dc0();
  FUN_004d4a00();
  iVar4 = FUN_004cb8b0();
  if (iVar4 == 0) goto LAB_0054e06d;
  cVar1 = *(char *)((int)local_44 + 0x29 + (int)local_40 * 8);
  if ((cVar1 != '\x02') && (cVar1 != '\b')) {
    iVar6 = 4;
    puVar5 = &uStack_34;
    do {
      puVar5[-2] = 0;
      puVar5[-1] = 0;
      *puVar5 = 0;
      iVar6 = iVar6 + -1;
      puVar5 = puVar5 + 3;
    } while (iVar6 != 0);
    FUN_004ca2e0();
    if ((((0xe < *(int *)(param_1 + 0x1c)) &&
         ((((fStack_3c <= (float)DAT_0062ce48 - DAT_005aa404 / DAT_005aa280 ||
            (DAT_005aa404 / DAT_005aa280 + (float)DAT_0062ce48 <= fStack_3c)) ||
           (fStack_38 <= (float)DAT_0062ce4c)) || ((float)DAT_0062ce4c + DAT_005aa440 <= fStack_38))
         )) && ((((fStack_30 <= (float)DAT_0062ce48 - DAT_005aa404 / DAT_005aa280 ||
                  (DAT_005aa404 / DAT_005aa280 + (float)DAT_0062ce48 <= fStack_30)) ||
                 (fStack_2c <= (float)DAT_0062ce4c)) ||
                ((float)DAT_0062ce4c + DAT_005aa440 <= fStack_2c)))) &&
       ((((fStack_24 <= (float)DAT_0062ce48 - DAT_005aa404 / DAT_005aa280 ||
          (DAT_005aa404 / DAT_005aa280 + (float)DAT_0062ce48 <= fStack_24)) ||
         ((fStack_20 <= (float)DAT_0062ce4c || ((float)DAT_0062ce4c + DAT_005aa440 <= fStack_20))))
        && ((((fStack_18 <= (float)DAT_0062ce48 - DAT_005aa404 / DAT_005aa280 ||
              (DAT_005aa404 / DAT_005aa280 + (float)DAT_0062ce48 <= fStack_18)) ||
             (fStack_14 <= (float)DAT_0062ce4c)) ||
            ((float)DAT_0062ce4c + DAT_005aa440 <= fStack_14)))))) goto LAB_0054e06d;
  }
  if (((*(int *)(param_1 + 0xdc) != 0) && ((*(byte *)(param_1 + 0x40) & 1) != 0)) &&
     (local_44 = (float)FUN_00520240(), local_44 != 0.0)) {
    *(undefined8 *)((int)local_44 + 0x34) = *(undefined8 *)(param_1 + 0x54);
    *(undefined4 *)((int)local_44 + 0x3c) = *(undefined4 *)(param_1 + 0x5c);
    FUN_0042ffe0();
    uVar7 = *(undefined4 *)(param_1 + 0xb4);
    *(undefined4 *)((int)local_44 + 0x2c) = *(undefined4 *)(param_1 + 0xb0);
    *(undefined4 *)((int)local_44 + 0x30) = uVar7;
    *(undefined4 *)((int)local_44 + 0x98) = *(undefined4 *)(param_1 + 0xac);
  }
  *(undefined8 *)(iVar4 + 0x614) = *(undefined8 *)(param_1 + 0x54);
  *(undefined4 *)(iVar4 + 0x61c) = *(undefined4 *)(param_1 + 0x5c);
  switch(*(undefined1 *)(iVar4 + 0x4cc)) {
  case 1:
    uVar7 = *(undefined4 *)(param_1 + 0x70);
    goto LAB_0054e451;
  case 2:
    local_40 = *(float *)(param_1 + 0x70);
    if ((local_40 < DAT_005aa4d4 / DAT_005aa280) || (DAT_005aa2bc / DAT_005aa280 < local_40)) {
      puVar5 = (undefined4 *)FUN_004074f0();
      *(undefined4 *)(iVar4 + 0x44) = *puVar5;
      *(undefined1 *)(iVar4 + 0x4b8) = 1;
      dVar9 = _fabs((double)*(float *)(iVar4 + 0x54));
      local_44 = (float)dVar9;
      *(uint *)(iVar4 + 0x54) = (uint)local_44 ^ DAT_005aa690;
      *(undefined1 *)(iVar4 + 0x4b9) = 1;
    }
    else {
      *(undefined1 *)(iVar4 + 0x4b8) = 1;
      *(float *)(iVar4 + 0x44) = local_40;
      dVar9 = _fabs((double)*(float *)(iVar4 + 0x54));
      local_44 = (float)dVar9;
      *(float *)(iVar4 + 0x54) = local_44;
      *(undefined1 *)(iVar4 + 0x4b9) = 1;
    }
    goto switchD_0054e313_default;
  case 3:
    puVar5 = (undefined4 *)FUN_004074f0();
    break;
  case 4:
    puVar5 = (undefined4 *)FUN_004074f0();
    break;
  case 5:
    puVar5 = (undefined4 *)FUN_004073d0();
    break;
  default:
    goto switchD_0054e313_default;
  }
  uVar7 = *puVar5;
  FUN_00404a70();
LAB_0054e451:
  *(undefined1 *)(iVar4 + 0x4b8) = 1;
  *(undefined4 *)(iVar4 + 0x44) = uVar7;
switchD_0054e313_default:
  FUN_004026c0();
  return 0;
}


// Reference 0054f92d
// Address 54cd70

undefined4 __thiscall
FUN_0054cd70(int param_1,undefined8 *param_2,float *param_3,undefined4 param_4,float param_5)

{
  undefined8 uVar1;
  int *piVar2;
  float10 fVar3;
  bool bVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  float *pfVar8;
  undefined4 *puVar9;
  float10 fVar10;
  float fVar11;
  float fVar12;
  double dVar13;
  float local_78;
  uint local_68;
  float local_64;
  float local_60;
  undefined4 local_5c;
  float local_58;
  float local_54;
  undefined4 local_50;
  undefined8 local_4c;
  undefined4 local_44;
  
  uVar7 = *(uint *)(param_1 + 0xd8);
  local_68 = *(uint *)(*(int *)(*(int *)(*(int *)(param_1 + 0x124) + 4) + 0x2238) + 0xf0 +
                      ((int)uVar7 >> 8) * 4);
  *(undefined4 *)(param_1 + 0xa4) = 1;
  if (*(int *)(param_1 + 0xa8) == 0) {
    FUN_004cb830(*(undefined4 *)(param_1 + 0x14),2);
    *(undefined4 *)(param_1 + 0xa8) = 1;
  }
  if (param_3 == (float *)0x0) {
    iVar5 = FUN_00520240();
    if (iVar5 == 0) {
      return 0;
    }
    FUN_004d3720(param_1 + 0x54,param_2,*(float *)(iVar5 + 0x30) / DAT_005aa280 + param_5);
    fVar11 = local_78 + DAT_005aa2f0;
    *(float *)(param_1 + 0xb0) = fVar11;
    if (fVar11 < 0.0) {
      *(undefined4 *)(param_1 + 0xb0) = 0;
    }
    *(undefined4 *)(iVar5 + 0x2c) = *(undefined4 *)(param_1 + 0xb0);
  }
  else {
    local_64 = 0.0;
    local_60 = 0.0;
    local_5c = 0;
    local_4c = 0;
    local_44 = 0;
    iVar5 = FUN_00520240();
    if (iVar5 == 0) {
      return 0;
    }
    pfVar8 = (float *)(param_1 + 0x54);
    iVar6 = FUN_004d3140(pfVar8,*(undefined4 *)param_2,*(undefined4 *)((int)param_2 + 4),
                         *param_3 + *(float *)(iVar5 + 0x30),param_3[1] + *(float *)(iVar5 + 0x30),
                         param_4);
    if (iVar6 == 0) {
      uVar1 = *param_2;
      local_44 = *(undefined4 *)(param_2 + 1);
      local_4c._0_4_ = (float)uVar1;
      local_4c._4_4_ = (float)((ulonglong)uVar1 >> 0x20);
      local_4c._4_4_ = local_4c._4_4_ - *(float *)(param_1 + 0x58);
      fVar11 = local_4c._4_4_ * local_4c._4_4_ +
               ((float)local_4c - *pfVar8) * ((float)local_4c - *pfVar8);
      local_4c = uVar1;
      FUN_00405000();
      fVar12 = fVar11 - DAT_005aa320;
      *(float *)(param_1 + 0xb0) = fVar11 - DAT_005aa320;
      if (fVar12 < 0.0) {
        *(undefined4 *)(param_1 + 0xb0) = 0;
      }
      *(float *)(iVar5 + 0x2c) = *(float *)(param_1 + 0xb0) + DAT_005aa30c;
    }
    else {
      fVar11 = *(float *)(param_1 + 0x70);
      fVar12 = local_60 - *(float *)(param_1 + 0x58);
      FUN_00407490();
      if (fVar12 - fVar11 <= DAT_005aa2bc) {
        if (DAT_005aa2bc < fVar11 - fVar12) {
          fVar11 = fVar11 - DAT_005aa2bc * DAT_005aa280;
        }
      }
      else {
        fVar11 = DAT_005aa2bc * DAT_005aa280 + fVar11;
      }
      dVar13 = _fabs((double)(fVar12 - fVar11));
      if (DAT_005aa2bc / DAT_005aa280 <= (float)dVar13) {
        fVar11 = 0.0;
      }
      else {
        fVar11 = local_60 - *(float *)(param_1 + 0x58);
        fVar11 = fVar11 * fVar11 + (local_64 - *pfVar8) * (local_64 - *pfVar8);
        FUN_00405000();
      }
      fVar11 = fVar11 + DAT_005aa2f0;
      *(float *)(param_1 + 0xb0) = fVar11;
      if (fVar11 < 0.0) {
        *(undefined4 *)(param_1 + 0xb0) = 0;
      }
      *(undefined4 *)(iVar5 + 0x2c) = *(undefined4 *)(param_1 + 0xb0);
    }
  }
  pfVar8 = (float *)(param_1 + 0x70);
  if ((*(uint *)(param_1 + 0x1c) == *(uint *)(param_1 + 0x18)) ||
     ((*(uint *)(param_1 + 0x1c) & 1) != 0)) {
    bVar4 = false;
  }
  else {
    bVar4 = true;
  }
  if (bVar4) {
    fVar3 = (float10)fcos((float10)*pfVar8);
    fVar10 = (float10)fsin((float10)*pfVar8);
    uVar1 = *(undefined8 *)(param_1 + 0x54);
    local_44 = *(undefined4 *)(param_1 + 0x5c);
    local_4c._0_4_ = (float)uVar1;
    local_64 = (float)local_4c + (float)(fVar3 * (float10)*(float *)(param_1 + 0xb0));
    local_4c._4_4_ = (float)((ulonglong)uVar1 >> 0x20);
    local_60 = local_4c._4_4_ + (float)(fVar10 * (float10)*(float *)(param_1 + 0xb0));
    fVar3 = (float10)fcos((float10)*pfVar8);
    fVar10 = (float10)fsin((float10)*pfVar8);
    local_58 = (float)(fVar3 * (float10)64.0);
    local_54 = (float)(fVar10 * (float10)64.0);
    local_5c = 0;
    local_50 = 0;
    local_4c = uVar1;
    FUN_004cb960(&local_68,local_68,(int)*(short *)(local_68 + 0x2e + (uVar7 & 0xff) * 0x78),
                 &local_64,0,0xffffffff,0,0);
    iVar5 = *(int *)(*(int *)(param_1 + 0x124) + 0x28);
    iVar6 = FUN_0040ff90();
    if ((iVar6 != -1) && (iVar6 < 0x400)) {
      *(uint *)(iVar5 + 0x24 + iVar6 * 4) = local_68;
    }
    if (local_68 == 0) {
      iVar5 = 0;
    }
    else if ((local_68 & 0xffff) == 0xffff) {
      iVar6 = 0;
      puVar9 = (undefined4 *)(DAT_005d19bc + 0x71c);
      do {
        for (piVar2 = (int *)puVar9[-2]; piVar2 != (int *)0x0; piVar2 = (int *)piVar2[1]) {
          iVar5 = *piVar2;
          if (*(uint *)(iVar5 + 0x4d8) == local_68) goto LAB_0054d2d3;
        }
        for (piVar2 = (int *)*puVar9; piVar2 != (int *)0x0; piVar2 = (int *)piVar2[1]) {
          iVar5 = *piVar2;
          if (*(uint *)(iVar5 + 0x4d8) == local_68) goto LAB_0054d2d3;
        }
        iVar6 = iVar6 + 1;
        puVar9 = puVar9 + 8;
      } while (iVar6 < 2);
      iVar5 = 0;
    }
    else {
      iVar5 = (local_68 & 0xffff) * 0x64c;
      if (*(char *)(iVar5 + 0xd98 + DAT_005d19bc) == '\0') {
        iVar5 = 0;
      }
      else if (*(uint *)(iVar5 + 0xc2c + DAT_005d19bc) == local_68) {
        iVar5 = DAT_005d19bc + 0x754 + iVar5;
      }
      else {
        iVar5 = 0;
      }
    }
LAB_0054d2d3:
    if (iVar5 != 0) {
      *(float *)(iVar5 + 0x44) = *pfVar8;
      *(undefined1 *)(iVar5 + 0x4b8) = 1;
    }
    FUN_004cb8b0();
    FUN_0040e160(0x14,4,&DAT_005a03c4,&local_58);
  }
  if (*(uint *)(param_1 + 0x1c) != *(uint *)(param_1 + 0x18)) {
    uVar7 = *(uint *)(param_1 + 0x1c) & 0x80000003;
    if ((int)uVar7 < 0) {
      uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
    }
    if (uVar7 == 0) {
      bVar4 = true;
      goto LAB_0054d321;
    }
  }
  bVar4 = false;
LAB_0054d321:
  if ((bVar4) && (iVar5 = FUN_00520240(), iVar5 != 0)) {
    return *(undefined4 *)(iVar5 + 0x98);
  }
  return 0;
}


// Reference 005a03d8
// Address 54c2f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0054c2f0(int param_1)

{
  float *pfVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 *puVar6;
  int iVar7;
  float fVar8;
  int local_8;
  
  uVar5 = *(uint *)(param_1 + 0xd8) & 0xff;
  iVar7 = *(int *)(*(int *)(*(int *)(*(int *)(param_1 + 0x124) + 4) + 0x2238) + 0xf0 +
                  ((int)*(uint *)(param_1 + 0xd8) >> 8) * 4);
  pfVar1 = (float *)(param_1 + 0x54);
  local_8 = param_1;
  puVar6 = (undefined4 *)
           FUN_005200f0(&local_8,pfVar1,*(undefined4 *)(iVar7 + 0x68 + uVar5 * 0x78),0x14,
                        (int)*(short *)(iVar7 + 2 + uVar5 * 0x78));
  iVar7 = FUN_00520240(*puVar6);
  if (iVar7 != 0) {
    *(undefined4 *)(iVar7 + 0xa4) = 4;
    FUN_004cb830(*(undefined4 *)(param_1 + 0x14),1);
    *(undefined4 *)(param_1 + 0x6c) = 0x40000000;
    uVar2 = *(undefined4 *)(param_1 + 0x58);
    uVar3 = *(undefined4 *)(param_1 + 0x5c);
    uVar4 = *(undefined4 *)(param_1 + 0x60);
    *(undefined4 *)(param_1 + 0x9c) = 2;
    *(float *)(iVar7 + 0x34) = *pfVar1;
    *(undefined4 *)(iVar7 + 0x38) = uVar2;
    *(undefined4 *)(iVar7 + 0x3c) = uVar3;
    *(undefined4 *)(iVar7 + 0x40) = uVar4;
    uVar2 = *(undefined4 *)(param_1 + 0x68);
    uVar3 = *(undefined4 *)(param_1 + 0x6c);
    uVar4 = *(undefined4 *)(param_1 + 0x70);
    *(undefined4 *)(iVar7 + 0x44) = *(undefined4 *)(param_1 + 100);
    *(undefined4 *)(iVar7 + 0x48) = uVar2;
    *(undefined4 *)(iVar7 + 0x4c) = uVar3;
    *(undefined4 *)(iVar7 + 0x50) = uVar4;
    uVar2 = *(undefined4 *)(param_1 + 0x78);
    uVar3 = *(undefined4 *)(param_1 + 0x7c);
    uVar4 = *(undefined4 *)(param_1 + 0x80);
    *(undefined4 *)(iVar7 + 0x54) = *(undefined4 *)(param_1 + 0x74);
    *(undefined4 *)(iVar7 + 0x58) = uVar2;
    *(undefined4 *)(iVar7 + 0x5c) = uVar3;
    *(undefined4 *)(iVar7 + 0x60) = uVar4;
    uVar2 = *(undefined4 *)(param_1 + 0x88);
    uVar3 = *(undefined4 *)(param_1 + 0x8c);
    uVar4 = *(undefined4 *)(param_1 + 0x90);
    *(undefined4 *)(iVar7 + 100) = *(undefined4 *)(param_1 + 0x84);
    *(undefined4 *)(iVar7 + 0x68) = uVar2;
    *(undefined4 *)(iVar7 + 0x6c) = uVar3;
    *(undefined4 *)(iVar7 + 0x70) = uVar4;
    *(undefined8 *)(iVar7 + 0x74) = *(undefined8 *)(param_1 + 0x94);
    iVar7 = FUN_00520240(*(undefined4 *)(param_1 + 0xdc));
    if (iVar7 != 0) {
      FUN_00520320();
      fVar8 = *pfVar1 * _DAT_005aa478;
      *(undefined4 *)(param_1 + 0xdc) = 0;
      FUN_004bb8d0(0x41,(int)(fVar8 / DAT_005aa3dc));
      return *(undefined4 *)(param_1 + 0xac);
    }
  }
  return 0;
}


// Reference 005a03e0
// Address 54e960

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
FUN_0054e960(int param_1,uint param_2,undefined4 param_3,undefined8 *param_4,undefined4 param_5,
            int param_6)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  undefined4 uVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  int extraout_EDX;
  float fVar12;
  double dVar13;
  float local_10;
  undefined1 auStack_c [8];
  
  *(int *)(param_1 + 0x11c) = param_6;
  *(undefined4 **)(param_1 + 0x124) = &DAT_005d1a60 + param_6 * 0xf;
  *(int *)(param_1 + 0x120) = (&DAT_005d1a64)[param_6 * 0xf] + 0x22f8;
  uVar7 = param_2 & 0xff;
  iVar4 = *(int *)(*(int *)((&DAT_005d1a64)[param_6 * 0xf] + 0x2238) + 0xf0 +
                  ((int)param_2 >> 8) * 4);
  *(undefined4 *)(param_1 + 0x9c) = 1;
  *(uint *)(param_1 + 0xd8) = param_2;
  local_10 = *(float *)(param_1 + 0x28);
  if (((uint)local_10 & 1) == 0) {
    *(undefined4 *)(param_1 + 0x1c) = 0;
    *(undefined4 *)(param_1 + 0x20) = 0;
    *(undefined4 *)(param_1 + 0x24) = 0;
    *(uint *)(param_1 + 0x28) = (uint)local_10 | 1;
  }
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x20) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0xffffffff;
  *(int *)(param_1 + 0xac) = (int)*(short *)(iVar4 + 2 + uVar7 * 0x78);
  *(undefined4 *)(param_1 + 0xb0) = *(undefined4 *)(iVar4 + 0xc + uVar7 * 0x78);
  *(undefined4 *)(param_1 + 0xb4) = *(undefined4 *)(iVar4 + 0x10 + uVar7 * 0x78);
  *(undefined4 *)(param_1 + 0x110) = param_5;
  *(undefined1 *)(param_1 + 0x114) = 0;
  *(uint *)(param_1 + 0x40) =
       *(uint *)(param_1 + 0x40) ^
       (*(int *)(*(int *)(*(int *)(param_1 + 0x124) + 4) + 0x2070) * 2 ^ *(uint *)(param_1 + 0x40))
       & 2;
  *(undefined4 *)(param_1 + 0x44) =
       *(undefined4 *)(&DAT_005a0440 + *(int *)(iVar4 + 0x48 + uVar7 * 0x78) * 4);
  *(undefined4 *)(param_1 + 0x48) =
       *(undefined4 *)(&DAT_005a0400 + *(int *)(iVar4 + 0x4c + uVar7 * 0x78) * 4);
  *(undefined4 *)(param_1 + 0x4c) =
       *(undefined4 *)(&DAT_005a03fc + *(int *)(iVar4 + 0x50 + uVar7 * 0x78) * 4);
  *(undefined4 *)(param_1 + 0x50) =
       *(undefined4 *)(&DAT_005a03d0 + *(int *)(iVar4 + 0x54 + uVar7 * 0x78) * 4);
  _memset((void *)(param_1 + 0x54),0,0x48);
  fVar12 = DAT_005aa3a0;
  bVar1 = *(byte *)(iVar4 + 0x28 + uVar7 * 0x78);
  bVar6 = bVar1 & 0xf;
  if (bVar6 == 0) {
    *(undefined8 *)(param_1 + 0x54) = *param_4;
    *(undefined4 *)(param_1 + 0x5c) = *(undefined4 *)(param_4 + 1);
  }
  else if (bVar6 == 6) {
    *(undefined8 *)(param_1 + 0x54) = DAT_005a03c4;
    *(undefined4 *)(param_1 + 0x5c) = DAT_005a03cc;
  }
  else {
    iVar11 = ((int)(char)bVar1 & 0xfU) * 0x128;
    iVar8 = *(int *)(*(int *)(param_1 + 0x124) + 4);
    iVar10 = *(int *)(iVar11 + 0x654 + iVar8);
    *(float *)(param_1 + 0x54) = (float)*(int *)(iVar11 + 0x650 + iVar8) / DAT_005aa3a0;
    *(float *)(param_1 + 0x58) = (float)iVar10 / fVar12;
    *(undefined4 *)(param_1 + 0x5c) = 0;
  }
  if (*(char *)(iVar4 + 0x29 + uVar7 * 0x78) == '\x02') {
    *(undefined4 *)(*(int *)(param_1 + 0x120) + 0x12894 + *(int *)(iVar4 + 0x38 + uVar7 * 0x78) * 4)
         = 1;
  }
  *(undefined4 *)(param_1 + 0x6c) = *(undefined4 *)(iVar4 + 0x1c + uVar7 * 0x78);
  if ((*(float *)(iVar4 + 0x14 + uVar7 * 0x78) < _DAT_005aa478) ||
     (bVar1 = *(byte *)(iVar4 + 0x28 + uVar7 * 0x78), (bVar1 & 0xf) == 0)) {
    FUN_0040fdf0();
  }
  else {
    local_10 = *(float *)(((int)(char)bVar1 & 0xfU) * 0x128 + 0x6ac +
                         *(int *)(*(int *)(param_1 + 0x124) + 4));
    FUN_00402540();
    FUN_0040fdf0();
    iVar8 = FUN_00402540();
    *(float *)(param_1 + 0x6c) =
         ((float)((double)iVar8 + (double)(&DAT_005aa660)[-(iVar8 >> 0x1f)]) /
          ((float)((double)DAT_005c6630 + (double)(&DAT_005aa660)[-(DAT_005c6630 >> 0x1f)]) /
           DAT_005aa280 - DAT_005aa230) - DAT_005aa230) * DAT_005aa280 +
         *(float *)(iVar4 + 0x1c + uVar7 * 0x78);
  }
  FUN_004d4dc0();
  cVar2 = *(char *)(iVar4 + 0x29 + uVar7 * 0x78);
  if (((cVar2 == '\x04') || (cVar2 == '\x05')) || (cVar2 == '\x06')) {
    *(float *)(param_1 + 0x54) = *(float *)(iVar4 + 4 + uVar7 * 0x78) + *(float *)(param_1 + 0x54);
    fVar12 = *(float *)(iVar4 + 8 + uVar7 * 0x78);
  }
  else {
    *(float *)(param_1 + 0x54) =
         (*(float *)(iVar4 + 4 + uVar7 * 0x78) - *(float *)(param_1 + 0x8c)) +
         *(float *)(param_1 + 0x54);
    fVar12 = *(float *)(iVar4 + 8 + uVar7 * 0x78) - *(float *)(param_1 + 0x90);
  }
  *(float *)(param_1 + 0x58) = fVar12 + *(float *)(param_1 + 0x58);
  uVar5 = *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x124) + 4) + 0x4c);
  *(undefined4 *)(param_1 + 0x10c) = uVar5;
  FUN_004cb960(&local_10,uVar5,(int)*(short *)(iVar4 + 0x2a + uVar7 * 0x78),0,0,0xffffffff,0,0);
  *(float *)(param_1 + 0x14) = local_10;
  for (puVar9 = *(undefined4 **)(*(int *)(*(int *)(param_1 + 0x124) + 0x20) + 0x20);
      puVar9 != (undefined4 *)0x0; puVar9 = (undefined4 *)puVar9[1]) {
    (**(code **)(*(int *)*puVar9 + 0x28))();
  }
  iVar8 = FUN_004cb8b0();
  if (iVar8 == 0) {
    *(undefined4 *)(param_1 + 0x14) = 0;
  }
  switch(*(undefined1 *)(iVar8 + 0x4cc)) {
  case 1:
    fVar12 = *(float *)(iVar4 + 0x14 + uVar7 * 0x78);
    iVar10 = iVar8;
    goto LAB_0054eeff;
  case 2:
    FUN_00404530();
    if ((local_10 < DAT_005aa4d4 / DAT_005aa280) || (DAT_005aa2bc / DAT_005aa280 < local_10)) {
      puVar9 = (undefined4 *)FUN_004074f0();
      *(undefined4 *)(iVar8 + 0x44) = *puVar9;
      *(undefined1 *)(iVar8 + 0x4b8) = 1;
      dVar13 = _fabs((double)*(float *)(iVar8 + 0x54));
      local_10 = (float)dVar13;
      fVar12 = (float)((uint)local_10 ^ DAT_005aa690);
    }
    else {
      *(undefined1 *)(iVar8 + 0x4b8) = 1;
      *(float *)(iVar8 + 0x44) = local_10;
      dVar13 = _fabs((double)*(float *)(iVar8 + 0x54));
      fVar12 = (float)dVar13;
      local_10 = fVar12;
    }
    *(float *)(iVar8 + 0x54) = fVar12;
    *(undefined1 *)(iVar8 + 0x4b9) = 1;
    goto switchD_0054edbf_default;
  case 3:
    fVar12 = *(float *)(iVar4 + 0x14 + uVar7 * 0x78) + DAT_005aa2bc;
    break;
  case 4:
    fVar12 = DAT_005aa2bc / DAT_005aa280 + *(float *)(iVar4 + 0x14 + uVar7 * 0x78);
    break;
  case 5:
    fVar12 = *(float *)(iVar4 + 0x14 + uVar7 * 0x78) - DAT_005aa2bc / DAT_005aa280;
    break;
  default:
    goto switchD_0054edbf_default;
  }
  FUN_00404a70();
  iVar10 = extraout_EDX;
LAB_0054eeff:
  *(undefined1 *)(iVar10 + 0x4b8) = 1;
  *(float *)(iVar10 + 0x44) = fVar12;
switchD_0054edbf_default:
  puVar9 = (undefined4 *)
           FUN_0051f320(auStack_c,param_1 + 0x54,*(undefined4 *)(param_1 + 0xb4),
                        *(undefined4 *)(param_1 + 0x70),9999999,*(undefined4 *)(param_1 + 0xac));
  *(undefined4 *)(param_1 + 0xdc) = *puVar9;
  iVar10 = FUN_00520240();
  if (iVar10 != 0) {
    *(undefined4 *)(iVar10 + 0xb8) = 1;
    *(undefined4 *)(iVar10 + 0x94) = *(undefined4 *)(param_1 + 0x10);
  }
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | 1;
  if ((*(code **)(param_1 + 0x44) != (code *)0x0) &&
     (iVar10 = (**(code **)(param_1 + 0x44))(), iVar10 != 0)) {
    FUN_0054f020();
    return 0xffffffff;
  }
  sVar3 = *(short *)(iVar4 + 0x2c + uVar7 * 0x78);
  if (-1 < sVar3) {
    FUN_004bb8d0((int)sVar3,(int)((*(float *)(param_1 + 0x54) * _DAT_005aa478) / DAT_005aa3dc));
  }
  *(undefined8 *)(iVar8 + 0x614) = *(undefined8 *)(param_1 + 0x54);
  *(undefined4 *)(iVar8 + 0x61c) = *(undefined4 *)(param_1 + 0x5c);
  return 0;
}


// Reference 004dea31
// Reference 0054faa6
// Reference 0054fcc2
