// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 516450

undefined4 __fastcall FUN_00516450(undefined *param_1)

{
  undefined1 *puVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int *piVar7;
  undefined **ppuVar8;
  uint extraout_ECX;
  uint uVar9;
  undefined4 extraout_ECX_00;
  undefined **ppuVar10;
  undefined4 *puVar11;
  bool bVar12;
  undefined4 uVar13;
  int local_34;
  int local_30;
  undefined *local_2c;
  undefined *local_28;
  undefined4 *local_24;
  undefined1 *puStack_20;
  void *local_1c;
  undefined1 *puStack_18;
  int local_14;

  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0057748d;
  local_1c = ExceptionList;
  ExceptionList = &local_1c;
  local_2c = param_1;
  puVar1 = &stack0xfffffffc;
  if ((DAT_0062b254 == 2) && (puVar1 = &stack0xfffffffc, DAT_0062b258 == 3)) {
    local_28 = &DAT_0062b018;
    iVar4 = FUN_0046230c(&DAT_0062b018,DAT_005c3d80 ^ (uint)&stack0xfffffff0);
    if (iVar4 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(5);
    }
    if (DAT_0062b044 == 0x7fffffff) {
      DAT_0062b044 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(6);
    }
    bVar2 = param_1[0x143] & 3;
    local_14 = iVar4;
    if ((param_1[0x143] & 3) == 0) {
      uVar13 = 4;
LAB_005164fc:
      iVar4 = FUN_00563750(uVar13);
      if (iVar4 != 0) goto LAB_00516681;
      *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) & 0xfdffffff | 0x1000000;
    }
    else {
      if (bVar2 == 2) {
        uVar13 = 5;
        goto LAB_005164fc;
      }
      if (bVar2 == 3) {
        uVar13 = 6;
        goto LAB_005164fc;
      }
    }
    cVar3 = FUN_004493b0();
    if (cVar3 != '\0') {
      piVar5 = (int *)FUN_00448e70();
      iVar4 = (**(code **)(*piVar5 + 0x14))();
      if (1 < iVar4) {
        if ((*(uint *)(param_1 + 0x140) & 0x8000000) != 0) goto LAB_0051661c;
        local_14 = 0xffffffff;
        FUN_0046231d(&DAT_0062b018);
        puVar1 = puStack_20;
        goto LAB_00516575;
      }
    }
    iVar4 = DAT_005d1c28;
    *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) | 0x8000000;
    DAT_005d19a0 = &DAT_00623e40;
    DAT_005d19a4 = &DAT_00624100;
    piVar5 = (int *)(iVar4 + 0x34);
    *piVar5 = *piVar5 + 1;
    DAT_005d19a8 = &DAT_006243c0;
    DAT_005d19ac = &DAT_00624680;
    if ((*(int *)(iVar4 + 0x34) < 0x78) || ((DAT_006243d0 & 0x80001) == 0)) {
LAB_00516681:
      FUN_0046231d(&DAT_0062b018);
      ExceptionList = local_1c;
      return 3;
    }
LAB_0051661c:
    FUN_004cb830(*(undefined4 *)(DAT_005d1a34 + 0x197bc),1);
    FUN_004107f0();
    FUN_004cb830(DAT_0062bd10,1);
    DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffffe) + 4;
    FUN_0046231d(&DAT_0062b018);
    ExceptionList = local_1c;
    return 1;
  }
