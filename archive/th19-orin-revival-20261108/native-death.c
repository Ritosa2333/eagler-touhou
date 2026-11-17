// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 50c1f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0050c1f0(int param_1)

{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  undefined4 extraout_ECX;
  int iVar4;
  float fVar5;
  uint *puVar6;
  float fVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 local_c;
  uint *local_8;
  
  pfVar1 = (float *)(param_1 + 0x1278);
  if (-1 < *(int *)(param_1 + 0x6328)) {
    FUN_004bb8d0(*(int *)(param_1 + 0x6328),(int)((*pfVar1 * _DAT_005aa478) / DAT_005aa3dc));
  }
  fVar5 = *(float *)(param_1 + 0x14c4) - *(float *)(param_1 + 0x127c);
  fVar7 = *(float *)(param_1 + 0x14c0) - *pfVar1;
  if (DAT_005aa1f4 * DAT_005aa1f4 <= fVar7 * fVar7 + fVar5 * fVar5) {
    puVar6 = (uint *)(*(float *)(param_1 + 0x127c) - *(float *)(param_1 + 0x14c4));
    FUN_00407490();
  }
  else {
    puVar6 = (uint *)(DAT_005aa4d4 / DAT_005aa280);
  }
  local_8 = puVar6;
  if (-1 < *(int *)(param_1 + 0x632c)) {
    iVar4 = *(int *)(*(int *)(param_1 + 0x68e0) + 0x28);
    FUN_0050d380(*(undefined4 *)(param_1 + 0x6330));
    FUN_004cb960(&local_c,extraout_ECX,*(undefined4 *)(param_1 + 0x632c),pfVar1,local_8,0xffffffff,0
                 ,0);
    iVar2 = FUN_0040ff90();
    if ((iVar2 != -1) && (iVar2 < 0x400)) {
      *(undefined4 *)(iVar4 + 0x24 + iVar2 * 4) = local_c;
    }
  }
  puVar6 = local_8;
  if ((float)local_8 <= DAT_005aa2bc) {
    if ((float)local_8 < DAT_005aa4d4) {
      iVar4 = 0;
      do {
        puVar6 = (uint *)((float)puVar6 + DAT_005aa2bc * DAT_005aa280);
        if (0x20 < iVar4) break;
        iVar4 = iVar4 + 1;
      } while ((float)puVar6 < DAT_005aa4d4);
    }
  }
  else {
    iVar4 = 0;
    do {
      puVar6 = (uint *)((float)puVar6 - DAT_005aa2bc * DAT_005aa280);
      if (0x20 < iVar4) break;
      iVar4 = iVar4 + 1;
    } while (DAT_005aa2bc < (float)puVar6);
  }
  *(uint **)(*(int *)(*(int *)(param_1 + 0x68e0) + 8) + 0x4b7c) = puVar6;
  if (*(int *)(param_1 + 0x6258) != 0) {
    FUN_00521d60(*(int *)(param_1 + 0x6258),pfVar1,0xffffffff,DAT_005aa4d4 / DAT_005aa280,0x400ccccd
                 ,0,0,0xffffffff);
  }
  FUN_0050c6d0(pfVar1);
  *(undefined4 *)(param_1 + 0x6258) = 0;
  if ((*(uint *)(param_1 + 0x63a0) & 0x20000000) == 0) {
    uVar3 = *(uint *)(param_1 + 0x63a0) & 0x700000;
    if (uVar3 == 0) {
      uVar8 = 0;
      uVar9 = 0;
LAB_0050c41b:
      FUN_0051d4e0(*(undefined4 *)(param_1 + 0x68dc),pfVar1,*(undefined4 *)(param_1 + 0x634c),
                   *(undefined4 *)(param_1 + 0x6344),uVar8,uVar9);
    }
    else {
      if ((0xfffff < uVar3) && (uVar3 < 0x400001)) {
        uVar9 = *(undefined4 *)(param_1 + 0x14d4);
        uVar8 = 1;
        goto LAB_0050c41b;
      }
      if (uVar3 == 0x500000) {
        uVar9 = 0;
        uVar8 = 3;
        goto LAB_0050c41b;
      }
      if (uVar3 == 0x600000) {
        uVar9 = 0;
        uVar8 = 4;
        goto LAB_0050c41b;
      }
    }
    FUN_00549780(*(undefined4 *)(param_1 + 0x6350));
  }
  iVar4 = *(int *)(*(int *)(*(int *)(param_1 + 0x68e0) + 4) + 0x22bc);
  if (5 < iVar4) {
    iVar4 = iVar4 / 0xc + 1;
    if (3 < iVar4) {
      iVar4 = 3;
    }
    local_8 = (uint *)(param_1 + 0x63a0);
    if ((*local_8 & 0x8000000) == 0) {
      if (iVar4 < 1) goto LAB_0050c4ca;
    }
    else {
      iVar4 = 1;
    }
    do {
      FUN_00521c60(pfVar1);
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
LAB_0050c4ca:
  if (*(char *)(param_1 + 0x6818) != '\0') {
    FUN_0040f2f0();
    FUN_0040f220();
    FUN_00436a40((undefined1 *)(param_1 + 0x6818));
    *(undefined4 *)(*(int *)(param_1 + 0xc) + 8) = 0;
    **(undefined4 **)(param_1 + 0xc) = 0;
    FUN_0056b4e0();
    *(undefined1 *)(param_1 + 0x6818) = 0;
    if ((*(uint *)(param_1 + 0x63a0) & 0x8000000) != 0) {
      return 0;
    }
  }
  if (*(code **)(param_1 + 0x68d4) != (code *)0x0) {
    (**(code **)(param_1 + 0x68d4))(param_1);
  }
  return 1;
}


// Reference 00509b19
// Reference 00509ed0
// Reference 00509ef3
// Reference 0050cb96
// Reference 0050cd95
// Reference 0050cf8d
// Reference 004fef1d
// Address 509980

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00509980(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int **ppiVar4;
  int **ppiVar5;
  int extraout_ECX;
  uint uVar6;
  int *piVar7;
  int iVar8;
  int **ppiVar9;
  bool bVar10;
  float fVar11;
  int *piVar12;
  float *pfVar13;
  float local_24;
  float local_20;
  float local_1c;
  int *local_18;
  int local_14;
  int local_10;
  int *local_c;
  int *local_8;
  
  iVar2 = FUN_004cb8b0(*(undefined4 *)(param_1 + 0x134));
  if (iVar2 == 0) {
    *(undefined4 *)(param_1 + 0x134) = 0;
    return 0;
  }
  uVar6 = *(uint *)(param_1 + 0x5170);
  iVar2 = *(int *)(*(int *)(param_1 + 0x563c) + 0x1c);
  if ((uVar6 & 0x1000) == 0) {
    if ((*(int *)(iVar2 + 0x18) != 1) && ((uVar6 & 0x2000) != 0)) {
      *(undefined4 *)(param_1 + 0x280) = *(undefined4 *)(param_1 + 0x5178);
      FUN_004cb1e0(*(undefined4 *)(param_1 + 0x5178));
      *(uint *)(param_1 + 0x5170) = *(uint *)(param_1 + 0x5170) & 0xffffdfff;
      uVar6 = *(uint *)(param_1 + 0x5170);
      *(uint *)(param_1 + 0x516c) = *(uint *)(param_1 + 0x516c) & 0xfffffffe;
    }
  }
  else {
    uVar3 = uVar6 >> 0xd & 1;
    if (*(int *)(iVar2 + 0x18) == 1) {
      if (uVar3 == 0) {
        *(undefined4 *)(param_1 + 0x280) = *(undefined4 *)(param_1 + 0x5174);
        FUN_004cb1e0(*(undefined4 *)(param_1 + 0x5174));
        *(uint *)(param_1 + 0x5170) = *(uint *)(param_1 + 0x5170) | 0x2000;
        uVar6 = *(uint *)(param_1 + 0x5170);
        *(uint *)(param_1 + 0x516c) = *(uint *)(param_1 + 0x516c) | 1;
      }
    }
    else if (uVar3 != 0) {
      *(undefined4 *)(param_1 + 0x280) = *(undefined4 *)(param_1 + 0x5178);
      FUN_004cb1e0(*(undefined4 *)(param_1 + 0x5178));
      *(uint *)(param_1 + 0x5170) = *(uint *)(param_1 + 0x5170) & 0xffffdfff;
      *(uint *)(param_1 + 0x516c) = *(uint *)(param_1 + 0x516c) & 0xfffffffe;
      uVar6 = *(uint *)(param_1 + 0x5170);
    }
  }
  if ((*(uint *)(param_1 + 0x516c) & 0x800) != 0) {
    local_8 = (int *)0x0;
    *(uint *)(param_1 + 0x5170) = uVar6 & 0xfdffffff;
    pfVar13 = (float *)(param_1 + 0x120);
    local_c = *(int **)(*(int *)(param_1 + 0x563c) + 0x30);
    if ((*(uint *)(param_1 + 0x516c) & 0x1000) == 0) {
      fVar11 = *pfVar13 / DAT_005aa280;
      pfVar13 = (float *)0x0;
    }
    else {
      fVar11 = 0.0;
    }
    iVar2 = FUN_00520450(param_1 + 0x48,pfVar13,fVar11,&local_8,param_1 + 0x290,1,
                         *(undefined4 *)(*(int *)(param_1 + 0x55c8) + 0x6858));
    if (((iVar2 != 0) && (local_8 != (int *)0x0)) && (iVar2 = FUN_0050c1f0(), iVar2 != 0)) {
      return 1;
    }
  }
  piVar7 = (int *)(param_1 + 0x55c8);
  local_8 = (int *)FUN_00508630();
  if (local_8 != (int *)0x0) {
    if ((*(uint *)(param_1 + 0x2dc) & 1) == 0) {
      *(undefined4 *)(param_1 + 0x2d0) = 0;
      *(uint *)(param_1 + 0x2dc) = *(uint *)(param_1 + 0x2dc) | 1;
      *(undefined4 *)(param_1 + 0x2d4) = 0;
      *(undefined4 *)(param_1 + 0x2d8) = 0;
    }
    local_c = (int *)(param_1 + 0x55c8);
    *(undefined4 *)(param_1 + 0x2d0) = 0;
    *(undefined4 *)(param_1 + 0x2d4) = 0;
    *(undefined4 *)(param_1 + 0x2cc) = 0xffffffff;
    FUN_0040f2f0();
    FUN_0040f220();
    iVar2 = *piVar7;
    FUN_00436a40(local_8);
    piVar7 = local_c;
    *(undefined4 *)(*(int *)(iVar2 + 0xc) + 8) = 0;
    **(undefined4 **)(iVar2 + 0xc) = 0;
    iVar2 = FUN_0056b4e0();
    if (iVar2 != 0) {
      return 0xffffffff;
    }
  }
  *(uint *)(param_1 + 0x5170) = *(uint *)(param_1 + 0x5170) & 0xffffffdf;
  local_8 = (int *)0x0;
  if ((*(uint *)(param_1 + 0x516c) & 0x21) == 0) {
    iVar2 = param_1 + 0x120;
    iVar8 = 0;
    if (0.0 < *(float *)(param_1 + 0x120)) {
      local_c = *(int **)(*(int *)(param_1 + 0x563c) + 0x30);
      if ((*(uint *)(param_1 + 0x516c) & 0x1000) == 0) {
        fVar11 = *(float *)(param_1 + 0x120) / DAT_005aa280;
        iVar2 = 0;
      }
      else {
        fVar11 = 0.0;
      }
      iVar2 = FUN_00520450(param_1 + 0x48,iVar2,fVar11,&local_8,param_1 + 0x290,0,
                           *(undefined4 *)(*piVar7 + 0x6858));
      puVar1 = *(undefined4 **)(*(int *)(*(int *)(param_1 + 0x563c) + 0x20) + 0x20);
      iVar8 = (int)((float)iVar2 * *(float *)(*(int *)(*(int *)(param_1 + 0x563c) + 4) + 0x2270));
      while (puVar1 != (undefined4 *)0x0) {
        iVar2 = (**(code **)(*(int *)*puVar1 + 0x2c))(param_1,iVar8);
        puVar1 = (undefined4 *)puVar1[1];
        iVar8 = 0;
        if (-1 < iVar2) {
          iVar8 = iVar2;
        }
      }
    }
    if (*(code **)(param_1 + 0x5644) != (code *)0x0) {
      iVar2 = (**(code **)(param_1 + 0x5644))(iVar8);
      iVar8 = iVar8 + iVar2;
    }
    if (0 < *(int *)(param_1 + 0x50f4)) {
      iVar8 = iVar8 + *(int *)(param_1 + 0x50f4);
      *(undefined4 *)(param_1 + 0x50f4) = 0;
    }
    iVar2 = *(int *)(*(int *)(*(int *)(param_1 + 0x563c) + 4) + 0x10);
    if ((iVar2 == 2) || (iVar2 == 0)) {
      iVar8 = iVar8 / 5;
    }
    iVar2 = 0;
    if (*(int *)(DAT_005d1a50 + 0x310) == 0) {
      iVar2 = iVar8;
    }
    if (0 < iVar2) {
      if (local_8 == (int *)0x0) {
        piVar7 = (int *)(*(int *)(*(int *)(param_1 + 0x563c) + 8) + 0xb4);
        *piVar7 = *piVar7 + iVar2;
      }
      else {
        iVar8 = *(int *)(param_1 + 0x5008);
        if (iVar2 < iVar8) {
          piVar7 = (int *)(*(int *)(*(int *)(param_1 + 0x563c) + 8) + 0xb0);
          *piVar7 = *piVar7 + iVar2;
        }
        else {
          piVar7 = (int *)(*(int *)(*(int *)(param_1 + 0x563c) + 8) + 0xb0);
          *piVar7 = *piVar7 + ((int)((iVar2 - iVar8 >> 0x1f & 3U) + (iVar2 - iVar8)) >> 2) + iVar8;
        }
      }
    }
    if ((*(int *)(*(int *)(*(int *)(param_1 + 0x563c) + 0x1c) + 0x18) == 1) &&
       (*(float *)(param_1 + 0x5164) <= DAT_005aa230 && DAT_005aa230 != *(float *)(param_1 + 0x5164)
       )) {
      if ((iVar2 != 0) && (*(float *)(param_1 + 0x5164) <= 0.0)) {
        FUN_004bb8d0(0x24,(int)((*(float *)(param_1 + 0x48) * _DAT_005aa478) / DAT_005aa3dc));
      }
      iVar2 = (int)((float)iVar2 * *(float *)(param_1 + 0x5164));
    }
    if (iVar2 != 0) {
      if (((byte)*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x563c) + 0x10) + 0x7c) & 0x21) == 0x21
         ) {
        iVar2 = iVar2 / 0x1e;
      }
      if (((*(byte *)(param_1 + 0x516c) & 0x10) == 0) && (*(int *)(param_1 + 0x512c) < 1)) {
        *(int *)(param_1 + 0x501c) = *(int *)(param_1 + 0x501c) + iVar2;
        if ((*(byte *)(param_1 + 0x5020) & 1) == 0) {
          *(int *)(param_1 + 0x5008) = *(int *)(param_1 + 0x5008) - iVar2;
        }
        else {
          *(int *)(param_1 + 0x5014) = *(int *)(param_1 + 0x5014) - iVar2;
          *(int *)(param_1 + 0x5008) =
               (*(int *)(param_1 + 0x5014) + *(int *)(param_1 + 0x5018) * -7) / 7 +
               *(int *)(param_1 + 0x5018);
        }
      }
      else {
        *(int *)(param_1 + 0x501c) = *(int *)(param_1 + 0x501c) + iVar2;
      }
      FUN_00402650(0x1e);
      iVar2 = *(int *)(param_1 + 0x55c8);
      uVar6 = 0;
      *(undefined4 *)(iVar2 + 0x6240) = *(undefined4 *)(iVar2 + 0x6238);
      piVar7 = (int *)(iVar2 + 0x63b8);
      *(undefined4 *)(iVar2 + 0x6248) = 0;
      do {
        if (-1 < *piVar7) {
          local_18 = (int *)(uVar6 * 0x88);
          *(int *)(iVar2 + 0x6240) = *(int *)(iVar2 + 0x6238) - *piVar7;
          *(undefined4 *)(iVar2 + 0x6248) = *(undefined4 *)(iVar2 + 0x63b8 + (int)local_18);
          if (*(int *)(iVar2 + 0x6238) <= *(int *)(iVar2 + 0x63b8 + (int)local_18)) {
            if (*(int *)(iVar2 + 0x6864) != 0) {
              piVar7 = &local_10;
              local_14 = 999;
              local_10 = 0;
              if (-1 < DAT_0062ba8c) {
                piVar7 = &DAT_0062ba8c;
              }
              piVar12 = &local_14;
              if (DAT_0062ba8c < 1000) {
                piVar12 = piVar7;
              }
              DAT_0062ba8c = *piVar12;
              if (*(int *)(iVar2 + 0x6860) == DAT_0062ba8c) {
                local_8 = (int *)0xf423f;
                local_c = (int *)0x0;
                ppiVar9 = (int **)(*(int *)(*(int *)(iVar2 + 0x68e0) + 0x2c) + 0x7c);
                piVar7 = (int *)(*(int *)(iVar2 + 0x6864) + (int)*ppiVar9);
                *ppiVar9 = piVar7;
                ppiVar5 = &local_c;
                if (-1 < (int)piVar7) {
                  ppiVar5 = ppiVar9;
                }
                ppiVar4 = &local_8;
                if ((int)piVar7 < 1000000) {
                  ppiVar4 = ppiVar5;
                }
                *ppiVar9 = *ppiVar4;
                *(undefined4 *)(iVar2 + 0x6864) = 0;
              }
            }
            *(undefined4 *)(iVar2 + 0x6238) = *(undefined4 *)(iVar2 + 0x63b8 + (int)local_18);
            *(undefined4 *)(iVar2 + 0x63b8 + (int)local_18) = 0xffffffff;
            if ((*(uint *)(iVar2 + 0x150c) & 1) == 0) {
              *(undefined4 *)(iVar2 + 0x1500) = 0;
              *(undefined4 *)(iVar2 + 0x1504) = 0;
              *(undefined4 *)(iVar2 + 0x1508) = 0;
              *(uint *)(iVar2 + 0x150c) = *(uint *)(iVar2 + 0x150c) | 1;
            }
            *(undefined4 *)(iVar2 + 0x1500) = 0;
            *(undefined4 *)(iVar2 + 0x1504) = 0;
            *(undefined4 *)(iVar2 + 0x14fc) = 0xffffffff;
            *(uint *)(iVar2 + 0x63a0) = *(uint *)(iVar2 + 0x63a0) & 0xfffffeff;
            iVar2 = (int)local_18 + iVar2 + 0x63c0;
            if (iVar2 != 0) {
              FUN_00402650(0);
              FUN_0040f2f0();
              FUN_0040f220();
              FUN_0056bf30(iVar2);
              iVar2 = FUN_0056b4e0();
              if (iVar2 != 0) {
                return 0xffffffff;
              }
            }
          }
          break;
        }
        uVar6 = uVar6 + 1;
        piVar7 = piVar7 + 0x22;
      } while (uVar6 < 8);
      if ((((*(byte *)(param_1 + 0x516c) & 0x80) == 0) && (*(int *)(param_1 + 0x5008) < 1)) &&
         (iVar2 = FUN_0050c1f0(), iVar2 != 0)) {
        return 1;
      }
      *(uint *)(param_1 + 0x5170) = *(uint *)(param_1 + 0x5170) | 0x20;
    }
  }
  if (((*(byte *)(param_1 + 0x5020) & 2) != 0) && (iVar2 = FUN_0050c1f0(), iVar2 != 0)) {
    return 1;
  }
  if (((((((*(byte *)(param_1 + 0x516c) & 0x22) == 0) && (*(int *)(param_1 + 0x5140) < 1)) &&
        ((uVar6 = *(uint *)(param_1 + 0x5170), (uVar6 & 0x400) == 0 &&
         ((*(int *)(*(int *)(*(int *)(param_1 + 0x563c) + 8) + 0x4ab0) == 0 || ((char)uVar6 < '\0'))
         )))) && ((iVar2 = *(int *)(*(int *)(param_1 + 0x563c) + 0x2c),
                  *(char *)(iVar2 + 0x9c) == '\0' ||
                  (((uVar6 & 0x700000) == 0 || ((uVar6 & 0x80000) == 0)))))) &&
      (((*(char *)(iVar2 + 0xae) == '\0' || ((uVar6 & 0x700000) != 0x200000)) &&
       ((*(char *)(iVar2 + 0xaf) == '\0' || ((uVar6 & 0x700000) != 0x300000)))))) &&
     ((*(char *)(iVar2 + 0xb0) == '\0' || ((*(uint *)(param_1 + 0x5170) & 0x700000) != 0x400000))))
  {
    if (*(code **)(param_1 + 0x564c) == (code *)0x0) {
      if ((*(uint *)(param_1 + 0x516c) & 0x1000) == 0) {
        iVar2 = FUN_00549e00(param_1 + 0x48,0);
      }
      else {
        iVar2 = FUN_004d0990();
        local_24 = 0.0;
        local_20 = 0.0;
        if (iVar2 != 0) {
          piVar12 = (int *)(DAT_005aa2bc / DAT_005aa280 + *(float *)(iVar2 + 0x44));
          FUN_00404a70();
          local_18 = piVar12;
          FUN_00404450();
          piVar7 = local_18;
          local_c = piVar12;
          FUN_00404420();
          local_24 = (float)piVar7 * 0.0 - (float)local_c * 0.0;
          local_20 = (float)piVar7 * 0.0 + (float)local_c * 0.0;
        }
        local_20 = local_20 + *(float *)(param_1 + 0x4c);
        local_24 = local_24 + *(float *)(param_1 + 0x48);
        local_1c = *(float *)(param_1 + 0x50) + 0.0;
        iVar2 = FUN_00549570(&local_24,*(undefined4 *)(param_1 + 0x128),0);
      }
      if ((((*(uint *)(param_1 + 0x516c) & 0x200) != 0) && (iVar2 == 2)) &&
         (*(int *)(param_1 + 0x2d0) % 6 == 0)) {
        FUN_00549fb0(*(int *)(*(int *)(param_1 + 0x563c) + 4) + 0x68c,0);
      }
    }
    else {
      (**(code **)(param_1 + 0x564c))();
    }
  }
  iVar2 = FUN_004cb8b0(*(undefined4 *)(param_1 + 0x134));
  if (iVar2 == 0) {
    *(undefined4 *)(param_1 + 0x134) = 0;
    goto LAB_0050a450;
  }
  if (*(int *)(param_1 + 0x5104) != 0) {
    *(undefined1 *)(iVar2 + 0x4c6) = 0;
    *(int *)(param_1 + 0x5104) = *(int *)(param_1 + 0x5104) + -1;
    goto LAB_0050a450;
  }
  if ((*(uint *)(param_1 + 0x5170) & 0x8000) != 0) {
    uVar6 = *(uint *)(param_1 + 0x2d0) & 0x80000003;
    bVar10 = uVar6 == 0;
    if ((int)uVar6 < 0) {
      bVar10 = (uVar6 - 1 | 0xfffffffc) == 0xffffffff;
    }
    if (bVar10) {
      *(undefined4 *)(iVar2 + 0x4b0) = 0xffff00ff;
      *(undefined1 *)(iVar2 + 0x4c6) = 1;
    }
    else {
      *(undefined1 *)(iVar2 + 0x4c6) = 0;
    }
  }
  if (((*(uint *)(param_1 + 0x5170) & 0x20) == 0) || ((*(uint *)(param_1 + 0x516c) & 0x2000) != 0))
  {
    uVar6 = *(uint *)(param_1 + 0x2d0) & 0x80000003;
    if ((int)uVar6 < 0) {
      uVar6 = (uVar6 - 1 | 0xfffffffc) + 1;
    }
    if (uVar6 == 0) {
      if (((*(uint *)(param_1 + 0x5170) & 0x4080) != 0) &&
         (uVar6 = *(uint *)(*(int *)(*(int *)(param_1 + 0x563c) + 0x10) + 0x7c),
         ((byte)uVar6 & 9) != 9)) {
        iVar8 = *(int *)(*(int *)(param_1 + 0x55c8) + 0x6240);
        if ((uVar6 & 1) == 0) {
          if (499 < iVar8) goto LAB_0050a450;
        }
        else if (99 < iVar8) goto LAB_0050a450;
        *(undefined4 *)(iVar2 + 0x4b0) = 0xff0000ff;
        *(undefined1 *)(iVar2 + 0x4c6) = 1;
      }
    }
    else {
      *(undefined1 *)(iVar2 + 0x4c6) = 0;
    }
    goto LAB_0050a450;
  }
  *(undefined4 *)(iVar2 + 0x4b0) = 0xff0000ff;
  *(undefined1 *)(iVar2 + 0x4c6) = 1;
  iVar2 = *(int *)(param_1 + 0x510c);
  *(undefined4 *)(param_1 + 0x5104) = 4;
  if (iVar2 < 0) {
    if (((*(uint *)(param_1 + 0x5170) & 0x4080) != 0) &&
       (uVar6 = *(uint *)(*(int *)(*(int *)(param_1 + 0x563c) + 0x10) + 0x7c),
       ((byte)uVar6 & 9) != 9)) {
      iVar2 = *(int *)(*(int *)(param_1 + 0x55c8) + 0x6240);
      if ((uVar6 & 1) == 0) {
        if (iVar2 < 900) goto LAB_0050a3ce;
      }
      else if (iVar2 < 200) {
LAB_0050a3ce:
        iVar2 = 0x23;
        goto LAB_0050a39d;
      }
    }
    iVar2 = 0x22;
  }
LAB_0050a39d:
  FUN_004bb8d0(iVar2,(int)((*(float *)(param_1 + 0x48) * _DAT_005aa478) / DAT_005aa3dc));
  iVar2 = extraout_ECX;
LAB_0050a450:
  if (0 < *(int *)(param_1 + 0x5154)) {
    FUN_00404810(iVar2);
  }
  return 0;
}


// Reference 00508a03
