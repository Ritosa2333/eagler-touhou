/* Entry: 0x0054c685; symbol: FUN_0054c685; body bytes: 921 */

/* WARNING: Removing unreachable block (ram,0x0054c841) */
/* WARNING: Removing unreachable block (ram,0x0054c98f) */

undefined4 __cdecl
FUN_0054c685(uint param_1,uint param_2,int param_3,bool param_4,char param_5,
            floating_point_value *param_6)

{
  floating_point_value fVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  double *pdVar10;
  float *pfVar11;
  undefined4 uVar12;
  byte bVar13;
  uint uVar14;
  uint uVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  ulonglong uVar19;
  ulonglong uVar20;
  undefined8 uVar21;
  int local_c;
  
  uVar18 = 0;
  uVar19 = CONCAT44(param_2,param_1);
  if (param_2 == 0) {
    iVar6 = 0x1f;
    if (param_1 != 0) {
      for (; param_1 >> iVar6 == 0; iVar6 = iVar6 + -1) {
      }
    }
    if (param_1 == 0) {
      iVar6 = 0;
    }
    else {
      iVar6 = iVar6 + 1;
    }
  }
  else {
    iVar6 = 0x1f;
    if (param_2 != 0) {
      for (; param_2 >> iVar6 == 0; iVar6 = iVar6 + -1) {
      }
    }
    if (param_2 == 0) {
      iVar6 = 0;
    }
    else {
      iVar6 = iVar6 + 1;
    }
    iVar6 = iVar6 + 0x20;
  }
  fVar1 = param_6[4];
  iVar16 = 0x18;
  if (fVar1 != (floating_point_value)0x0) {
    iVar16 = 0x35;
  }
  iVar16 = iVar16 - iVar6;
  local_c = param_3 - iVar16;
  iVar6 = 0x7f;
  if (fVar1 != (floating_point_value)0x0) {
    iVar6 = 0x3ff;
  }
  if (iVar6 < local_c) {
LAB_0054ca0b:
    __crt_strtox::assemble_floating_point_infinity(param_4,param_6);
    return 3;
  }
  iVar7 = -0x7e;
  if (fVar1 != (floating_point_value)0x0) {
    iVar7 = -0x3fe;
  }
  if (local_c < iVar7) {
    iVar7 = param_3 + -1 + iVar6;
    local_c = -iVar6;
    if (iVar7 < 0) {
      uVar17 = -iVar7;
      if (uVar17 < 0x40) {
        uVar8 = uVar17 - 1;
        uVar14 = 1 << (uVar8 & 0x1f);
        uVar9 = 0;
        if (0x1f < uVar8) {
          uVar9 = uVar14;
        }
        uVar14 = uVar14 ^ uVar9;
        if (0x3f < uVar8) {
          uVar9 = uVar14;
        }
        uVar8 = 1 << (uVar17 & 0x1f);
        uVar15 = 0;
        if (0x1f < uVar17) {
          uVar15 = uVar8;
        }
        uVar8 = uVar8 ^ uVar15;
        if (0x3f < uVar17) {
          uVar15 = uVar8;
        }
        bVar2 = (uVar14 & param_1) != 0;
        bVar3 = (uVar9 & param_2) != 0;
        if ((param_5 == '\0') ||
           ((uVar14 - 1 & param_1) != 0 || ((uVar9 - 1) + (uint)(uVar14 != 0) & param_2) != 0)) {
          bVar4 = true;
          bVar5 = true;
        }
        else {
          bVar4 = false;
          bVar5 = false;
        }
        if ((bVar2 || bVar3) || (bVar4)) {
          iVar6 = FUN_00559a20();
          if (iVar6 == 0) {
            if ((bVar2 || bVar3) && ((bVar5 || ((uVar8 & param_1) != 0 || (uVar15 & param_2) != 0)))
               ) {
              uVar18 = 1;
            }
          }
          else if (iVar6 == 0x100) {
            uVar18 = (uint)param_4;
          }
          else if (iVar6 == 0x200) {
            uVar18 = (uint)!param_4;
          }
        }
        uVar19 = __aullshr((byte)uVar17,param_2);
        uVar19 = uVar19 + uVar18;
        if (uVar19 != 0) {
          uVar20 = FUN_0054e9d8((int)param_6);
          if (uVar20 < uVar19) {
            local_c = ((param_3 - iVar7) - iVar16) + -1;
          }
          goto LAB_0054c9c9;
        }
      }
      __crt_strtox::assemble_floating_point_zero(param_4,param_6);
      return 2;
    }
    bVar13 = (byte)iVar7;
  }
  else {
    if (iVar16 < 0) {
      uVar17 = -iVar16;
      if (uVar17 < 0x40) {
        uVar8 = uVar17 - 1;
        uVar14 = 1 << (uVar8 & 0x1f);
        uVar9 = 0;
        if (0x1f < uVar8) {
          uVar9 = uVar14;
        }
        uVar14 = uVar14 ^ uVar9;
        if (0x3f < uVar8) {
          uVar9 = uVar14;
        }
        uVar8 = 1 << (uVar17 & 0x1f);
        uVar15 = 0;
        if (0x1f < uVar17) {
          uVar15 = uVar8;
        }
        uVar8 = uVar8 ^ uVar15;
        if (0x3f < uVar17) {
          uVar15 = uVar8;
        }
        bVar2 = (uVar14 & param_1) != 0;
        bVar3 = (uVar9 & param_2) != 0;
        if ((param_5 == '\0') ||
           ((uVar14 - 1 & param_1) != 0 || ((uVar9 - 1) + (uint)(uVar14 != 0) & param_2) != 0)) {
          bVar4 = true;
          bVar5 = true;
        }
        else {
          bVar4 = false;
          bVar5 = false;
        }
        if ((bVar2 || bVar3) || (bVar4)) {
          iVar6 = FUN_00559a20();
          if (iVar6 == 0) {
            if ((bVar2 || bVar3) && ((bVar5 || ((uVar8 & param_1) != 0 || (uVar15 & param_2) != 0)))
               ) {
              uVar18 = 1;
            }
          }
          else if (iVar6 == 0x100) {
            uVar18 = (uint)param_4;
          }
          else if (iVar6 == 0x200) {
            uVar18 = (uint)!param_4;
          }
        }
        uVar19 = __aullshr((byte)uVar17,param_2);
        uVar19 = uVar19 + uVar18;
      }
      else {
        uVar19 = 0;
      }
      uVar20 = FUN_0054efd4((int)param_6);
      if (uVar20 < uVar19) {
        uVar19 = uVar19 >> 1;
        local_c = local_c + 1;
        iVar6 = 0x7f;
        if (param_6[4] != (floating_point_value)0x0) {
          iVar6 = 0x3ff;
        }
        if (iVar6 < local_c) goto LAB_0054ca0b;
      }
      goto LAB_0054c9c9;
    }
    if (iVar16 < 1) goto LAB_0054c9c9;
    bVar13 = (byte)iVar16;
  }
  uVar19 = __allshl(bVar13,param_2);
LAB_0054c9c9:
  uVar20 = FUN_0054e9d8((int)param_6);
  uVar18 = (uint)(uVar19 & uVar20);
  uVar17 = (uint)((uVar19 & uVar20) >> 0x20);
  if (param_6[4] == (floating_point_value)0x0) {
    pfVar11 = __crt_strtox::floating_point_value::as_float(param_6);
    uVar12 = FUN_0054a57c(param_4,local_c,uVar18,uVar17,(uint *)pfVar11);
  }
  else {
    pdVar10 = __crt_strtox::floating_point_value::as_double(param_6);
    uVar21 = FUN_0054a5bb(param_4,local_c,uVar18,uVar17,(undefined4 *)pdVar10);
    uVar12 = (undefined4)uVar21;
  }
  return uVar12;
}

