/* Entry: 0x0056281d; symbol: FUN_0056281d; body bytes: 5325 */

/* WARNING: Type propagation algorithm not settling */

void __cdecl
FUN_0056281d(uint *****param_1,uint param_2,int param_3,int param_4,undefined4 *param_5,
            char *param_6,int param_7)

{
  byte bVar1;
  longlong lVar2;
  longlong lVar3;
  undefined4 uVar4;
  __acrt_fp_class _Var5;
  int iVar6;
  uint ********ppppppppuVar7;
  uint uVar8;
  uint *********pppppppppuVar9;
  rsize_t rVar10;
  void *pvVar11;
  uint ********ppppppppuVar12;
  uint uVar13;
  char cVar14;
  uint ********ppppppppuVar15;
  uint ********ppppppppuVar16;
  uint *********pppppppppuVar17;
  uint *********pppppppppuVar18;
  uint *******pppppppuVar19;
  uint *********pppppppppuVar20;
  char *pcVar21;
  uint **********ppppppppppuVar22;
  uint *********pppppppppuVar23;
  bool bVar24;
  float10 fVar25;
  uint *****pppppuVar26;
  ulonglong uVar27;
  undefined1 local_b44 [460];
  undefined4 local_978;
  uint *******local_974 [115];
  uint local_7a8;
  byte local_7a4 [8];
  char local_79c;
  uint local_798;
  uint local_794;
  undefined4 *local_790;
  char *local_78c;
  uint *********local_788;
  uint *********local_784;
  uint *********local_780;
  undefined8 local_77c;
  undefined8 local_774;
  uint *********local_76c;
  char *local_768;
  uint local_764;
  uint *********local_760;
  uint *********local_75c;
  uint *********local_758;
  undefined1 local_751;
  uint *********local_750;
  uint *********local_74c;
  uint *********local_748;
  uint ********local_744 [115];
  uint *********local_578;
  uint ********local_574 [115];
  uint *********local_3a8;
  uint ********local_3a4;
  uint *******local_3a0 [114];
  uint *********local_1d8 [2];
  uint local_1d0 [114];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_790 = param_5;
  local_768 = param_6;
  FUN_00565f50((uint *)local_7a4);
  local_79c = (local_7a4[0] & 0x1f) != 0x1f;
  if ((bool)local_79c) {
    FUN_00565fd0((uint *)local_7a4);
  }
  uVar13 = param_2;
  uVar4 = 0x20;
  if (((int)param_2 < 1) && ((int)param_2 < 0)) {
    uVar4 = 0x2d;
  }
  *local_790 = uVar4;
  local_790[2] = local_768;
  FUN_00557210(&local_7a8,0,0);
  if (((uVar13 & 0x7ff00000) == 0) &&
     ((param_1 == (uint *****)0x0 && (uVar13 & 0xfffff) == 0 || ((local_7a8 & 0x1000000) != 0)))) {
    pcVar21 = "0";
    local_790[1] = 0;
LAB_00562935:
    iVar6 = FUN_00548610(local_768,param_7,(int)pcVar21);
  }
  else {
    _Var5 = __acrt_fp_classify((double *)&param_1);
    if (_Var5 == 0) {
LAB_00562978:
      param_2 = uVar13 & 0x7fffffff;
      local_794 = 0x8001f;
      FUN_00557210(&local_798,0,0);
      FUN_00557210((uint *)((int)&local_774 + 4),0x8001f,local_794);
      local_77c = CONCAT44(param_2,param_1);
      local_78c = (char *)(param_3 + 1);
      local_774 = (double)(ulonglong)(uint)local_774;
      bVar24 = (param_2 >> 0x14 & 0x7ff) != 0;
      if (bVar24) {
        iVar6 = 0x100000;
      }
      else {
        local_774 = 0.0;
        local_774._4_4_ = 0;
        iVar6 = local_774._4_4_;
      }
      local_758 = (uint *********)0x0;
      local_780 = (uint *********)param_1;
      uVar13 = (param_2 & 0xfffff) + iVar6;
      pvVar11 = (void *)(param_2 >> 0x14 & 0x7ff);
      local_74c = (uint *********)((uint)!bVar24 + (int)pvVar11);
      pppppuVar26 = param_1;
      uVar8 = param_2;
      FUN_00566030(pvVar11);
      local_774 = (double)CONCAT44(uVar8,pppppuVar26);
      fVar25 = FUN_00566320(local_774);
      local_774 = (double)fVar25;
      local_764 = (uint)fVar25;
      if ((local_764 == 0x7fffffff) || (local_764 == 0x80000000)) {
        local_764 = 0;
      }
      local_1d8[1] = local_780;
      local_1d0[0] = uVar13;
      local_3a8 = (uint *********)0x0;
      local_1d8[0] = (uint *********)((uVar13 != 0) + 1);
      if (local_74c < (uint *********)0x433) {
        if (local_74c == (uint *********)0x35) {
LAB_00562fb0:
          iVar6 = 8;
          if (uVar13 == 0) {
            iVar6 = 4;
          }
          local_774 = (double)((ulonglong)local_774 & 0xffffffff);
          uVar13 = *(uint *)((int)local_1d8 + iVar6);
          iVar6 = 0x1f;
          if (uVar13 != 0) {
            for (; uVar13 >> iVar6 == 0; iVar6 = iVar6 + -1) {
            }
          }
          if (uVar13 == 0) {
            iVar6 = 0;
          }
          else {
            iVar6 = iVar6 + 1;
          }
          pppppppppuVar20 = (uint *********)((uint)(iVar6 == 0x20) + (int)local_1d8[0]);
          if (pppppppppuVar20 < (uint *********)0x74) {
            if ((uint ********)((int)pppppppppuVar20 - 1U) != (uint ********)0xffffffff) {
              ppppppppuVar16 = (uint ********)((int)pppppppppuVar20 - 2);
              ppppppppuVar7 = (uint ********)((int)pppppppppuVar20 - 1U);
              pppppppppuVar23 = local_1d8[0];
              do {
                if (ppppppppuVar7 < pppppppppuVar23) {
                  uVar13 = local_1d0[(int)ppppppppuVar7 - 1];
                }
                else {
                  uVar13 = 0;
                }
                if (ppppppppuVar16 < pppppppppuVar23) {
                  ppppppppuVar15 = (uint ********)local_1d8[(int)ppppppppuVar7];
                }
                else {
                  ppppppppuVar15 = (uint ********)0x0;
                }
                ppppppppuVar12 = (uint ********)((int)ppppppppuVar7 - 1);
                ppppppppuVar16 = (uint ********)((int)ppppppppuVar16 - 1);
                local_1d0[(int)ppppppppuVar7 - 1] = (uint)ppppppppuVar15 >> 0x1f | uVar13 * 2;
                ppppppppuVar7 = ppppppppuVar12;
                pppppppppuVar23 = local_1d8[0];
              } while (ppppppppuVar12 != (uint ********)0xffffffff);
            }
          }
          else {
            local_578 = (uint *********)0x0;
            local_1d8[0] = (uint *********)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_574,0);
            pppppppppuVar20 = local_1d8[0];
          }
          local_1d8[0] = pppppppppuVar20;
          uVar13 = 0x434 - (int)local_74c;
          uVar8 = uVar13 >> 5;
          _memset(local_574,0,uVar8 * 4);
          local_574[uVar8] = (uint ********)(1 << ((byte)uVar13 & 0x1f));
        }
        else {
          local_574[0] = (uint ********)0x0;
          local_574[1] = (uint ********)0x100000;
          local_578 = (uint *********)0x2;
          if (uVar13 == 0) goto LAB_00562fb0;
          iVar6 = 0;
          do {
            if (*(int *)((int)local_574 + iVar6) != *(int *)((int)local_1d0 + iVar6 + -4))
            goto LAB_00562fb0;
            iVar6 = iVar6 + 4;
          } while (iVar6 != 8);
          iVar6 = 0x1f;
          if (uVar13 != 0) {
            for (; uVar13 >> iVar6 == 0; iVar6 = iVar6 + -1) {
            }
          }
          local_774 = (double)((ulonglong)local_774 & 0xffffffff);
          if (uVar13 == 0) {
            iVar6 = 0;
          }
          else {
            iVar6 = iVar6 + 1;
          }
          pppppppppuVar20 = (uint *********)((uint)(0x20U - iVar6 < 2) + (int)local_1d8[0]);
          if (pppppppppuVar20 < (uint *********)0x74) {
            if ((uint ********)((int)pppppppppuVar20 - 1U) != (uint ********)0xffffffff) {
              ppppppppuVar16 = (uint ********)((int)pppppppppuVar20 - 2);
              ppppppppuVar7 = (uint ********)((int)pppppppppuVar20 - 1U);
              pppppppppuVar23 = local_1d8[0];
              do {
                if (ppppppppuVar7 < pppppppppuVar23) {
                  uVar13 = local_1d0[(int)ppppppppuVar7 - 1];
                }
                else {
                  uVar13 = 0;
                }
                if (ppppppppuVar16 < pppppppppuVar23) {
                  ppppppppuVar15 = (uint ********)local_1d8[(int)ppppppppuVar7];
                }
                else {
                  ppppppppuVar15 = (uint ********)0x0;
                }
                ppppppppuVar12 = (uint ********)((int)ppppppppuVar7 - 1);
                ppppppppuVar16 = (uint ********)((int)ppppppppuVar16 - 1);
                local_1d0[(int)ppppppppuVar7 - 1] = (uint)ppppppppuVar15 >> 0x1e | uVar13 << 2;
                ppppppppuVar7 = ppppppppuVar12;
                pppppppppuVar23 = local_1d8[0];
              } while (ppppppppuVar12 != (uint ********)0xffffffff);
            }
          }
          else {
            local_578 = (uint *********)0x0;
            local_1d8[0] = (uint *********)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_574,0);
            pppppppppuVar20 = local_1d8[0];
          }
          local_1d8[0] = pppppppppuVar20;
          uVar13 = 0x435 - (int)local_74c;
          uVar8 = uVar13 >> 5;
          _memset(local_574,0,uVar8 * 4);
          local_574[uVar8] = (uint ********)(1 << ((byte)uVar13 & 0x1f));
        }
        local_578 = (uint *********)(uVar8 + 1);
        local_3a8 = local_578;
        _memcpy_s(&local_3a4,0x1cc,local_574,(int)local_578 * 4);
      }
      else {
        local_574[0] = (uint ********)0x0;
        local_574[1] = (uint ********)0x100000;
        local_578 = (uint *********)0x2;
        if (uVar13 == 0) {
LAB_00562ce7:
          local_76c = (uint *********)((int)local_74c - 0x432U & 0x1f);
          local_74c = (uint *********)((int)local_74c - 0x432U >> 5);
          local_788 = (uint *********)(0x20 - (int)local_76c);
          uVar13 = 1 << ((uint)local_788 & 0x1f);
          uVar8 = 0;
          if ((uint *********)0x1f < local_788) {
            uVar8 = uVar13;
          }
          local_774 = (double)((ulonglong)local_774 & 0xffffffff);
          local_784 = (uint *********)((uVar13 ^ uVar8) - 1);
          local_77c = CONCAT44(~(uint)local_784,(undefined4)local_77c);
          ppppppppuVar16 = (uint ********)local_1d8[(int)local_1d8[0]];
          iVar6 = 0x1f;
          if (ppppppppuVar16 != (uint ********)0x0) {
            for (; (uint)ppppppppuVar16 >> iVar6 == 0; iVar6 = iVar6 + -1) {
            }
          }
          if (ppppppppuVar16 == (uint ********)0x0) {
            local_75c = (uint *********)0x0;
          }
          else {
            local_75c = (uint *********)(iVar6 + 1);
          }
          if (((uint)((int)local_74c + (int)local_1d8[0]) < 0x74) &&
             (local_780 = (uint *********)
                          ((int)local_74c +
                          (int)local_1d8[0] +
                          (uint)((uint *********)(0x20 - (int)local_75c) < local_76c)),
             local_780 < (uint *********)0x74)) {
            local_760 = (uint *********)((int)local_74c + -1);
            local_750 = (uint *********)((int)local_780 + -1);
            if (local_750 != local_760) {
              ppppppppuVar16 = (uint ********)((int)local_750 - (int)local_74c);
              local_758 = (uint *********)(local_1d8 + (int)ppppppppuVar16);
              pppppppppuVar20 = local_1d8[0];
              do {
                if (ppppppppuVar16 < pppppppppuVar20) {
                  local_75c = (uint *********)local_758[1];
                }
                else {
                  local_75c = (uint *********)0x0;
                }
                if ((uint ********)((int)ppppppppuVar16 - 1U) < pppppppppuVar20) {
                  ppppppppuVar7 = *local_758;
                }
                else {
                  ppppppppuVar7 = (uint ********)0x0;
                }
                local_1d0[(int)local_750 + -1] =
                     ((uint)ppppppppuVar7 & ~(uint)local_784) >> ((byte)local_788 & 0x1f) |
                     ((uint)local_75c & (uint)local_784) << (sbyte)local_76c;
                local_750 = (uint *********)((int)local_750 + -1);
                local_758 = local_758 + -1;
                ppppppppuVar16 = (uint ********)((int)ppppppppuVar16 - 1);
                pppppppppuVar20 = local_1d8[0];
              } while (local_750 != local_760);
            }
            local_1d8[0] = local_780;
            if (local_74c != (uint *********)0x0) {
              ppppppppppuVar22 = local_1d8;
              for (pppppppppuVar20 = local_74c;
                  ppppppppppuVar22 = (uint **********)((int)ppppppppppuVar22 + 4),
                  pppppppppuVar20 != (uint *********)0x0;
                  pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + -1)) {
                *ppppppppppuVar22 = (uint *********)0x0;
              }
            }
          }
          else {
            local_578 = (uint *********)0x0;
            local_1d8[0] = (uint *********)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_574,0);
          }
          local_574[0] = (uint ********)0x2;
        }
        else {
          iVar6 = 0;
          do {
            if (*(int *)((int)local_574 + iVar6) != *(int *)((int)local_1d0 + iVar6 + -4))
            goto LAB_00562ce7;
            iVar6 = iVar6 + 4;
          } while (iVar6 != 8);
          local_760 = (uint *********)((int)local_74c - 0x431U & 0x1f);
          local_75c = (uint *********)((int)local_74c - 0x431U >> 5);
          local_784 = (uint *********)(0x20 - (int)local_760);
          uVar13 = 1 << ((uint)local_784 & 0x1f);
          uVar8 = 0;
          if ((uint *********)0x1f < local_784) {
            uVar8 = uVar13;
          }
          local_774 = (double)((ulonglong)local_774 & 0xffffffff);
          local_788 = (uint *********)((uVar13 ^ uVar8) - 1);
          local_780 = (uint *********)~(uint)local_788;
          ppppppppuVar16 = (uint ********)local_1d8[(int)local_1d8[0]];
          iVar6 = 0x1f;
          if (ppppppppuVar16 != (uint ********)0x0) {
            for (; (uint)ppppppppuVar16 >> iVar6 == 0; iVar6 = iVar6 + -1) {
            }
          }
          if (ppppppppuVar16 == (uint ********)0x0) {
            local_758 = (uint *********)0x0;
          }
          else {
            local_758 = (uint *********)(iVar6 + 1);
          }
          if ((uint)((int)local_75c + (int)local_1d8[0]) < 0x74) {
            pppppppppuVar20 =
                 (uint *********)
                 ((int)local_75c +
                 (int)local_1d8[0] + (uint)((uint *********)(0x20 - (int)local_758) < local_760));
            local_77c = CONCAT44(pppppppppuVar20,(undefined4)local_77c);
            if ((uint *********)0x73 < pppppppppuVar20) goto LAB_00562b90;
            local_76c = (uint *********)((int)local_75c + -1);
            local_750 = (uint *********)((int)pppppppppuVar20 + -1);
            if (local_750 != local_76c) {
              ppppppppuVar16 = (uint ********)((int)local_750 - (int)local_75c);
              local_74c = (uint *********)(local_1d8 + (int)ppppppppuVar16);
              pppppppppuVar23 = local_1d8[0];
              do {
                if (ppppppppuVar16 < pppppppppuVar23) {
                  local_758 = (uint *********)local_74c[1];
                }
                else {
                  local_758 = (uint *********)0x0;
                }
                if ((uint ********)((int)ppppppppuVar16 - 1U) < pppppppppuVar23) {
                  ppppppppuVar7 = *local_74c;
                }
                else {
                  ppppppppuVar7 = (uint ********)0x0;
                }
                local_1d0[(int)local_750 + -1] =
                     ((uint)ppppppppuVar7 & (uint)local_780) >> ((byte)local_784 & 0x1f) |
                     ((uint)local_758 & (uint)local_788) << (sbyte)local_760;
                local_750 = (uint *********)((int)local_750 + -1);
                local_74c = local_74c + -1;
                ppppppppuVar16 = (uint ********)((int)ppppppppuVar16 - 1);
                pppppppppuVar23 = local_1d8[0];
              } while (local_750 != local_76c);
            }
            local_1d8[0] = pppppppppuVar20;
            if (local_75c != (uint *********)0x0) {
              ppppppppppuVar22 = local_1d8;
              for (pppppppppuVar20 = local_75c;
                  ppppppppppuVar22 = (uint **********)((int)ppppppppppuVar22 + 4),
                  pppppppppuVar20 != (uint *********)0x0;
                  pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + -1)) {
                *ppppppppppuVar22 = (uint *********)0x0;
              }
            }
          }
          else {
LAB_00562b90:
            local_578 = (uint *********)0x0;
            local_1d8[0] = (uint *********)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_574,0);
          }
          local_574[0] = (uint ********)0x4;
        }
        local_574[1] = (uint ********)0x0;
        local_3a8 = (uint *********)0x1;
        local_578 = (uint *********)0x1;
        _memcpy_s(&local_3a4,0x1cc,local_574,4);
      }
      local_780 = (uint *********)0xa;
      if ((int)local_764 < 0) {
        local_774 = (double)CONCAT44(-local_764,(uint)local_774);
        uVar27 = (ulonglong)local_774;
        for (local_76c = (uint *********)(-local_764 / 10); local_774 = (double)uVar27,
            local_76c != (uint *********)0x0;
            local_76c = (uint *********)((int)local_76c - (int)local_75c)) {
          local_75c = local_76c;
          if ((uint *********)0x26 < local_76c) {
            local_75c = (uint *********)0x26;
          }
          bVar1 = (&DAT_0059867f)[(int)local_75c * 4];
          uVar13 = (uint)(byte)(&DAT_0059867e)[(int)local_75c * 4];
          local_578 = (uint *********)(uVar13 + bVar1);
          _memset(local_574,0,uVar13 * 4);
          FUN_00543e20((uint *)(local_574 + uVar13),
                       (uint *)(&DAT_00597d78 +
                               (uint)*(ushort *)(&DAT_0059867c + (int)local_75c * 4) * 4),
                       (uint)bVar1 << 2);
          pppppppppuVar20 = local_1d8[1];
          if (local_578 < (uint *********)0x2) {
            if (local_574[0] == (uint ********)0x0) {
LAB_00563603:
              local_978 = 0;
              local_1d8[0] = (uint *********)0x0;
              rVar10 = 0;
              ppppppppuVar16 = local_974;
              goto LAB_005638e1;
            }
            if ((local_574[0] == (uint ********)0x1) || (local_1d8[0] == (uint *********)0x0))
            goto LAB_005638f6;
            uVar13 = 0;
            local_77c = CONCAT44(local_1d8[0],(undefined4)local_77c);
            pppppppppuVar20 = (uint *********)0x0;
            do {
              lVar2 = ZEXT48(local_574[0]) * (ulonglong)local_1d0[(int)pppppppppuVar20 + -1] +
                      (ulonglong)uVar13;
              local_1d0[(int)pppppppppuVar20 + -1] = (uint)lVar2;
              uVar13 = (uint)((ulonglong)lVar2 >> 0x20);
              pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + 1);
            } while (pppppppppuVar20 != local_1d8[0]);
LAB_00563662:
            uVar13 = (uint)((ulonglong)lVar2 >> 0x20);
            if (uVar13 == 0) goto LAB_005638f6;
            if (local_1d8[0] < (uint *********)0x73) {
              local_1d0[(int)local_1d8[0] + -1] = uVar13;
              local_1d8[0] = (uint *********)((int)local_1d8[0] + 1);
              goto LAB_005638f6;
            }
            local_978 = 0;
            local_1d8[0] = (uint *********)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_974,0);
            bVar24 = false;
          }
          else {
            if (local_1d8[0] < (uint *********)0x2) {
              local_1d8[0] = local_578;
              _memcpy_s(local_1d8 + 1,0x1cc,local_574,(int)local_578 << 2);
              if (pppppppppuVar20 != (uint *********)0x0) {
                if ((pppppppppuVar20 != (uint *********)0x1) &&
                   (local_1d8[0] != (uint *********)0x0)) {
                  uVar13 = 0;
                  local_77c = CONCAT44(local_1d8[0],(undefined4)local_77c);
                  pppppppppuVar23 = (uint *********)0x0;
                  do {
                    uVar27 = (ulonglong)uVar13;
                    uVar8 = local_1d0[(int)pppppppppuVar23 + -1];
                    lVar2 = ZEXT48(pppppppppuVar20) * (ulonglong)uVar8 + (ulonglong)uVar13;
                    local_1d0[(int)pppppppppuVar23 + -1] = (uint)lVar2;
                    uVar13 = (uint)((ulonglong)lVar2 >> 0x20);
                    lVar2 = ZEXT48(pppppppppuVar20) * (ulonglong)uVar8 + uVar27;
                    pppppppppuVar23 = (uint *********)((int)pppppppppuVar23 + 1);
                  } while (pppppppppuVar23 != local_1d8[0]);
                  goto LAB_00563662;
                }
                goto LAB_005638f6;
              }
              goto LAB_00563603;
            }
            bVar24 = local_1d8[0] <= local_578;
            local_758 = local_574;
            if (bVar24) {
              local_758 = (uint *********)(local_1d8 + 1);
            }
            pppppppppuVar20 = (uint *********)(local_1d8 + 1);
            if (bVar24) {
              pppppppppuVar20 = local_574;
            }
            local_77c = CONCAT44(pppppppppuVar20,(undefined4)local_77c);
            local_788 = local_578;
            local_784 = local_1d8[0];
            if (!bVar24) {
              local_788 = local_1d8[0];
              local_784 = local_578;
            }
            local_1d8[0] = (uint *********)0x0;
            pppppppppuVar23 = (uint *********)0x0;
            local_748 = (uint *********)0x0;
            if (local_784 != (uint *********)0x0) {
              do {
                if (local_758[(int)pppppppppuVar23] == (uint ********)0x0) {
                  if (pppppppppuVar23 == local_1d8[0]) {
                    local_744[(int)pppppppppuVar23] = (uint ********)0x0;
                    local_1d8[0] = (uint *********)((int)pppppppppuVar23 + 1);
                    local_748 = local_1d8[0];
                  }
                }
                else {
                  local_74c = (uint *********)0x0;
                  local_750 = (uint *********)0x0;
                  pppppppppuVar17 = pppppppppuVar23;
                  if (local_788 != (uint *********)0x0) {
                    do {
                      if (pppppppppuVar17 == (uint *********)0x73) break;
                      if (pppppppppuVar17 == local_1d8[0]) {
                        local_744[(int)pppppppppuVar17] = (uint ********)0x0;
                        local_748 = (uint *********)((int)local_750 + 1 + (int)pppppppppuVar23);
                      }
                      ppppppppuVar16 = pppppppppuVar20[(int)local_750];
                      ppppppppuVar7 = local_758[(int)pppppppppuVar23];
                      uVar13 = (uint)(ZEXT48(ppppppppuVar16) * ZEXT48(ppppppppuVar7));
                      pppppppuVar19 = (uint *******)local_744[(int)pppppppppuVar17];
                      uVar8 = uVar13 + (int)local_744[(int)pppppppppuVar17];
                      local_744[(int)pppppppppuVar17] = (uint ********)(uVar8 + (int)local_74c);
                      local_74c = (uint *********)
                                  ((int)(ZEXT48(ppppppppuVar16) * ZEXT48(ppppppppuVar7) >> 0x20) +
                                   (uint)CARRY4(uVar13,(uint)pppppppuVar19) +
                                  (uint)CARRY4(uVar8,(uint)local_74c));
                      local_750 = (uint *********)((int)local_750 + 1);
                      pppppppppuVar17 = (uint *********)((int)pppppppppuVar17 + 1);
                      local_1d8[0] = local_748;
                    } while (local_750 != local_788);
                    if (local_74c != (uint *********)0x0) {
                      pppppppppuVar18 = pppppppppuVar17;
                      local_760 = local_744 + (int)pppppppppuVar17;
                      do {
                        if (pppppppppuVar18 == (uint *********)0x73) goto LAB_0056399a;
                        pppppppppuVar17 = (uint *********)((int)pppppppppuVar18 + 1);
                        if (pppppppppuVar18 == local_1d8[0]) {
                          *local_760 = (uint ********)0x0;
                          local_748 = pppppppppuVar17;
                        }
                        pppppppppuVar9 = local_760 + 1;
                        ppppppppuVar16 = *local_760;
                        *local_760 = (uint ********)((int)*local_760 + (int)local_74c);
                        local_74c = (uint *********)
                                    (uint)CARRY4((uint)ppppppppuVar16,(uint)local_74c);
                        local_1d8[0] = local_748;
                        pppppppppuVar18 = pppppppppuVar17;
                        local_760 = pppppppppuVar9;
                      } while (local_74c != (uint *********)0x0);
                    }
                  }
                  if (pppppppppuVar17 == (uint *********)0x73) {
LAB_0056399a:
                    local_1d8[0] = (uint *********)0x0;
                    _memcpy_s(local_1d8 + 1,0x1cc,local_b44,0);
                    goto LAB_005639c4;
                  }
                }
                pppppppppuVar23 = (uint *********)((int)pppppppppuVar23 + 1);
              } while (pppppppppuVar23 != local_784);
            }
            rVar10 = (int)local_1d8[0] << 2;
            ppppppppuVar16 = (uint ********)local_744;
LAB_005638e1:
            _memcpy_s(local_1d8 + 1,0x1cc,ppppppppuVar16,rVar10);
LAB_005638f6:
            bVar24 = true;
          }
          if (!bVar24) goto LAB_005639c4;
          uVar27 = (ulonglong)local_774;
        }
        local_774._4_4_ = (uint)(uVar27 >> 0x20);
        uVar13 = local_774._4_4_ % 10;
        if (uVar13 != 0) {
          uVar13 = *(uint *)(&DAT_00598714 + uVar13 * 2);
          local_774._0_4_ = (uint)uVar27;
          local_774 = (double)CONCAT44(uVar13,(uint)local_774);
          if (uVar13 == 0) {
LAB_005639c4:
            local_1d8[0] = (uint *********)0x0;
            pppppppppuVar20 = (uint *********)(local_1d8 + 1);
            goto LAB_005639e0;
          }
          if ((uVar13 != 1) && (local_1d8[0] != (uint *********)0x0)) {
            uVar8 = 0;
            pppppppppuVar20 = (uint *********)0x0;
            do {
              lVar2 = (ulonglong)uVar13 * (ulonglong)local_1d0[(int)pppppppppuVar20 + -1] +
                      (ulonglong)uVar8;
              local_1d0[(int)pppppppppuVar20 + -1] = (uint)lVar2;
              uVar8 = (uint)((ulonglong)lVar2 >> 0x20);
              pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + 1);
            } while (pppppppppuVar20 != local_1d8[0]);
            if (uVar8 != 0) {
              if ((uint *********)0x72 < local_1d8[0]) goto LAB_005639c4;
              local_1d0[(int)local_1d8[0] + -1] = uVar8;
              local_1d8[0] = (uint *********)((int)local_1d8[0] + 1);
            }
          }
        }
      }
      else {
        local_76c = (uint *********)(local_764 / 10);
        pppppppppuVar20 = local_3a8;
        if (local_76c != (uint *********)0x0) {
          local_74c = local_3a8;
          do {
            local_75c = local_76c;
            if ((uint *********)0x26 < local_76c) {
              local_75c = (uint *********)0x26;
            }
            bVar1 = (&DAT_0059867f)[(int)local_75c * 4];
            uVar13 = (uint)(byte)(&DAT_0059867e)[(int)local_75c * 4];
            local_578 = (uint *********)(uVar13 + bVar1);
            _memset(local_574,0,uVar13 * 4);
            FUN_00543e20((uint *)(local_574 + uVar13),
                         (uint *)(&DAT_00597d78 +
                                 (uint)*(ushort *)(&DAT_0059867c + (int)local_75c * 4) * 4),
                         (uint)bVar1 << 2);
            ppppppppuVar16 = local_3a4;
            if (local_578 < (uint *********)0x2) {
              if (local_574[0] == (uint ********)0x0) {
LAB_00563174:
                local_578 = (uint *********)0x0;
                local_3a8 = (uint *********)0x0;
                rVar10 = 0;
                ppppppppuVar16 = (uint ********)local_574;
                goto LAB_00563458;
              }
              pppppppppuVar20 = local_74c;
              if ((local_574[0] == (uint ********)0x1) || (local_74c == (uint *********)0x0))
              goto LAB_00563479;
              uVar13 = 0;
              pppppppppuVar20 = (uint *********)0x0;
              do {
                lVar2 = ZEXT48(local_574[0]) * ZEXT48((&local_3a4)[(int)pppppppppuVar20]) +
                        (ulonglong)uVar13;
                (&local_3a4)[(int)pppppppppuVar20] = (uint ********)(uint *******)lVar2;
                uVar13 = (uint)((ulonglong)lVar2 >> 0x20);
                pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + 1);
              } while (pppppppppuVar20 != local_74c);
LAB_005631cc:
              pppppppuVar19 = (uint *******)((ulonglong)lVar2 >> 0x20);
              pppppppppuVar20 = local_3a8;
              if (pppppppuVar19 == (uint *******)0x0) goto LAB_00563479;
              if (local_3a8 < (uint *********)0x73) {
                (&local_3a4)[(int)local_3a8] = (uint ********)pppppppuVar19;
                local_3a8 = (uint *********)((int)local_3a8 + 1);
                pppppppppuVar20 = local_3a8;
                goto LAB_00563479;
              }
              local_578 = (uint *********)0x0;
              local_3a8 = (uint *********)0x0;
              _memcpy_s(&local_3a4,0x1cc,local_574,0);
              bVar24 = false;
              local_74c = local_3a8;
            }
            else {
              if (local_74c < (uint *********)0x2) {
                local_77c = CONCAT44(local_3a4,(undefined4)local_77c);
                local_3a8 = local_578;
                _memcpy_s(&local_3a4,0x1cc,local_574,(int)local_578 << 2);
                if (ppppppppuVar16 != (uint ********)0x0) {
                  local_74c = local_3a8;
                  pppppppppuVar20 = local_74c;
                  if ((ppppppppuVar16 != (uint ********)0x1) && (local_3a8 != (uint *********)0x0))
                  {
                    uVar13 = 0;
                    pppppppppuVar20 = (uint *********)0x0;
                    do {
                      uVar27 = (ulonglong)uVar13;
                      lVar2 = (local_77c >> 0x20) * ZEXT48((&local_3a4)[(int)pppppppppuVar20]);
                      lVar3 = lVar2 + (ulonglong)uVar13;
                      (&local_3a4)[(int)pppppppppuVar20] = (uint ********)(uint *******)lVar3;
                      uVar13 = (uint)((ulonglong)lVar3 >> 0x20);
                      lVar2 = lVar2 + uVar27;
                      pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + 1);
                    } while (pppppppppuVar20 != local_3a8);
                    goto LAB_005631cc;
                  }
                  goto LAB_00563479;
                }
                goto LAB_00563174;
              }
              bVar24 = local_74c <= local_578;
              local_760 = local_574;
              if (bVar24) {
                local_760 = &local_3a4;
              }
              pppppppppuVar20 = &local_3a4;
              if (bVar24) {
                pppppppppuVar20 = local_574;
              }
              local_77c = CONCAT44(pppppppppuVar20,(undefined4)local_77c);
              local_788 = local_578;
              local_784 = local_74c;
              if (!bVar24) {
                local_788 = local_74c;
                local_784 = local_578;
              }
              local_3a8 = (uint *********)0x0;
              pppppppppuVar23 = (uint *********)0x0;
              local_748 = (uint *********)0x0;
              if (local_784 != (uint *********)0x0) {
                do {
                  if (local_760[(int)pppppppppuVar23] == (uint ********)0x0) {
                    if (pppppppppuVar23 == local_3a8) {
                      local_744[(int)pppppppppuVar23] = (uint ********)0x0;
                      local_3a8 = (uint *********)((int)pppppppppuVar23 + 1);
                      local_748 = local_3a8;
                    }
                  }
                  else {
                    local_74c = (uint *********)0x0;
                    local_750 = (uint *********)0x0;
                    pppppppppuVar17 = pppppppppuVar23;
                    if (local_788 != (uint *********)0x0) {
                      do {
                        if (pppppppppuVar17 == (uint *********)0x73) break;
                        if (pppppppppuVar17 == local_3a8) {
                          local_744[(int)pppppppppuVar17] = (uint ********)0x0;
                          local_748 = (uint *********)((int)local_750 + 1 + (int)pppppppppuVar23);
                        }
                        ppppppppuVar7 = pppppppppuVar20[(int)local_750];
                        ppppppppuVar15 = local_760[(int)pppppppppuVar23];
                        uVar13 = (uint)(ZEXT48(ppppppppuVar7) * ZEXT48(ppppppppuVar15));
                        uVar8 = uVar13 + (int)local_74c;
                        ppppppppuVar16 = (uint ********)(local_744 + (int)pppppppppuVar17);
                        pppppppuVar19 = *ppppppppuVar16;
                        *ppppppppuVar16 = (uint *******)((int)*ppppppppuVar16 + uVar8);
                        local_74c = (uint *********)
                                    ((int)(ZEXT48(ppppppppuVar7) * ZEXT48(ppppppppuVar15) >> 0x20) +
                                     (uint)CARRY4(uVar13,(uint)local_74c) +
                                    (uint)CARRY4((uint)pppppppuVar19,uVar8));
                        local_750 = (uint *********)((int)local_750 + 1);
                        pppppppppuVar17 = (uint *********)((int)pppppppppuVar17 + 1);
                        local_3a8 = local_748;
                      } while (local_750 != local_788);
                      if (local_74c != (uint *********)0x0) {
                        pppppppppuVar18 = pppppppppuVar17;
                        local_750 = local_744 + (int)pppppppppuVar17;
                        do {
                          if (pppppppppuVar18 == (uint *********)0x73) goto LAB_0056351e;
                          pppppppppuVar17 = (uint *********)((int)pppppppppuVar18 + 1);
                          if (pppppppppuVar18 == local_3a8) {
                            *local_750 = (uint ********)0x0;
                            local_748 = pppppppppuVar17;
                          }
                          pppppppppuVar9 = local_750 + 1;
                          ppppppppuVar16 = *local_750;
                          *local_750 = (uint ********)((int)*local_750 + (int)local_74c);
                          local_74c = (uint *********)
                                      (uint)CARRY4((uint)ppppppppuVar16,(uint)local_74c);
                          local_3a8 = local_748;
                          pppppppppuVar18 = pppppppppuVar17;
                          local_750 = pppppppppuVar9;
                        } while (local_74c != (uint *********)0x0);
                      }
                    }
                    if (pppppppppuVar17 == (uint *********)0x73) {
LAB_0056351e:
                      local_3a8 = (uint *********)0x0;
                      _memcpy_s(&local_3a4,0x1cc,local_b44,0);
                      goto LAB_00563548;
                    }
                  }
                  pppppppppuVar23 = (uint *********)((int)pppppppppuVar23 + 1);
                } while (pppppppppuVar23 != local_784);
              }
              rVar10 = (int)local_3a8 << 2;
              ppppppppuVar16 = (uint ********)local_744;
LAB_00563458:
              _memcpy_s(&local_3a4,0x1cc,ppppppppuVar16,rVar10);
              pppppppppuVar20 = local_3a8;
LAB_00563479:
              local_74c = pppppppppuVar20;
              bVar24 = true;
            }
            if (!bVar24) goto LAB_00563548;
            local_76c = (uint *********)((int)local_76c - (int)local_75c);
            pppppppppuVar20 = local_74c;
          } while (local_76c != (uint *********)0x0);
        }
        if (local_764 % 10 != 0) {
          uVar13 = *(uint *)(&DAT_00598714 + (local_764 % 10) * 2);
          local_77c = CONCAT44(uVar13,(undefined4)local_77c);
          if (uVar13 == 0) {
LAB_00563548:
            local_3a8 = (uint *********)0x0;
            pppppppppuVar20 = &local_3a4;
LAB_005639e0:
            local_978 = 0;
            _memcpy_s(pppppppppuVar20,0x1cc,local_974,0);
          }
          else if ((uVar13 != 1) && (pppppppppuVar20 != (uint *********)0x0)) {
            pppppppuVar19 = (uint *******)0x0;
            pppppppppuVar23 = (uint *********)0x0;
            do {
              lVar2 = (ulonglong)uVar13 * ZEXT48((&local_3a4)[(int)pppppppppuVar23]) +
                      ZEXT48(pppppppuVar19);
              (&local_3a4)[(int)pppppppppuVar23] = (uint ********)(uint *******)lVar2;
              pppppppuVar19 = (uint *******)((ulonglong)lVar2 >> 0x20);
              pppppppppuVar23 = (uint *********)((int)pppppppppuVar23 + 1);
            } while (pppppppppuVar23 != pppppppppuVar20);
            if (pppppppuVar19 != (uint *******)0x0) {
              if ((uint *********)0x72 < local_3a8) goto LAB_00563548;
              (&local_3a4)[(int)local_3a8] = (uint ********)pppppppuVar19;
              local_3a8 = (uint *********)((int)local_3a8 + 1);
            }
          }
        }
      }
      pcVar21 = local_768;
      local_74c = (uint *********)local_768;
      if (local_1d8[0] != (uint *********)0x0) {
        uVar13 = 0;
        pppppppppuVar20 = (uint *********)0x0;
        do {
          lVar2 = (ulonglong)local_1d0[(int)pppppppppuVar20 + -1] * 10 + (ulonglong)uVar13;
          local_1d0[(int)pppppppppuVar20 + -1] = (uint)lVar2;
          uVar13 = (uint)((ulonglong)lVar2 >> 0x20);
          pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + 1);
        } while (pppppppppuVar20 != local_1d8[0]);
        if (uVar13 != 0) {
          if (local_1d8[0] < (uint *********)0x73) {
            local_1d0[(int)local_1d8[0] + -1] = uVar13;
            local_1d8[0] = (uint *********)((int)local_1d8[0] + 1);
          }
          else {
            local_978 = 0;
            local_1d8[0] = (uint *********)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_974,0);
          }
        }
      }
      uVar27 = FUN_0054e9f0((uint *)local_1d8,&local_3a8);
      if ((int)uVar27 == 10) {
        local_764 = local_764 + 1;
        local_74c = (uint *********)(pcVar21 + 1);
        *pcVar21 = '1';
        if (local_3a8 != (uint *********)0x0) {
          pppppppuVar19 = (uint *******)0x0;
          pppppppppuVar20 = (uint *********)0x0;
          do {
            lVar2 = ZEXT48((&local_3a4)[(int)pppppppppuVar20]) * 10 + ZEXT48(pppppppuVar19);
            (&local_3a4)[(int)pppppppppuVar20] = (uint ********)(uint *******)lVar2;
            pppppppuVar19 = (uint *******)((ulonglong)lVar2 >> 0x20);
            pppppppppuVar20 = (uint *********)((int)pppppppppuVar20 + 1);
          } while (pppppppppuVar20 != local_3a8);
          if (pppppppuVar19 != (uint *******)0x0) {
            if (local_3a8 < (uint *********)0x73) {
              (&local_3a4)[(int)local_3a8] = (uint ********)pppppppuVar19;
              local_3a8 = (uint *********)((int)local_3a8 + 1);
            }
            else {
              local_978 = 0;
              local_3a8 = (uint *********)0x0;
              _memcpy_s(&local_3a4,0x1cc,local_974,0);
            }
          }
        }
LAB_00563b48:
        pcVar21 = pcVar21 + 1;
        uVar13 = local_764;
      }
      else {
        if ((int)uVar27 != 0) {
          local_74c = (uint *********)(pcVar21 + 1);
          *pcVar21 = (char)uVar27 + '0';
          goto LAB_00563b48;
        }
        uVar13 = local_764 - 1;
      }
      local_790[1] = uVar13;
      if (((-1 < (int)uVar13) && (local_78c < 0x80000000)) && (param_4 == 0)) {
        local_78c = (char *)((int)local_78c + uVar13);
      }
      if (param_7 - 1U < local_78c) {
        local_78c = (char *)(param_7 - 1U);
      }
      local_78c = local_768 + (int)local_78c;
      local_751 = 0;
      if (pcVar21 != local_78c) {
        do {
          pppppppppuVar20 = local_74c;
          if (local_1d8[0] == (uint *********)0x0) break;
          uVar13 = 0;
          pppppppppuVar23 = (uint *********)0x0;
          do {
            lVar2 = (ulonglong)local_1d0[(int)pppppppppuVar23 + -1] * 1000000000 + (ulonglong)uVar13
            ;
            local_1d0[(int)pppppppppuVar23 + -1] = (uint)lVar2;
            uVar13 = (uint)((ulonglong)lVar2 >> 0x20);
            pppppppppuVar23 = (uint *********)((int)pppppppppuVar23 + 1);
          } while (pppppppppuVar23 != local_1d8[0]);
          if (uVar13 != 0) {
            if (local_1d8[0] < (uint *********)0x73) {
              local_1d0[(int)local_1d8[0] + -1] = uVar13;
              local_1d8[0] = (uint *********)((int)local_1d8[0] + 1);
            }
            else {
              local_978 = 0;
              local_1d8[0] = (uint *********)0x0;
              _memcpy_s(local_1d8 + 1,0x1cc,local_974,0);
            }
          }
          uVar27 = FUN_0054e9f0((uint *)local_1d8,&local_3a8);
          uVar13 = 8;
          do {
            cVar14 = (char)((uVar27 & 0xffffffff) % ZEXT48(local_780)) + '0';
            if (uVar13 < (uint)((int)local_78c - (int)pppppppppuVar20)) {
              *(char *)((int)pppppppppuVar20 + uVar13) = cVar14;
            }
            else if (cVar14 != '0') {
              local_751 = 1;
            }
            uVar27 = (ulonglong)CONCAT14(local_751,(int)((uVar27 & 0xffffffff) / ZEXT48(local_780)))
            ;
            uVar13 = uVar13 - 1;
          } while (uVar13 != 0xffffffff);
          uVar13 = (int)local_78c - (int)pppppppppuVar20;
          if (9 < (uint)((int)local_78c - (int)pppppppppuVar20)) {
            uVar13 = 9;
          }
          pcVar21 = (char *)((int)pppppppppuVar20 + uVar13);
          local_74c = (uint *********)pcVar21;
        } while (pcVar21 != local_78c);
      }
      *pcVar21 = '\0';
      FUN_00557210((uint *)((int)&local_774 + 4),local_798,local_794);
      goto LAB_00563cb7;
    }
    local_790[1] = 1;
    if (_Var5 != 1) {
      if (_Var5 == 2) {
        pcVar21 = "1#QNAN";
      }
      else if (_Var5 == 3) {
        pcVar21 = "1#SNAN";
      }
      else {
        if (_Var5 != 4) goto LAB_00562978;
        pcVar21 = "1#IND";
      }
      goto LAB_00562935;
    }
    iVar6 = FUN_00548610(local_768,param_7,0x59d870);
  }
  if (iVar6 != 0) {
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
LAB_00563cb7:
  if (local_79c != '\0') {
    FUN_00565f70((uint *)local_7a4);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