LAB_00516575:
  puStack_20 = puVar1;
  iVar4 = *(int *)(param_1 + 0x38);
  bVar12 = DAT_005d1c28 != 0;
  *(int *)(param_1 + 0x38) = iVar4 + 1;
  if ((bVar12) && (iVar4 + 1 < 10)) {
    FUN_00401850(&DAT_0062b018);
    FUN_00563600();
    FUN_0046231d(local_24);
  }
  if ((DAT_0062c48c & 0x60) != 0) {
    ExceptionList = local_1c;
    return 0;
  }
  uVar9 = *(uint *)(param_1 + 0x140);
  if ((uVar9 >> 7 & 1) != 0) {
    if ((uVar9 & 0x10) != 0) {
      ExceptionList = local_1c;
      return 3;
    }
    iVar4 = *(int *)(param_1 + 0x164);
    *(int *)(param_1 + 0x164) = iVar4 + 1;
    if (*(int *)(param_1 + 0x160) != 0) {
      if (iVar4 + 1 == 0x78) {
        FUN_0053a8c0();
        uVar9 = extraout_ECX;
      }
      if (*(int *)(param_1 + 0x160) != 0) goto LAB_0051685d;
    }
    if (*(int *)(param_1 + 0x164) == 0xb4) {
      FUN_004b9f00(0,0,uVar9,0x65,2);
    }
    if ((*(int *)(param_1 + 0x160) == 0) && (0xef < *(int *)(param_1 + 0x164))) {
      local_24 = &DAT_005d1a80;
      do {
        local_28 = (undefined *)*local_24;
        iVar4 = 0;
        if (0 < *(int *)(local_28 + 0x2c)) {
          puVar11 = (undefined4 *)(local_28 + 0x6a0);
          do {
            iVar6 = FUN_004cb8b0(puVar11[-0x100]);
            if (iVar6 != 0) {
              *(undefined1 *)(iVar6 + 0x4b5) = 0;
              for (iVar6 = *(int *)(iVar6 + 0x540); iVar6 != 0; iVar6 = *(int *)(iVar6 + 4)) {
                iVar6 = FUN_004cb420();
              }
            }
            iVar6 = FUN_004cb8b0(*puVar11);
            if (iVar6 != 0) {
              *(undefined1 *)(iVar6 + 0x4b5) = 0;
              for (iVar6 = *(int *)(iVar6 + 0x540); iVar6 != 0; iVar6 = *(int *)(iVar6 + 4)) {
                iVar6 = FUN_004cb420();
              }
            }
            iVar4 = iVar4 + 1;
            puVar11 = puVar11 + 1;
          } while (iVar4 < *(int *)(local_28 + 0x2c));
        }
        puVar11 = local_24;
        iVar4 = FUN_004cb8b0(*(undefined4 *)(local_28 + 0x84));
        if (iVar4 != 0) {
          *(undefined1 *)(iVar4 + 0x4b5) = 0;
          for (iVar4 = *(int *)(iVar4 + 0x540); iVar4 != 0; iVar4 = *(int *)(iVar4 + 4)) {
            iVar4 = FUN_004cb420();
          }
        }
        FUN_004de2f0(1,0);
        local_24 = puVar11 + 0xf;
      } while ((int)local_24 < 0x5d1af8);
      iVar4 = FUN_0040e510();
      param_1 = local_2c;
      if (iVar4 == 4) {
        DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffff2) + 0x10;
      }
      else {
        DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffff3) + 0xf;
      }
    }
  }
