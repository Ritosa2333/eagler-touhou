/* Entry: 0x0054cbf0; symbol: FUN_0054cbf0; body bytes: 7423 */

/* WARNING: Type propagation algorithm not settling */

void FUN_0054cbf0(uint *param_1,floating_point_value *param_2)

{
  uint *puVar1;
  byte bVar2;
  uint ***pppuVar3;
  uint ***pppuVar4;
  int iVar5;
  uint ***pppuVar6;
  byte *pbVar7;
  char cVar8;
  uint ****ppppuVar9;
  uint **ppuVar10;
  uint uVar11;
  uint ****ppppuVar12;
  uint uVar13;
  uint uVar14;
  uint ****ppppuVar15;
  uint uVar16;
  uint uVar17;
  uint ****ppppuVar18;
  uint ****ppppuVar19;
  bool bVar20;
  ulonglong uVar21;
  longlong lVar22;
  undefined1 uVar23;
  rsize_t rVar24;
  floating_point_value *pfVar25;
  undefined1 local_cf8 [460];
  undefined4 local_b2c;
  uint **local_b28 [115];
  floating_point_value *local_95c;
  uint ****local_958;
  uint *local_954;
  uint ****local_950;
  uint local_94c;
  uint ****local_948;
  uint ****local_944;
  uint ****local_940;
  uint ****local_93c;
  uint ****local_938;
  uint ****local_934;
  uint ****local_930;
  uint ****local_92c;
  uint ****local_928;
  uint ****local_924;
  uint ****local_920;
  uint ****local_91c;
  uint ****local_918;
  uint ***local_914 [115];
  uint ****local_748;
  uint ***local_744 [115];
  uint ****local_578;
  uint ***local_574 [115];
  uint ****local_3a8;
  uint ***local_3a4 [115];
  uint ****local_1d8 [2];
  uint local_1d0;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_958 = (uint ****)0x19;
  if (param_2[4] != (floating_point_value)0x0) {
    local_958 = (uint ****)0x36;
  }
  local_95c = param_2;
  uVar14 = *param_1;
  uVar11 = param_1[1];
  local_954 = param_1;
  if ((int)uVar14 < 0) {
    uVar14 = 0;
  }
  uVar17 = uVar14;
  if (uVar11 <= uVar14) {
    uVar17 = uVar11;
  }
  local_944 = (uint ****)(uVar14 - uVar17);
  local_920 = (uint ****)0x0;
  ppppuVar18 = (uint ****)((int)param_1 + uVar17 + 8);
  local_938 = (uint ****)((int)param_1 + uVar11 + 8);
  ppppuVar19 = (uint ****)(param_1 + 2);
  ppppuVar9 = (uint ****)0x0;
  local_94c = (int)local_938 - (int)ppppuVar18;
  local_91c = (uint ****)0x0;
  ppppuVar15 = (uint ****)0x0;
  local_1d8[0] = (uint ****)0x0;
  local_930 = ppppuVar18;
  local_924 = ppppuVar19;
  ppppuVar12 = local_1d8[0];
  if (ppppuVar19 != ppppuVar18) {
    do {
      ppppuVar12 = local_920;
      if (ppppuVar15 == (uint ****)0x9) {
        local_924 = ppppuVar19;
        if (ppppuVar9 != (uint ****)0x0) {
          uVar14 = 0;
          ppppuVar19 = (uint ****)0x0;
          do {
            lVar22 = ZEXT48(local_1d8[(int)ppppuVar19 + 1]) * 1000000000 + (ulonglong)uVar14;
            local_1d8[(int)ppppuVar19 + 1] = (uint ****)(int)lVar22;
            uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
            ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
          } while (ppppuVar19 != ppppuVar9);
          ppppuVar9 = local_1d8[0];
          ppppuVar18 = local_930;
          if (uVar14 != 0) {
            if (local_1d8[0] < (uint ****)0x73) {
              local_1d8[(int)local_1d8[0] + 1] = (uint ****)uVar14;
              local_1d8[0] = (uint ****)((int)local_1d8[0] + 1);
              ppppuVar9 = local_1d8[0];
            }
            else {
              local_918 = (uint ****)0x0;
              local_1d8[0] = (uint ****)0x0;
              _memcpy_s(local_1d8 + 1,0x1cc,local_914,0);
              ppppuVar9 = local_1d8[0];
              ppppuVar18 = local_930;
            }
          }
        }
        ppppuVar19 = local_924;
        if (ppppuVar12 != (uint ****)0x0) {
          ppppuVar15 = (uint ****)0x0;
          if (ppppuVar9 != (uint ****)0x0) {
            do {
              puVar1 = (uint *)(local_1d8 + (int)ppppuVar15 + 1);
              uVar14 = *puVar1;
              *puVar1 = (uint)(*puVar1 + (int)ppppuVar12);
              ppppuVar12 = (uint ****)(uint)CARRY4(uVar14,(uint)ppppuVar12);
              ppppuVar15 = (uint ****)((int)ppppuVar15 + 1);
            } while (ppppuVar15 != local_1d8[0]);
            ppppuVar9 = local_1d8[0];
            if (ppppuVar12 == (uint ****)0x0) goto LAB_0054cd9e;
          }
          if (ppppuVar9 < (uint ****)0x73) {
            local_1d8[(int)ppppuVar9 + 1] = ppppuVar12;
            local_1d8[0] = (uint ****)((int)local_1d8[0] + 1);
            ppppuVar9 = local_1d8[0];
          }
          else {
            local_918 = (uint ****)0x0;
            local_1d8[0] = (uint ****)0x0;
            _memcpy_s(local_1d8 + 1,0x1cc,local_914,0);
            ppppuVar9 = local_1d8[0];
          }
        }
LAB_0054cd9e:
        local_920 = (uint ****)0x0;
        ppppuVar15 = (uint ****)0x0;
      }
      local_920 = (uint ****)((int)local_920 * 10 + (uint)*(byte *)ppppuVar19);
      ppppuVar15 = (uint ****)((int)ppppuVar15 + 1);
      ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
    } while (ppppuVar19 != ppppuVar18);
    local_92c = ppppuVar15;
    local_924 = ppppuVar19;
    ppppuVar12 = ppppuVar9;
    if (ppppuVar15 != (uint ****)0x0) {
      ppppuVar19 = local_920;
      local_91c = ppppuVar9;
      for (local_934 = (uint ****)((uint)ppppuVar15 / 10); local_934 != (uint ****)0x0;
          local_934 = (uint ****)((int)local_934 - (int)local_93c)) {
        local_93c = local_934;
        if ((uint ****)0x26 < local_934) {
          local_93c = (uint ****)0x26;
        }
        bVar2 = (&DAT_0059867f)[(int)local_93c * 4];
        uVar14 = (uint)(byte)(&DAT_0059867e)[(int)local_93c * 4];
        local_918 = (uint ****)(uVar14 + bVar2);
        _memset(local_914,0,uVar14 * 4);
        FUN_00543e20((uint *)(local_914 + uVar14),
                     (uint *)(&DAT_00597d78 +
                             (uint)*(ushort *)(&DAT_0059867c + (int)local_93c * 4) * 4),
                     (uint)bVar2 << 2);
        ppppuVar9 = local_1d8[1];
        bVar20 = true;
        if (local_918 < (uint ****)0x2) {
          if (local_914[0] == (uint ***)0x0) {
LAB_0054ce71:
            local_918 = (uint ****)0x0;
            local_1d8[0] = (uint ****)0x0;
            rVar24 = 0;
            pppuVar6 = (uint ***)local_914;
            goto LAB_0054d161;
          }
          if ((local_914[0] != (uint ***)0x1) && (local_91c != (uint ****)0x0)) {
            uVar14 = 0;
            ppppuVar9 = (uint ****)0x0;
            do {
              lVar22 = ZEXT48(local_914[0]) * ZEXT48(local_1d8[(int)ppppuVar9 + 1]) +
                       (ulonglong)uVar14;
              local_1d8[(int)ppppuVar9 + 1] = (uint ****)(int)lVar22;
              uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
              ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
            } while (ppppuVar9 != local_91c);
LAB_0054cec7:
            if (uVar14 != 0) {
              if ((uint ****)0x72 < local_1d8[0]) {
                local_918 = (uint ****)0x0;
                local_1d8[0] = (uint ****)0x0;
                _memcpy_s(local_1d8 + 1,0x1cc,local_914,0);
                bVar20 = false;
                local_91c = local_1d8[0];
                goto LAB_0054d187;
              }
              local_1d8[(int)local_1d8[0] + 1] = (uint ****)uVar14;
              local_1d8[0] = (uint ****)((int)local_1d8[0] + 1);
            }
            bVar20 = true;
            local_91c = local_1d8[0];
          }
        }
        else {
          if (local_91c < (uint ****)0x2) {
            local_928 = local_1d8[1];
            local_1d8[0] = local_918;
            _memcpy_s(local_1d8 + 1,0x1cc,local_914,(int)local_918 << 2);
            if (ppppuVar9 != (uint ****)0x0) {
              bVar20 = true;
              local_91c = local_1d8[0];
              if ((ppppuVar9 != (uint ****)0x1) && (local_1d8[0] != (uint ****)0x0)) {
                uVar14 = 0;
                ppppuVar9 = (uint ****)0x0;
                do {
                  lVar22 = ZEXT48(local_928) * ZEXT48(local_1d8[(int)ppppuVar9 + 1]) +
                           (ulonglong)uVar14;
                  local_1d8[(int)ppppuVar9 + 1] = (uint ****)(int)lVar22;
                  uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
                  ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
                  ppppuVar19 = local_920;
                } while (ppppuVar9 != local_1d8[0]);
                goto LAB_0054cec7;
              }
              goto LAB_0054d187;
            }
            goto LAB_0054ce71;
          }
          local_928 = local_914;
          if (local_91c <= local_918) {
            local_928 = (uint ****)(local_1d8 + 1);
          }
          local_950 = local_914;
          local_924 = local_918;
          local_948 = local_91c;
          if (local_91c > local_918) {
            local_924 = local_91c;
            local_950 = (uint ****)(local_1d8 + 1);
            local_948 = local_918;
          }
          local_1d8[0] = (uint ****)0x0;
          ppppuVar9 = (uint ****)0x0;
          local_748 = (uint ****)0x0;
          if (local_948 != (uint ****)0x0) {
            do {
              if (local_928[(int)ppppuVar9] == (uint ***)0x0) {
                if (ppppuVar9 == local_1d8[0]) {
                  local_744[(int)ppppuVar9] = (uint ***)0x0;
                  local_1d8[0] = (uint ****)((int)ppppuVar9 + 1);
                  local_748 = local_1d8[0];
                }
              }
              else {
                local_91c = (uint ****)0x0;
                local_940 = (uint ****)0x0;
                ppppuVar18 = ppppuVar9;
                if (local_924 != (uint ****)0x0) {
                  do {
                    if (ppppuVar18 == (uint ****)0x73) break;
                    if (ppppuVar18 == local_1d8[0]) {
                      local_744[(int)ppppuVar18] = (uint ***)0x0;
                      local_748 = (uint ****)((byte *)((int)ppppuVar9 + 1) + (int)local_940);
                    }
                    pppuVar6 = local_950[(int)local_940];
                    pppuVar3 = local_928[(int)ppppuVar9];
                    uVar14 = (uint)(ZEXT48(pppuVar6) * ZEXT48(pppuVar3));
                    ppuVar10 = (uint **)local_744[(int)ppppuVar18];
                    uVar11 = uVar14 + (int)local_744[(int)ppppuVar18];
                    local_744[(int)ppppuVar18] = (uint ***)(uVar11 + (int)local_91c);
                    local_91c = (uint ****)
                                ((int)(ZEXT48(pppuVar6) * ZEXT48(pppuVar3) >> 0x20) +
                                 (uint)CARRY4(uVar14,(uint)ppuVar10) +
                                (uint)CARRY4(uVar11,(uint)local_91c));
                    local_940 = (uint ****)((int)local_940 + 1);
                    ppppuVar18 = (uint ****)((int)ppppuVar18 + 1);
                    local_1d8[0] = local_748;
                  } while (local_940 != local_924);
                  if (local_91c != (uint ****)0x0) {
                    ppppuVar12 = ppppuVar18;
                    local_940 = local_744 + (int)ppppuVar18;
                    do {
                      if (ppppuVar12 == (uint ****)0x73) goto LAB_0054d261;
                      ppppuVar18 = (uint ****)((int)ppppuVar12 + 1);
                      if (ppppuVar12 == local_1d8[0]) {
                        *local_940 = (uint ***)0x0;
                        local_748 = ppppuVar18;
                      }
                      pppuVar6 = *local_940;
                      ppppuVar15 = local_940 + 1;
                      *local_940 = (uint ***)((int)pppuVar6 + (int)local_91c);
                      local_91c = (uint ****)(uint)CARRY4((uint)pppuVar6,(uint)local_91c);
                      local_1d8[0] = local_748;
                      ppppuVar12 = ppppuVar18;
                      local_940 = ppppuVar15;
                    } while (local_91c != (uint ****)0x0);
                  }
                }
                if (ppppuVar18 == (uint ****)0x73) {
LAB_0054d261:
                  local_1d8[0] = (uint ****)0x0;
                  _memcpy_s(local_1d8 + 1,0x1cc,local_cf8,0);
                  goto LAB_0054d28b;
                }
              }
              ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
            } while (ppppuVar9 != local_948);
          }
          rVar24 = (int)local_1d8[0] << 2;
          pppuVar6 = (uint ***)local_744;
LAB_0054d161:
          _memcpy_s(local_1d8 + 1,0x1cc,pppuVar6,rVar24);
          bVar20 = true;
          local_91c = local_1d8[0];
        }
LAB_0054d187:
        if (!bVar20) {
LAB_0054d28b:
          local_b2c = 0;
          local_1d8[0] = (uint ****)0x0;
          _memcpy_s(local_1d8 + 1,0x1cc,local_b28,0);
          ppppuVar9 = local_1d8[0];
          goto LAB_0054d203;
        }
      }
      ppppuVar9 = local_91c;
      if ((uint)local_92c % 10 != 0) {
        uVar14 = *(uint *)(&DAT_00598714 + ((uint)local_92c % 10) * 2);
        if (uVar14 == 0) {
LAB_0054d1c9:
          local_b2c = 0;
          local_1d8[0] = (uint ****)0x0;
          _memcpy_s(local_1d8 + 1,0x1cc,local_b28,0);
          ppppuVar19 = local_920;
          ppppuVar9 = local_1d8[0];
        }
        else {
          ppppuVar19 = local_920;
          if ((uVar14 != 1) && (local_91c != (uint ****)0x0)) {
            uVar11 = 0;
            ppppuVar9 = (uint ****)0x0;
            do {
              lVar22 = (ulonglong)uVar14 * ZEXT48(local_1d8[(int)ppppuVar9 + 1]) + (ulonglong)uVar11
              ;
              local_1d8[(int)ppppuVar9 + 1] = (uint ****)(int)lVar22;
              uVar11 = (uint)((ulonglong)lVar22 >> 0x20);
              ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
            } while (ppppuVar9 != local_91c);
            ppppuVar9 = local_1d8[0];
            if (uVar11 != 0) {
              if ((uint ****)0x72 < local_1d8[0]) goto LAB_0054d1c9;
              local_1d8[(int)local_1d8[0] + 1] = (uint ****)uVar11;
              local_1d8[0] = (uint ****)((int)local_1d8[0] + 1);
              ppppuVar9 = local_1d8[0];
            }
          }
        }
      }
LAB_0054d203:
      local_91c = ppppuVar9;
      ppppuVar12 = local_91c;
      if (ppppuVar19 != (uint ****)0x0) {
        ppppuVar9 = (uint ****)0x0;
        if (local_91c != (uint ****)0x0) {
          do {
            puVar1 = (uint *)(local_1d8 + (int)ppppuVar9 + 1);
            uVar14 = *puVar1;
            *puVar1 = (uint)(*puVar1 + (int)ppppuVar19);
            ppppuVar19 = (uint ****)(uint)CARRY4(uVar14,(uint)ppppuVar19);
            local_91c = local_1d8[0];
            ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
          } while (ppppuVar9 != local_1d8[0]);
          ppppuVar12 = local_1d8[0];
          if (ppppuVar19 == (uint ****)0x0) goto LAB_0054d36d;
        }
        if (local_91c < (uint ****)0x73) {
          local_1d8[(int)local_91c + 1] = ppppuVar19;
          local_1d8[0] = (uint ****)((int)local_1d8[0] + 1);
          ppppuVar12 = local_1d8[0];
        }
        else {
          local_b2c = 0;
          local_1d8[0] = (uint ****)0x0;
          _memcpy_s(local_1d8 + 1,0x1cc,local_b28,0);
          local_91c = local_1d8[0];
          ppppuVar12 = local_91c;
        }
      }
    }
  }
LAB_0054d36d:
  local_91c = ppppuVar12;
  if (local_944 != (uint ****)0x0) {
    ppppuVar19 = local_91c;
    for (local_920 = (uint ****)((uint)local_944 / 10); local_920 != (uint ****)0x0;
        local_920 = (uint ****)((int)local_920 - (int)local_924)) {
      local_924 = local_920;
      if ((uint ****)0x26 < local_920) {
        local_924 = (uint ****)0x26;
      }
      bVar2 = (&DAT_0059867f)[(int)local_924 * 4];
      uVar14 = (uint)(byte)(&DAT_0059867e)[(int)local_924 * 4];
      local_918 = (uint ****)(uVar14 + bVar2);
      _memset(local_914,0,uVar14 * 4);
      FUN_00543e20((uint *)(local_914 + uVar14),
                   (uint *)(&DAT_00597d78 +
                           (uint)*(ushort *)(&DAT_0059867c + (int)local_924 * 4) * 4),
                   (uint)bVar2 << 2);
      ppppuVar9 = local_1d8[1];
      if (local_918 < (uint ****)0x2) {
        if (local_914[0] == (uint ***)0x0) {
LAB_0054d415:
          local_b2c = 0;
          local_1d8[0] = (uint ****)0x0;
          rVar24 = 0;
          pppuVar6 = local_b28;
          goto LAB_0054d6b2;
        }
        if ((local_914[0] == (uint ***)0x1) || (uVar14 = 0, ppppuVar19 == (uint ****)0x0)) {
LAB_0054d435:
          bVar20 = true;
        }
        else {
          ppppuVar9 = (uint ****)0x0;
          do {
            lVar22 = ZEXT48(local_914[0]) * ZEXT48(local_1d8[(int)ppppuVar9 + 1]) +
                     (ulonglong)uVar14;
            local_1d8[(int)ppppuVar9 + 1] = (uint ****)(int)lVar22;
            uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
            ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
          } while (ppppuVar9 != ppppuVar19);
LAB_0054d461:
          if (uVar14 != 0) {
            if ((uint ****)0x72 < local_1d8[0]) {
              local_b2c = 0;
              local_1d8[0] = (uint ****)0x0;
              _memcpy_s(local_1d8 + 1,0x1cc,local_b28,0);
              bVar20 = false;
              ppppuVar19 = local_1d8[0];
              local_91c = local_1d8[0];
              goto LAB_0054d6d8;
            }
            local_1d8[(int)local_1d8[0] + 1] = (uint ****)uVar14;
            local_1d8[0] = (uint ****)((int)local_1d8[0] + 1);
          }
          bVar20 = true;
          ppppuVar19 = local_1d8[0];
          local_91c = local_1d8[0];
        }
      }
      else {
        if (ppppuVar19 < (uint ****)0x2) {
          local_1d8[0] = local_918;
          _memcpy_s(local_1d8 + 1,0x1cc,local_914,(int)local_918 << 2);
          if (ppppuVar9 != (uint ****)0x0) {
            local_91c = local_1d8[0];
            ppppuVar19 = local_1d8[0];
            if ((ppppuVar9 == (uint ****)0x1) || (uVar14 = 0, local_1d8[0] == (uint ****)0x0))
            goto LAB_0054d435;
            ppppuVar19 = (uint ****)0x0;
            do {
              lVar22 = ZEXT48(ppppuVar9) * ZEXT48(local_1d8[(int)ppppuVar19 + 1]) +
                       (ulonglong)uVar14;
              local_1d8[(int)ppppuVar19 + 1] = (uint ****)(int)lVar22;
              uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
              ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
            } while (ppppuVar19 != local_1d8[0]);
            goto LAB_0054d461;
          }
          goto LAB_0054d415;
        }
        local_940 = local_914;
        if (ppppuVar19 <= local_918) {
          local_940 = (uint ****)(local_1d8 + 1);
        }
        local_92c = local_914;
        local_93c = local_918;
        local_948 = ppppuVar19;
        if (ppppuVar19 > local_918) {
          local_93c = ppppuVar19;
          local_92c = (uint ****)(local_1d8 + 1);
          local_948 = local_918;
        }
        local_1d8[0] = (uint ****)0x0;
        ppppuVar19 = (uint ****)0x0;
        local_748 = (uint ****)0x0;
        if (local_948 != (uint ****)0x0) {
          do {
            if (local_940[(int)ppppuVar19] == (uint ***)0x0) {
              if (ppppuVar19 == local_1d8[0]) {
                local_744[(int)ppppuVar19] = (uint ***)0x0;
                local_1d8[0] = (uint ****)((int)ppppuVar19 + 1);
                local_748 = local_1d8[0];
              }
            }
            else {
              local_928 = (uint ****)0x0;
              local_934 = (uint ****)0x0;
              ppppuVar9 = ppppuVar19;
              if (local_93c != (uint ****)0x0) {
                do {
                  if (ppppuVar9 == (uint ****)0x73) break;
                  if (ppppuVar9 == local_1d8[0]) {
                    local_744[(int)ppppuVar9] = (uint ***)0x0;
                    local_748 = (uint ****)((byte *)((int)ppppuVar19 + 1) + (int)local_934);
                  }
                  pppuVar3 = local_92c[(int)local_934];
                  pppuVar4 = local_940[(int)ppppuVar19];
                  uVar14 = (uint)(ZEXT48(pppuVar3) * ZEXT48(pppuVar4));
                  pbVar7 = (byte *)(uVar14 + (int)local_928);
                  pppuVar6 = (uint ***)(local_744 + (int)ppppuVar9);
                  ppuVar10 = *pppuVar6;
                  *pppuVar6 = (uint **)(pbVar7 + (int)*pppuVar6);
                  local_928 = (uint ****)
                              ((int)(ZEXT48(pppuVar3) * ZEXT48(pppuVar4) >> 0x20) +
                               (uint)CARRY4(uVar14,(uint)local_928) +
                              (uint)CARRY4((uint)ppuVar10,(uint)pbVar7));
                  local_934 = (uint ****)((int)local_934 + 1);
                  ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
                  local_1d8[0] = local_748;
                } while (local_934 != local_93c);
                if (local_928 != (uint ****)0x0) {
                  pppuVar6 = (uint ***)(local_744 + (int)ppppuVar9);
                  ppppuVar18 = local_928;
                  ppppuVar12 = ppppuVar9;
                  do {
                    if (ppppuVar12 == (uint ****)0x73) goto LAB_0054d6f6;
                    ppppuVar9 = (uint ****)((int)ppppuVar12 + 1);
                    if (ppppuVar12 == local_1d8[0]) {
                      *pppuVar6 = (uint **)0x0;
                      local_748 = ppppuVar9;
                    }
                    ppuVar10 = *pppuVar6;
                    *pppuVar6 = (uint **)((int)*pppuVar6 + (int)ppppuVar18);
                    ppppuVar18 = (uint ****)(uint)CARRY4((uint)ppuVar10,(uint)ppppuVar18);
                    local_1d8[0] = local_748;
                    pppuVar6 = pppuVar6 + 1;
                    ppppuVar12 = ppppuVar9;
                  } while (ppppuVar18 != (uint ****)0x0);
                }
              }
              if (ppppuVar9 == (uint ****)0x73) {
LAB_0054d6f6:
                local_1d8[0] = (uint ****)0x0;
                _memcpy_s(local_1d8 + 1,0x1cc,local_cf8,0);
                goto LAB_0054d7fe;
              }
            }
            ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
          } while (ppppuVar19 != local_948);
        }
        rVar24 = (int)local_1d8[0] << 2;
        pppuVar6 = (uint ***)local_744;
LAB_0054d6b2:
        _memcpy_s(local_1d8 + 1,0x1cc,pppuVar6,rVar24);
        bVar20 = true;
        ppppuVar19 = local_1d8[0];
        local_91c = local_1d8[0];
      }
LAB_0054d6d8:
      if (!bVar20) goto LAB_0054d7fe;
    }
    if ((uint)local_944 % 10 != 0) {
      uVar14 = *(uint *)(&DAT_00598714 + ((uint)local_944 % 10) * 2);
      if (uVar14 == 0) {
        local_b2c = 0;
        local_1d8[0] = (uint ****)0x0;
        _memcpy_s(local_1d8 + 1,0x1cc,local_b28,0);
LAB_0054d783:
        local_91c = local_1d8[0];
      }
      else if ((uVar14 != 1) && (ppppuVar19 != (uint ****)0x0)) {
        uVar11 = 0;
        ppppuVar9 = (uint ****)0x0;
        do {
          lVar22 = (ulonglong)uVar14 * ZEXT48(local_1d8[(int)ppppuVar9 + 1]) + (ulonglong)uVar11;
          local_1d8[(int)ppppuVar9 + 1] = (uint ****)(int)lVar22;
          uVar11 = (uint)((ulonglong)lVar22 >> 0x20);
          ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
        } while (ppppuVar9 != ppppuVar19);
        if (uVar11 == 0) goto LAB_0054d783;
        if ((uint ****)0x72 < local_1d8[0]) {
LAB_0054d7fe:
          local_1d8[0] = (uint ****)0x0;
          local_b2c = 0;
          _memcpy_s(local_1d8 + 1,0x1cc,local_b28,0);
          __crt_strtox::assemble_floating_point_infinity(SUB41(local_954[0xc2],0),local_95c);
          goto LAB_0054e8e0;
        }
        local_1d8[(int)local_1d8[0] + 1] = (uint ****)uVar11;
        local_91c = (uint ****)((int)local_1d8[0] + 1);
        local_1d8[0] = local_91c;
      }
    }
  }
  local_940 = local_91c;
  if (local_91c != (uint ****)0x0) {
    pppuVar6 = (uint ***)local_1d8[(int)local_91c];
    local_92c = (uint ****)0x0;
    iVar5 = 0x1f;
    if (pppuVar6 != (uint ***)0x0) {
      for (; (uint)pppuVar6 >> iVar5 == 0; iVar5 = iVar5 + -1) {
      }
    }
    if (pppuVar6 == (uint ***)0x0) {
      iVar5 = 0;
    }
    else {
      iVar5 = iVar5 + 1;
    }
    local_940 = (uint ****)((int)((int)local_91c + -1) * 0x20 + iVar5);
  }
  if ((local_958 <= local_940) || (local_94c == 0)) {
    bVar20 = local_94c != 0;
LAB_0054e8bb:
    FUN_0054ca1e((uint *)local_1d8,(uint)local_940,SUB41(local_954[0xc2],0),bVar20,local_95c);
    goto LAB_0054e8e0;
  }
  ppppuVar19 = (uint ****)0x0;
  local_3a8 = (uint ****)0x0;
  ppppuVar9 = (uint ****)0x0;
  local_924 = (uint ****)0x0;
  if (local_930 != local_938) {
    do {
      ppppuVar18 = local_924;
      ppppuVar12 = local_930;
      if (ppppuVar9 == (uint ****)0x9) {
        if (ppppuVar19 != (uint ****)0x0) {
          ppuVar10 = (uint **)0x0;
          ppppuVar9 = (uint ****)0x0;
          do {
            lVar22 = ZEXT48(local_3a4[(int)ppppuVar9]) * 1000000000 + ZEXT48(ppuVar10);
            local_3a4[(int)ppppuVar9] = (uint ***)(uint **)lVar22;
            ppuVar10 = (uint **)((ulonglong)lVar22 >> 0x20);
            ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
          } while (ppppuVar9 != ppppuVar19);
          ppppuVar19 = local_3a8;
          if (ppuVar10 != (uint **)0x0) {
            if (local_3a8 < (uint ****)0x73) {
              local_3a4[(int)local_3a8] = (uint ***)ppuVar10;
              local_3a8 = (uint ****)((int)local_3a8 + 1);
              ppppuVar19 = local_3a8;
            }
            else {
              local_b2c = 0;
              local_3a8 = (uint ****)0x0;
              _memcpy_s(local_3a4,0x1cc,local_b28,0);
              ppppuVar19 = local_3a8;
            }
          }
        }
        ppppuVar12 = local_930;
        if (ppppuVar18 != (uint ****)0x0) {
          ppppuVar9 = (uint ****)0x0;
          if (ppppuVar19 != (uint ****)0x0) {
            do {
              pppuVar6 = (uint ***)(local_3a4 + (int)ppppuVar9);
              ppuVar10 = *pppuVar6;
              *pppuVar6 = (uint **)((int)*pppuVar6 + (int)ppppuVar18);
              ppppuVar18 = (uint ****)(uint)CARRY4((uint)ppuVar10,(uint)ppppuVar18);
              ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
            } while (ppppuVar9 != local_3a8);
            ppppuVar19 = local_3a8;
            if (ppppuVar18 == (uint ****)0x0) goto LAB_0054d9b3;
          }
          if (ppppuVar19 < (uint ****)0x73) {
            local_3a4[(int)ppppuVar19] = (uint ***)ppppuVar18;
            local_3a8 = (uint ****)((int)local_3a8 + 1);
            ppppuVar19 = local_3a8;
          }
          else {
            local_b2c = 0;
            local_3a8 = (uint ****)0x0;
            _memcpy_s(local_3a4,0x1cc,local_b28,0);
            ppppuVar19 = local_3a8;
          }
        }
LAB_0054d9b3:
        local_924 = (uint ****)0x0;
        ppppuVar9 = (uint ****)0x0;
      }
      local_924 = (uint ****)((int)local_924 * 10 + (uint)*(byte *)ppppuVar12);
      ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
      local_930 = (uint ****)((int)ppppuVar12 + 1);
    } while (local_930 != local_938);
    local_930 = ppppuVar9;
    if (ppppuVar9 != (uint ****)0x0) {
      for (local_93c = (uint ****)((uint)ppppuVar9 / 10); local_93c != (uint ****)0x0;
          local_93c = (uint ****)((int)local_93c - (int)local_934)) {
        local_934 = local_93c;
        if ((uint ****)0x26 < local_93c) {
          local_934 = (uint ****)0x26;
        }
        bVar2 = (&DAT_0059867f)[(int)local_934 * 4];
        uVar14 = (uint)(byte)(&DAT_0059867e)[(int)local_934 * 4];
        local_918 = (uint ****)(uVar14 + bVar2);
        _memset(local_914,0,uVar14 * 4);
        FUN_00543e20((uint *)(local_914 + uVar14),
                     (uint *)(&DAT_00597d78 +
                             (uint)*(ushort *)(&DAT_0059867c + (int)local_934 * 4) * 4),
                     (uint)bVar2 << 2);
        pppuVar6 = local_3a4[0];
        if (local_918 < (uint ****)0x2) {
          if (local_914[0] == (uint ***)0x0) {
LAB_0054da83:
            local_b2c = 0;
            local_3a8 = (uint ****)0x0;
            rVar24 = 0;
            pppuVar6 = local_b28;
            goto LAB_0054dd10;
          }
          if ((local_914[0] == (uint ***)0x1) || (ppppuVar19 == (uint ****)0x0)) {
LAB_0054daa1:
            bVar20 = true;
          }
          else {
            uVar14 = 0;
            ppppuVar9 = (uint ****)0x0;
            do {
              lVar22 = ZEXT48(local_914[0]) * ZEXT48(local_3a4[(int)ppppuVar9]) + (ulonglong)uVar14;
              local_3a4[(int)ppppuVar9] = (uint ***)(uint **)lVar22;
              uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
              ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
            } while (ppppuVar9 != ppppuVar19);
LAB_0054dacc:
            ppuVar10 = (uint **)((ulonglong)lVar22 >> 0x20);
            if (ppuVar10 != (uint **)0x0) {
              if ((uint ****)0x72 < local_3a8) {
                local_b2c = 0;
                local_3a8 = (uint ****)0x0;
                _memcpy_s(local_3a4,0x1cc,local_b28,0);
                bVar20 = false;
                ppppuVar19 = local_3a8;
                goto LAB_0054dd2e;
              }
              local_3a4[(int)local_3a8] = (uint ***)ppuVar10;
              local_3a8 = (uint ****)((int)local_3a8 + 1);
            }
            bVar20 = true;
            ppppuVar19 = local_3a8;
          }
        }
        else {
          if (ppppuVar19 < (uint ****)0x2) {
            local_3a8 = local_918;
            _memcpy_s(local_3a4,0x1cc,local_914,(int)local_918 << 2);
            if (pppuVar6 != (uint ***)0x0) {
              ppppuVar19 = local_3a8;
              if ((pppuVar6 == (uint ***)0x1) || (local_3a8 == (uint ****)0x0)) goto LAB_0054daa1;
              uVar14 = 0;
              ppppuVar19 = (uint ****)0x0;
              do {
                uVar21 = (ulonglong)uVar14;
                ppuVar10 = (uint **)local_3a4[(int)ppppuVar19];
                lVar22 = ZEXT48(pppuVar6) * ZEXT48(ppuVar10) + (ulonglong)uVar14;
                local_3a4[(int)ppppuVar19] = (uint ***)(uint **)lVar22;
                uVar14 = (uint)((ulonglong)lVar22 >> 0x20);
                lVar22 = ZEXT48(pppuVar6) * ZEXT48(ppuVar10) + uVar21;
                ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
              } while (ppppuVar19 != local_3a8);
              goto LAB_0054dacc;
            }
            goto LAB_0054da83;
          }
          local_944 = local_914;
          if (ppppuVar19 <= local_918) {
            local_944 = local_3a4;
          }
          local_92c = local_914;
          local_948 = local_918;
          local_950 = ppppuVar19;
          if (ppppuVar19 > local_918) {
            local_948 = ppppuVar19;
            local_92c = local_3a4;
            local_950 = local_918;
          }
          local_3a8 = (uint ****)0x0;
          ppppuVar19 = (uint ****)0x0;
          local_748 = (uint ****)0x0;
          if (local_950 != (uint ****)0x0) {
            do {
              if (local_944[(int)ppppuVar19] == (uint ***)0x0) {
                if (ppppuVar19 == local_3a8) {
                  local_744[(int)ppppuVar19] = (uint ***)0x0;
                  local_3a8 = (uint ****)((int)ppppuVar19 + 1);
                  local_748 = local_3a8;
                }
              }
              else {
                local_928 = (uint ****)0x0;
                local_938 = (uint ****)0x0;
                ppppuVar9 = ppppuVar19;
                if (local_948 != (uint ****)0x0) {
                  do {
                    if (ppppuVar9 == (uint ****)0x73) break;
                    if (ppppuVar9 == local_3a8) {
                      local_744[(int)ppppuVar9] = (uint ***)0x0;
                      local_748 = (uint ****)((byte *)((int)ppppuVar19 + 1) + (int)local_938);
                    }
                    pppuVar3 = local_92c[(int)local_938];
                    pppuVar4 = local_944[(int)ppppuVar19];
                    uVar14 = (uint)(ZEXT48(pppuVar3) * ZEXT48(pppuVar4));
                    pbVar7 = (byte *)(uVar14 + (int)local_928);
                    pppuVar6 = (uint ***)(local_744 + (int)ppppuVar9);
                    ppuVar10 = *pppuVar6;
                    *pppuVar6 = (uint **)(pbVar7 + (int)*pppuVar6);
                    local_928 = (uint ****)
                                ((int)(ZEXT48(pppuVar3) * ZEXT48(pppuVar4) >> 0x20) +
                                 (uint)CARRY4(uVar14,(uint)local_928) +
                                (uint)CARRY4((uint)ppuVar10,(uint)pbVar7));
                    local_938 = (uint ****)((int)local_938 + 1);
                    ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
                    local_3a8 = local_748;
                  } while (local_938 != local_948);
                  if (local_928 != (uint ****)0x0) {
                    pppuVar6 = (uint ***)(local_744 + (int)ppppuVar9);
                    ppppuVar18 = local_928;
                    ppppuVar12 = ppppuVar9;
                    do {
                      if (ppppuVar12 == (uint ****)0x73) goto LAB_0054ddd0;
                      ppppuVar9 = (uint ****)((int)ppppuVar12 + 1);
                      if (ppppuVar12 == local_3a8) {
                        *pppuVar6 = (uint **)0x0;
                        local_748 = ppppuVar9;
                      }
                      ppuVar10 = *pppuVar6;
                      *pppuVar6 = (uint **)((int)*pppuVar6 + (int)ppppuVar18);
                      ppppuVar18 = (uint ****)(uint)CARRY4((uint)ppuVar10,(uint)ppppuVar18);
                      local_3a8 = local_748;
                      pppuVar6 = pppuVar6 + 1;
                      ppppuVar12 = ppppuVar9;
                    } while (ppppuVar18 != (uint ****)0x0);
                  }
                }
                if (ppppuVar9 == (uint ****)0x73) {
LAB_0054ddd0:
                  local_3a8 = (uint ****)0x0;
                  _memcpy_s(local_3a4,0x1cc,local_cf8,0);
                  goto LAB_0054de1c;
                }
              }
              ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
            } while (ppppuVar19 != local_950);
          }
          rVar24 = (int)local_3a8 << 2;
          pppuVar6 = (uint ***)local_744;
LAB_0054dd10:
          _memcpy_s(local_3a4,0x1cc,pppuVar6,rVar24);
          bVar20 = true;
          ppppuVar19 = local_3a8;
        }
LAB_0054dd2e:
        if (!bVar20) goto LAB_0054de1c;
      }
      if ((uint)local_930 % 10 != 0) {
        uVar14 = *(uint *)(&DAT_00598714 + ((uint)local_930 % 10) * 2);
        if (uVar14 == 0) {
LAB_0054de1c:
          local_3a8 = (uint ****)0x0;
          local_b2c = 0;
          _memcpy_s(local_3a4,0x1cc,local_b28,0);
          ppppuVar19 = local_3a8;
        }
        else if ((uVar14 != 1) && (ppppuVar19 != (uint ****)0x0)) {
          ppuVar10 = (uint **)0x0;
          ppppuVar9 = (uint ****)0x0;
          do {
            lVar22 = (ulonglong)uVar14 * ZEXT48(local_3a4[(int)ppppuVar9]) + ZEXT48(ppuVar10);
            local_3a4[(int)ppppuVar9] = (uint ***)(uint **)lVar22;
            ppuVar10 = (uint **)((ulonglong)lVar22 >> 0x20);
            ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
          } while (ppppuVar9 != ppppuVar19);
          ppppuVar19 = local_3a8;
          if (ppuVar10 != (uint **)0x0) {
            if ((uint ****)0x72 < local_3a8) goto LAB_0054de1c;
            local_3a4[(int)local_3a8] = (uint ***)ppuVar10;
            local_3a8 = (uint ****)((int)local_3a8 + 1);
            ppppuVar19 = local_3a8;
          }
        }
      }
      if (local_924 != (uint ****)0x0) {
        ppppuVar18 = (uint ****)0x0;
        ppppuVar9 = local_924;
        if (ppppuVar19 != (uint ****)0x0) {
          do {
            pppuVar6 = (uint ***)(local_3a4 + (int)ppppuVar18);
            ppuVar10 = *pppuVar6;
            *pppuVar6 = (uint **)((int)*pppuVar6 + (int)ppppuVar9);
            ppppuVar9 = (uint ****)(uint)CARRY4((uint)ppuVar10,(uint)ppppuVar9);
            ppppuVar18 = (uint ****)((int)ppppuVar18 + 1);
          } while (ppppuVar18 != local_3a8);
          ppppuVar19 = local_3a8;
          if (ppppuVar9 == (uint ****)0x0) goto LAB_0054debd;
        }
        if (ppppuVar19 < (uint ****)0x73) {
          local_3a4[(int)ppppuVar19] = (uint ***)ppppuVar9;
          local_3a8 = (uint ****)((int)local_3a8 + 1);
          ppppuVar19 = local_3a8;
        }
        else {
          local_b2c = 0;
          local_3a8 = (uint ****)0x0;
          _memcpy_s(local_3a4,0x1cc,local_b28,0);
          ppppuVar19 = local_3a8;
        }
      }
    }
  }
LAB_0054debd:
  if ((int)*local_954 < 0) {
    local_94c = local_94c - *local_954;
  }
  local_574[1] = (uint ***)0x0;
  ppppuVar9 = (uint ****)0x1;
  local_574[0] = (uint ***)0x1;
  local_920 = (uint ****)0x1;
  local_578 = (uint ****)0x1;
  for (local_93c = (uint ****)(local_94c / 10); local_93c != (uint ****)0x0;
      local_93c = (uint ****)((int)local_93c - (int)local_934)) {
    local_934 = local_93c;
    if ((uint ****)0x26 < local_93c) {
      local_934 = (uint ****)0x26;
    }
    bVar2 = (&DAT_0059867f)[(int)local_934 * 4];
    uVar14 = (uint)(byte)(&DAT_0059867e)[(int)local_934 * 4];
    local_918 = (uint ****)(uVar14 + bVar2);
    _memset(local_914,0,uVar14 * 4);
    FUN_00543e20((uint *)(local_914 + uVar14),
                 (uint *)(&DAT_00597d78 + (uint)*(ushort *)(&DAT_0059867c + (int)local_934 * 4) * 4)
                 ,(uint)bVar2 << 2);
    pppuVar6 = local_574[0];
    if (local_918 < (uint ****)0x2) {
      if (local_914[0] == (uint ***)0x0) {
        local_b2c = 0;
        local_578 = (uint ****)0x0;
        rVar24 = 0;
        pppuVar6 = local_b28;
        goto LAB_0054e24b;
      }
      ppppuVar19 = local_920;
      if ((local_914[0] == (uint ***)0x1) || (ppppuVar9 == (uint ****)0x0)) goto LAB_0054e269;
      ppuVar10 = (uint **)0x0;
      ppppuVar19 = (uint ****)0x0;
      do {
        lVar22 = ZEXT48(local_914[0]) * ZEXT48(local_574[(int)ppppuVar19]) + ZEXT48(ppuVar10);
        local_574[(int)ppppuVar19] = (uint ***)(uint **)lVar22;
        ppuVar10 = (uint **)((ulonglong)lVar22 >> 0x20);
        ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
      } while (ppppuVar19 != ppppuVar9);
LAB_0054dfd7:
      ppppuVar9 = local_578;
      ppppuVar19 = local_578;
      if (ppuVar10 == (uint **)0x0) goto LAB_0054e269;
      if (local_578 < (uint ****)0x73) {
        local_574[(int)local_578] = (uint ***)ppuVar10;
        local_578 = (uint ****)((int)local_578 + 1);
        ppppuVar9 = local_578;
        ppppuVar19 = local_578;
        goto LAB_0054e269;
      }
      local_b2c = 0;
      local_578 = (uint ****)0x0;
      _memcpy_s(local_574,0x1cc,local_b28,0);
      bVar20 = false;
      local_920 = local_578;
      ppppuVar9 = local_578;
    }
    else {
      if (ppppuVar9 < (uint ****)0x2) {
        local_578 = local_918;
        _memcpy_s(local_574,0x1cc,local_914,(int)local_918 << 2);
        if (pppuVar6 != (uint ***)0x0) {
          local_920 = local_578;
          ppppuVar9 = local_578;
          ppppuVar19 = local_920;
          if ((pppuVar6 != (uint ***)0x1) && (local_578 != (uint ****)0x0)) {
            ppuVar10 = (uint **)0x0;
            ppppuVar19 = (uint ****)0x0;
            do {
              lVar22 = ZEXT48(pppuVar6) * ZEXT48(local_574[(int)ppppuVar19]) + ZEXT48(ppuVar10);
              local_574[(int)ppppuVar19] = (uint ***)(uint **)lVar22;
              ppuVar10 = (uint **)((ulonglong)lVar22 >> 0x20);
              ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
            } while (ppppuVar19 != local_578);
            goto LAB_0054dfd7;
          }
          goto LAB_0054e269;
        }
        rVar24 = 0;
        local_b2c = 0;
        local_578 = (uint ****)0x0;
        pppuVar6 = local_b28;
      }
      else {
        local_944 = local_914;
        if (ppppuVar9 <= local_918) {
          local_944 = local_574;
        }
        local_92c = local_914;
        local_948 = local_918;
        local_950 = ppppuVar9;
        if (ppppuVar9 > local_918) {
          local_948 = ppppuVar9;
          local_92c = local_574;
          local_950 = local_918;
        }
        local_578 = (uint ****)0x0;
        ppppuVar19 = (uint ****)0x0;
        local_748 = (uint ****)0x0;
        if (local_950 != (uint ****)0x0) {
          do {
            if (local_944[(int)ppppuVar19] == (uint ***)0x0) {
              if (ppppuVar19 == local_578) {
                local_744[(int)ppppuVar19] = (uint ***)0x0;
                local_578 = (uint ****)((int)ppppuVar19 + 1);
                local_748 = local_578;
              }
            }
            else {
              local_924 = (uint ****)0x0;
              local_938 = (uint ****)0x0;
              ppppuVar9 = ppppuVar19;
              if (local_948 != (uint ****)0x0) {
                do {
                  if (ppppuVar9 == (uint ****)0x73) break;
                  if (ppppuVar9 == local_578) {
                    local_744[(int)ppppuVar9] = (uint ***)0x0;
                    local_748 = (uint ****)((byte *)((int)local_924 + 1) + (int)ppppuVar19);
                  }
                  pppuVar3 = local_92c[(int)local_924];
                  pppuVar4 = local_944[(int)ppppuVar19];
                  uVar14 = (uint)(ZEXT48(pppuVar3) * ZEXT48(pppuVar4));
                  pbVar7 = (byte *)(uVar14 + (int)local_938);
                  pppuVar6 = (uint ***)(local_744 + (int)ppppuVar9);
                  ppuVar10 = *pppuVar6;
                  *pppuVar6 = (uint **)(pbVar7 + (int)*pppuVar6);
                  local_938 = (uint ****)
                              ((int)(ZEXT48(pppuVar3) * ZEXT48(pppuVar4) >> 0x20) +
                               (uint)CARRY4(uVar14,(uint)local_938) +
                              (uint)CARRY4((uint)ppuVar10,(uint)pbVar7));
                  local_924 = (uint ****)((int)local_924 + 1);
                  ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
                  local_578 = local_748;
                } while (local_924 != local_948);
                if (local_938 != (uint ****)0x0) {
                  ppppuVar18 = local_938;
                  pppuVar6 = (uint ***)(local_744 + (int)ppppuVar9);
                  ppppuVar12 = ppppuVar9;
                  do {
                    if (ppppuVar12 == (uint ****)0x73) goto LAB_0054e306;
                    ppppuVar9 = (uint ****)((int)ppppuVar12 + 1);
                    if (ppppuVar12 == local_578) {
                      *pppuVar6 = (uint **)0x0;
                      local_748 = ppppuVar9;
                    }
                    ppuVar10 = *pppuVar6;
                    *pppuVar6 = (uint **)((int)*pppuVar6 + (int)ppppuVar18);
                    ppppuVar18 = (uint ****)(uint)CARRY4((uint)ppuVar10,(uint)ppppuVar18);
                    local_578 = local_748;
                    pppuVar6 = pppuVar6 + 1;
                    ppppuVar12 = ppppuVar9;
                  } while (ppppuVar18 != (uint ****)0x0);
                }
              }
              if (ppppuVar9 == (uint ****)0x73) {
LAB_0054e306:
                local_578 = (uint ****)0x0;
                _memcpy_s(local_574,0x1cc,local_cf8,0);
                goto LAB_0054e3aa;
              }
            }
            ppppuVar19 = (uint ****)((int)ppppuVar19 + 1);
          } while (ppppuVar19 != local_950);
        }
        rVar24 = (int)local_578 << 2;
        pppuVar6 = (uint ***)local_744;
      }
LAB_0054e24b:
      _memcpy_s(local_574,0x1cc,pppuVar6,rVar24);
      ppppuVar9 = local_578;
      ppppuVar19 = local_578;
LAB_0054e269:
      local_920 = ppppuVar19;
      bVar20 = true;
    }
    if (!bVar20) goto LAB_0054e3aa;
    ppppuVar19 = local_3a8;
  }
  ppppuVar9 = local_920;
  if (local_94c % 10 != 0) {
    local_92c = *(uint *****)(&DAT_00598714 + (local_94c % 10) * 2);
    if (local_92c == (uint ****)0x0) {
      local_b2c = 0;
      local_578 = (uint ****)0x0;
      _memcpy_s(local_574,0x1cc,local_b28,0);
      ppppuVar19 = local_3a8;
      ppppuVar9 = local_578;
    }
    else if ((local_92c != (uint ****)0x1) && (local_920 != (uint ****)0x0)) {
      ppuVar10 = (uint **)0x0;
      ppppuVar9 = (uint ****)0x0;
      do {
        lVar22 = ZEXT48(local_92c) * ZEXT48(local_574[(int)ppppuVar9]) + ZEXT48(ppuVar10);
        local_574[(int)ppppuVar9] = (uint ***)(uint **)lVar22;
        ppuVar10 = (uint **)((ulonglong)lVar22 >> 0x20);
        ppppuVar9 = (uint ****)((int)ppppuVar9 + 1);
      } while (ppppuVar9 != local_920);
      ppppuVar9 = local_578;
      if (ppuVar10 != (uint **)0x0) {
        if ((uint ****)0x72 < local_578) {
LAB_0054e3aa:
          local_578 = (uint ****)0x0;
          local_b2c = 0;
          _memcpy_s(local_574,0x1cc,local_b28,0);
          __crt_strtox::assemble_floating_point_zero(SUB41(local_954[0xc2],0),local_95c);
          goto LAB_0054e8e0;
        }
        local_574[(int)local_578] = (uint ***)ppuVar10;
        local_578 = (uint ****)((int)local_578 + 1);
        ppppuVar9 = local_578;
      }
    }
  }
  local_920 = ppppuVar9;
  if (ppppuVar19 == (uint ****)0x0) {
    uVar14 = 0;
  }
  else {
    ppuVar10 = (uint **)local_3a4[(int)((int)ppppuVar19 + -1)];
    local_92c = (uint ****)0x0;
    iVar5 = 0x1f;
    if (ppuVar10 != (uint **)0x0) {
      for (; (uint)ppuVar10 >> iVar5 == 0; iVar5 = iVar5 + -1) {
      }
    }
    if (ppuVar10 == (uint **)0x0) {
      iVar5 = 0;
    }
    else {
      iVar5 = iVar5 + 1;
    }
    uVar14 = (int)((int)ppppuVar19 + -1) * 0x20 + iVar5;
  }
  if (local_920 == (uint ****)0x0) {
    uVar11 = 0;
  }
  else {
    ppuVar10 = (uint **)local_574[(int)((int)local_920 + -1)];
    local_92c = (uint ****)0x0;
    iVar5 = 0x1f;
    if (ppuVar10 != (uint **)0x0) {
      for (; (uint)ppuVar10 >> iVar5 == 0; iVar5 = iVar5 + -1) {
      }
    }
    if (ppuVar10 == (uint **)0x0) {
      iVar5 = 0;
    }
    else {
      iVar5 = iVar5 + 1;
    }
    uVar11 = (int)((int)local_920 + -1) * 0x20 + iVar5;
  }
  local_930 = (uint ****)(-(uint)(uVar14 < uVar11) & uVar11 - uVar14);
  ppppuVar9 = local_920;
  if (local_930 != (uint ****)0x0) {
    local_944 = (uint ****)((uint)local_930 & 0x1f);
    local_924 = (uint ****)((uint)local_930 >> 5);
    local_950 = (uint ****)(0x20 - (int)local_944);
    uVar14 = 1 << ((uint)local_950 & 0x1f);
    uVar11 = 0;
    if ((uint ****)0x1f < local_950) {
      uVar11 = uVar14;
    }
    ppuVar10 = (uint **)local_3a4[(int)((int)ppppuVar19 + -1)];
    local_92c = (uint ****)((uVar14 ^ uVar11) - 1);
    local_948 = (uint ****)~(uint)local_92c;
    iVar5 = 0x1f;
    if (ppuVar10 != (uint **)0x0) {
      for (; (uint)ppuVar10 >> iVar5 == 0; iVar5 = iVar5 + -1) {
      }
    }
    local_938 = (uint ****)0x0;
    if (ppuVar10 == (uint **)0x0) {
      iVar5 = 0;
    }
    else {
      iVar5 = iVar5 + 1;
    }
    if (((byte *)((int)local_924 + (int)ppppuVar19) < (byte *)0x74) &&
       (local_928 = (uint ****)
                    ((byte *)((uint)((uint ****)(0x20 - iVar5) < local_944) + (int)local_924) +
                    (int)ppppuVar19), local_928 < (uint ****)0x74)) {
      local_93c = (uint ****)((int)local_924 + -1);
      local_934 = (uint ****)((int)local_928 + -1);
      if (local_934 != local_93c) {
        ppppuVar18 = (uint ****)((int)local_934 - (int)local_924);
        pppuVar6 = (uint ***)(local_3a4 + (int)((int)ppppuVar18 + -1));
        do {
          if (ppppuVar18 < ppppuVar19) {
            local_938 = (uint ****)pppuVar6[1];
          }
          else {
            local_938 = (uint ****)0x0;
          }
          if ((uint ****)((int)ppppuVar18 + -1) < ppppuVar19) {
            ppuVar10 = *pppuVar6;
          }
          else {
            ppuVar10 = (uint **)0x0;
          }
          pppuVar6 = pppuVar6 + -1;
          local_3a4[(int)local_934] =
               (uint ***)
               (((uint)ppuVar10 & (uint)local_948) >> ((byte)local_950 & 0x1f) |
               ((uint)local_938 & (uint)local_92c) << (sbyte)local_944);
          local_934 = (uint ****)((int)local_934 + -1);
          ppppuVar18 = (uint ****)((int)ppppuVar18 + -1);
          ppppuVar19 = local_3a8;
        } while (local_934 != local_93c);
      }
      if (local_924 != (uint ****)0x0) {
        pppuVar6 = (uint ***)local_3a4;
        for (ppppuVar19 = local_924; ppppuVar19 != (uint ****)0x0;
            ppppuVar19 = (uint ****)((int)ppppuVar19 + -1)) {
          *pppuVar6 = (uint **)0x0;
          pppuVar6 = pppuVar6 + 1;
        }
      }
      local_3a8 = local_928;
      ppppuVar19 = local_3a8;
    }
    else {
      local_3a8 = (uint ****)0x0;
      local_b2c = 0;
      _memcpy_s(local_3a4,0x1cc,local_b28,0);
      ppppuVar19 = local_3a8;
      ppppuVar9 = local_578;
    }
  }
  local_958 = (uint ****)((int)local_958 - (int)local_940);
  ppppuVar18 = local_958;
  if (local_940 != (uint ****)0x0) {
    if (local_958 < local_930) {
      bVar20 = true;
      goto LAB_0054e8bb;
    }
    ppppuVar18 = (uint ****)((int)local_958 - (int)local_930);
  }
  if (ppppuVar9 < ppppuVar19) {
LAB_0054e624:
    cVar8 = '\0';
  }
  else {
    ppppuVar12 = ppppuVar19;
    if (ppppuVar19 < ppppuVar9) {
      cVar8 = '\x01';
    }
    else {
      do {
        ppppuVar12 = (uint ****)((int)ppppuVar12 + -1);
        if (ppppuVar12 == (uint ****)0xffffffff) goto LAB_0054e624;
      } while (local_574[(int)ppppuVar12] == local_3a4[(int)ppppuVar12]);
      cVar8 = '\x01' - (local_574[(int)ppppuVar12] < local_3a4[(int)ppppuVar12]);
    }
  }
  if (cVar8 != '\0') {
    local_930 = (uint ****)((int)local_930 + 1);
  }
  local_924 = (uint ****)((uint)ppppuVar18 >> 5);
  local_934 = (uint ****)((uint)ppppuVar18 & 0x1f);
  local_950 = (uint ****)(0x20 - (int)local_934);
  uVar14 = 1 << ((uint)local_950 & 0x1f);
  uVar11 = 0;
  if ((uint ****)0x1f < local_950) {
    uVar11 = uVar14;
  }
  local_938 = (uint ****)0x0;
  local_92c = (uint ****)((uVar14 ^ uVar11) - 1);
  local_948 = (uint ****)~(uint)local_92c;
  ppuVar10 = (uint **)local_3a4[(int)((int)ppppuVar19 + -1)];
  iVar5 = 0x1f;
  if (ppuVar10 != (uint **)0x0) {
    for (; (uint)ppuVar10 >> iVar5 == 0; iVar5 = iVar5 + -1) {
    }
  }
  if (ppuVar10 == (uint **)0x0) {
    iVar5 = 0;
  }
  else {
    iVar5 = iVar5 + 1;
  }
  if (((byte *)((int)local_924 + (int)ppppuVar19) < (byte *)0x74) &&
     (local_928 = (uint ****)
                  ((byte *)((uint)((uint ****)(0x20 - iVar5) < local_934) + (int)local_924) +
                  (int)ppppuVar19), local_928 < (uint ****)0x74)) {
    local_93c = (uint ****)((int)local_924 + -1);
    local_944 = (uint ****)((int)local_928 + -1);
    if (local_944 != local_93c) {
      ppppuVar9 = (uint ****)((int)local_944 - (int)local_924);
      pppuVar6 = (uint ***)(local_3a4 + (int)((int)ppppuVar9 + -1));
      do {
        if (ppppuVar9 < ppppuVar19) {
          local_938 = (uint ****)pppuVar6[1];
        }
        else {
          local_938 = (uint ****)0x0;
        }
        if ((uint ****)((int)ppppuVar9 + -1) < ppppuVar19) {
          ppuVar10 = *pppuVar6;
        }
        else {
          ppuVar10 = (uint **)0x0;
        }
        pppuVar6 = pppuVar6 + -1;
        local_3a4[(int)local_944] =
             (uint ***)
             (((uint)ppuVar10 & (uint)local_948) >> ((byte)local_950 & 0x1f) |
             ((uint)local_938 & (uint)local_92c) << (sbyte)local_934);
        local_944 = (uint ****)((int)local_944 + -1);
        ppppuVar9 = (uint ****)((int)ppppuVar9 + -1);
        ppppuVar19 = local_3a8;
      } while (local_944 != local_93c);
    }
    local_3a8 = local_928;
    if (local_924 != (uint ****)0x0) {
      pppuVar6 = (uint ***)local_3a4;
      for (ppppuVar19 = local_924; ppppuVar19 != (uint ****)0x0;
          ppppuVar19 = (uint ****)((int)ppppuVar19 + -1)) {
        *pppuVar6 = (uint **)0x0;
        pppuVar6 = pppuVar6 + 1;
      }
    }
  }
  else {
    local_b2c = 0;
    local_3a8 = (uint ****)0x0;
    _memcpy_s(local_3a4,0x1cc,local_b28,0);
  }
  uVar21 = FUN_0054e9f0((uint *)&local_3a8,&local_578);
  uVar11 = (uint)(uVar21 >> 0x20);
  uVar14 = (uint)uVar21;
  local_928 = (uint ****)CONCAT31(local_928._1_3_,local_3a8 == (uint ****)0x0);
  if (uVar11 == 0) {
    iVar5 = 0x1f;
    if (uVar14 != 0) {
      for (; uVar14 >> iVar5 == 0; iVar5 = iVar5 + -1) {
      }
    }
    if (uVar14 == 0) {
      ppppuVar19 = (uint ****)0x0;
    }
    else {
      ppppuVar19 = (uint ****)(iVar5 + 1);
    }
  }
  else {
    iVar5 = 0x1f;
    if (uVar11 != 0) {
      for (; uVar11 >> iVar5 == 0; iVar5 = iVar5 + -1) {
      }
    }
    if (uVar11 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = iVar5 + 1;
    }
    ppppuVar19 = (uint ****)(iVar5 + 0x20);
  }
  if (local_958 < ppppuVar19) {
    uVar17 = (int)ppppuVar19 - (int)local_958;
    if (local_3a8 == (uint ****)0x0) {
      uVar16 = 1 << (uVar17 & 0x1f);
      uVar13 = 0;
      if (0x1f < uVar17) {
        uVar13 = uVar16;
      }
      uVar16 = uVar16 ^ uVar13;
      if (0x3f < uVar17) {
        uVar13 = uVar16;
      }
      if ((uVar16 - 1 & uVar14) != 0 || ((uVar13 - 1) + (uint)(uVar16 != 0) & uVar11) != 0)
      goto LAB_0054e824;
      local_928 = (uint ****)CONCAT31(local_928._1_3_,1);
    }
    else {
LAB_0054e824:
      local_928 = (uint ****)((uint)local_928._1_3_ << 8);
    }
    uVar21 = __aullshr((byte)uVar17,uVar11);
  }
  uVar23 = (undefined1)local_954[0xc2];
  pbVar7 = (byte *)~(uint)local_930;
  if (local_940 != (uint ****)0x0) {
    pbVar7 = (byte *)((int)local_940 + -2);
  }
  ppppuVar19 = local_928;
  pfVar25 = local_95c;
  lVar22 = __allshl((byte)local_958,-(uint)((uint ****)0x1 < local_91c) & local_1d0);
  FUN_0054c685((uint)(lVar22 + uVar21),(uint)(lVar22 + uVar21 >> 0x20),(int)pbVar7,(bool)uVar23,
               (char)ppppuVar19,pfVar25);
LAB_0054e8e0:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