LAB_0051685d:
  if (((DAT_005d1c10 != 0) && (*(int *)(DAT_005d1c10 + 0x20) != 0)) &&
     ((param_1[0x140] & 0x10) == 0)) {
    ExceptionList = local_1c;
    return 1;
  }
  if (*(int *)(param_1 + 0x14) == 0) {
    iVar4 = FUN_00515d80();
    if (iVar4 != 0) {
      ExceptionList = local_1c;
      return 1;
    }
  }
  else if (*(int *)(param_1 + 0x14) == 0x1e) {
    FUN_00515a10();
  }
  if ((DAT_005d1a38 != 0) && ((*(byte *)(DAT_005d1a38 + 0x360c) & 8) != 0)) {
    FUN_004ecba0();
  }
  if (*(int *)(param_1 + 0x14) == 5) {
    DAT_0062eef0 = 2;
  }
  uVar9 = *(uint *)(param_1 + 0x140);
  if ((uVar9 & 4) == 0) {
    if ((DAT_0062b0ac & 0x80) != 0) {
      if (((DAT_005d19a0 != (uint *)0x0) && ((*DAT_005d19a0 & 0x8010f) != 0)) ||
         ((uVar9 & 0x70) != 0)) {
        DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffffe) + 4;
      }
      if (*(int *)(param_1 + 0x14) == 0xdd4) {
        FUN_004b9f00(0,0,uVar9,0x67,2);
      }
      else if (*(int *)(param_1 + 0x14) == 0xe10) {
        DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffffe) + 4;
      }
    }
    FUN_0050f5d0();
    if ((param_1[0x140] & 0x10) == 0) {
      if ((*(uint *)(DAT_005d1a54 + 0x140) & 0x80000) != 0) {
        FUN_004026c0(extraout_ECX_00);
      }
      piVar5 = &DAT_005d1a98;
      do {
        (**(code **)(**(int **)(*piVar5 + 0x18) + 0x48))();
        piVar5 = piVar5 + 0xf;
      } while ((int)piVar5 < 0x5d1b10);
      if ((*(uint *)(param_1 + 0x140) & 0x40000) != 0) {
        iVar4 = *(int *)(param_1 + 0x144);
        *(int *)(param_1 + 0x144) = iVar4 + -1;
        if (0 < iVar4 + -1) {
          ExceptionList = local_1c;
          return 3;
        }
        *(undefined4 *)(param_1 + 0x144) = 0;
        *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) & 0xfffbffff;
        *(undefined4 *)(*(int *)(DAT_005d1a98 + 0x18) + 0xb8) = 0;
        *(undefined4 *)(*(int *)(DAT_005d1ad4 + 0x18) + 0xb8) = 0;
      }
      if ((*(uint *)(param_1 + 0x140) & 0x60) == 0) {
        if ((*(uint *)(param_1 + 0x140) & 0x10000) != 0) {
          if (*(int *)(param_1 + 0x148) == 0) {
            piVar5 = &local_30;
            local_34 = 999;
            local_30 = 0;
            if (-1 < DAT_0062ba8c) {
              piVar5 = &DAT_0062ba8c;
            }
            piVar7 = &local_34;
            if (DAT_0062ba8c < 1000) {
              piVar7 = piVar5;
            }
            DAT_0062ba8c = *piVar7;
            if (DAT_0062ba8c < 0x2b) {
              FUN_005346b0();
            }
            FUN_004cb830(DAT_0062bd10,1);
          }
          iVar4 = *(int *)(param_1 + 0x148) + 1;
          *(int *)(param_1 + 0x148) = iVar4;
          if (iVar4 < *(int *)(param_1 + 0x14c)) {
            if (1 < iVar4) {
              ExceptionList = local_1c;
              return 3;
            }
          }
          else {
            iVar4 = FUN_00407c90();
            if (iVar4 < 0x2b) {
              FUN_005349e0(1,*DAT_005d1c04);
              do {
                iVar4 = FUN_004bae70();
              } while (iVar4 != 0);
              iVar4 = FUN_00407c90();
              if (iVar4 < 0x2b) {
                ppuVar10 = &local_2c;
                local_28 = (undefined *)0x3b9ac9ff;
                local_2c = (undefined *)0x0;
                if (-1 < (int)DAT_0062ba78) {
                  ppuVar10 = &DAT_0062ba78;
                }
                ppuVar8 = &local_28;
                if ((int)DAT_0062ba78 < 1000000000) {
                  ppuVar8 = ppuVar10;
                }
                DAT_0062ba78 = *ppuVar8;
                FUN_00413660();
              }
            }
            *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) & 0xfffeffff;
            *(undefined4 *)(param_1 + 0x148) = 0;
          }
        }
        if (((*(uint *)(param_1 + 0x140) & 0x80000) == 0) && (iVar4 = FUN_00412c50(), 0 < iVar4)) {
          ppuVar10 = &local_2c;
          local_28 = (undefined *)0x57030;
          if (-1 < (int)(DAT_0062bb20 + -1)) {
            ppuVar10 = &DAT_0062bb20;
          }
          local_2c = (undefined *)0x0;
          ppuVar8 = &local_28;
          if ((int)(DAT_0062bb20 + -1) < 0x57031) {
            ppuVar8 = ppuVar10;
          }
          DAT_0062bb20 = *ppuVar8;
          iVar4 = FUN_00412c10();
          if (iVar4 < 1) {
            FUN_00515540();
          }
        }
        FUN_004bd660();
        FUN_004bd660();
        FUN_004bd660();
        FUN_004bd660();
        ppuVar10 = &local_2c;
        local_28 = (undefined *)0x3b9ac9ff;
        local_2c = (undefined *)0x0;
        if (-1 < (int)DAT_0062ba78) {
          ppuVar10 = &DAT_0062ba78;
        }
        ppuVar8 = &local_28;
        if ((int)DAT_0062ba78 < 1000000000) {
          ppuVar8 = ppuVar10;
        }
        ppuVar10 = &local_2c;
        DAT_0062ba78 = *ppuVar8;
        if (-1 < (int)DAT_0062ba7c) {
          ppuVar10 = &DAT_0062ba7c;
        }
        local_28 = (undefined *)0x3b9ac9ff;
        local_2c = (undefined *)0x0;
        ppuVar8 = &local_28;
        if ((int)DAT_0062ba7c < 1000000000) {
          ppuVar8 = ppuVar10;
        }
        DAT_0062ba7c = *ppuVar8;
        if (DAT_005d1a64 != 0) {
          *(undefined4 *)(DAT_005d1a64 + 0x2274) =
               *(undefined4 *)(*(int *)(DAT_005d1a64 + 0x2238) + 0x28);
        }
        iVar4 = DAT_005d1aa0;
        if (DAT_005d1aa0 != 0) {
          *(undefined4 *)(DAT_005d1aa0 + 0x2274) =
               *(undefined4 *)(*(int *)(DAT_005d1aa0 + 0x2238) + 0x28);
        }
        if (DAT_005d19b4 != 0) {
          DAT_005d19b4 = DAT_005d19b4 + -1;
        }
        FUN_004026c0(iVar4);
        ExceptionList = local_1c;
        return 1;
      }
    }
    ExceptionList = local_1c;
    return 3;
  }
  *(uint *)(param_1 + 0x140) = uVar9 | 0x100;
  ExceptionList = local_1c;
  return 1;
}
