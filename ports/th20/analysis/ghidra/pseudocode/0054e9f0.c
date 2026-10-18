/* Entry: 0x0054e9f0; symbol: FUN_0054e9f0; body bytes: 1239 */

/* WARNING: Removing unreachable block (ram,0x0054ed58) */

ulonglong __cdecl FUN_0054e9f0(uint *param_1,undefined4 *param_2)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  byte bVar4;
  uint extraout_ECX;
  uint *puVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint *puVar10;
  bool bVar11;
  undefined8 uVar12;
  longlong lVar13;
  ulonglong uVar14;
  undefined1 local_228 [460];
  uint local_5c;
  undefined8 local_58;
  undefined8 local_50;
  uint *local_48;
  uint local_44;
  uint local_40;
  uint local_3c;
  int local_38;
  int local_34;
  uint *local_30;
  uint *local_2c;
  uint *local_28;
  undefined8 local_24;
  uint local_1c;
  int local_18;
  undefined8 local_14;
  uint *local_c;
  uint *local_8;
  
  uVar7 = *param_1;
  if ((uVar7 != 0) && (local_2c = (uint *)*param_2, local_2c != (uint *)0x0)) {
    local_c = (uint *)(uVar7 - 1);
    uVar8 = (int)local_2c - 1;
    if (uVar8 == 0) {
      local_30 = (uint *)param_2[1];
      if (local_30 == (uint *)0x1) {
        uVar7 = param_1[1];
        *param_1 = 0;
        _memcpy_s(param_1 + 1,0x1cc,local_228,0);
        return (ulonglong)uVar7;
      }
      if (local_c != (uint *)0x0) {
        local_14 = 0;
        local_58 = 0;
        local_58._0_4_ = 0;
        local_8 = (uint *)0x0;
        if (local_c == (uint *)0xffffffff) {
          local_14._4_4_ = (uint *)0x0;
          local_2c = (uint *)0x0;
          local_c = (uint *)0xffffffff;
          puVar5 = local_14._4_4_;
          uVar8 = (uint)local_58;
        }
        else {
          local_c = param_1 + uVar7;
          puVar5 = param_1;
          uVar8 = (uint)local_58;
          do {
            puVar2 = puVar5;
            uVar12 = __aulldvrm(*local_c,uVar8,(uint)local_30,0);
            local_c = local_c + -1;
            uVar7 = uVar7 - 1;
            puVar5 = local_8;
            uVar8 = extraout_ECX;
            local_2c = puVar2;
            local_8 = (uint *)uVar12;
          } while (uVar7 != 0);
        }
        *param_1 = 0;
        _memcpy_s(param_1 + 1,0x1cc,local_228,0);
        param_1[1] = uVar8;
        param_1[2] = (uint)local_2c;
        *param_1 = (local_2c != (uint *)0x0) + 1;
        return CONCAT44(puVar5,local_8);
      }
      uVar7 = param_1[1];
      *param_1 = 0;
      _memcpy_s(param_1 + 1,0x1cc,local_228,0);
      uVar8 = uVar7 % (uint)local_30;
      param_1[1] = uVar8;
      *param_1 = (uint)(uVar8 != 0);
      return (ulonglong)uVar7 / ZEXT48(local_30);
    }
    if (uVar8 <= local_c) {
      iVar6 = (int)local_c - uVar8;
      if (iVar6 <= (int)local_c) {
        puVar5 = param_1 + uVar7;
        puVar2 = param_2 + (int)local_2c;
        uVar7 = (uint)local_c;
        do {
          if (*puVar2 != *puVar5) {
            if (*puVar5 <= *puVar2) goto LAB_0054eb96;
            break;
          }
          uVar7 = uVar7 - 1;
          puVar2 = puVar2 + -1;
          puVar5 = puVar5 + -1;
        } while (iVar6 <= (int)uVar7);
      }
      iVar6 = iVar6 + 1;
LAB_0054eb96:
      if (iVar6 != 0) {
        local_8 = (uint *)param_2[(int)local_2c - 1];
        local_3c = param_2[(int)local_2c];
        local_34 = 0x1f;
        if (local_3c != 0) {
          for (; local_3c >> local_34 == 0; local_34 = local_34 + -1) {
          }
        }
        if (local_3c == 0) {
          local_34 = 0x20;
        }
        else {
          local_34 = 0x1f - local_34;
        }
        local_38 = 0x20 - local_34;
        if (local_34 != 0) {
          local_3c = local_3c << ((byte)local_34 & 0x1f) | (uint)local_8 >> ((byte)local_38 & 0x1f);
          local_8 = (uint *)((int)local_8 << ((byte)local_34 & 0x1f));
          if ((uint *)0x2 < local_2c) {
            local_8 = (uint *)((uint)local_8 |
                              (uint)param_2[(int)local_2c - 2] >> ((byte)local_38 & 0x1f));
          }
        }
        iVar1 = iVar6 + -1;
        local_14 = 0;
        local_58 = CONCAT44(iVar1,(uint)local_58);
        local_14._0_4_ = 0;
        if (iVar1 < 0) {
          local_14._4_4_ = (uint *)0x0;
          uVar8 = (uint)local_14._4_4_;
          uVar7 = (uint)local_14;
        }
        else {
          local_44 = iVar1 + (int)local_2c;
          local_48 = param_1 + iVar6;
          local_30 = param_1 + (local_44 - 1);
          local_5c = 0;
          do {
            if (local_c < local_44) {
              uVar7 = 0;
            }
            else {
              uVar7 = local_30[2];
            }
            local_1c = local_30[1];
            uVar8 = *local_30;
            local_50 = CONCAT44(uVar7,(undefined4)local_50);
            local_24 = CONCAT44(uVar7,(int)local_24);
            local_28 = (uint *)uVar8;
            if (local_34 != 0) {
              bVar4 = (byte)local_38;
              lVar13 = __allshl((byte)local_34,uVar7);
              uVar7 = (uint)((ulonglong)lVar13 >> 0x20);
              local_1c = (uint)lVar13 | uVar8 >> (bVar4 & 0x1f);
              local_28 = (uint *)(uVar8 << ((byte)local_34 & 0x1f));
              local_24 = CONCAT44(uVar7,(uint)local_24);
              if (2 < local_44) {
                local_28 = (uint *)((uint)local_28 | local_30[-1] >> ((byte)local_38 & 0x1f));
              }
            }
            uVar8 = local_3c;
            local_14 = __aulldiv(local_1c,uVar7,local_3c,0);
            local_18 = (int)(local_14 >> 0x20);
            uVar7 = (uint)local_14;
            local_40 = uVar7;
            uVar14 = __aullrem(local_1c,local_24._4_4_,uVar8,0);
            uVar14 = uVar14 & 0xffffffff;
            local_1c = 0;
            if (local_18 != 0) {
              lVar13 = __allmul(uVar7 + 1,local_18 + -1 + (uint)(0xfffffffe < uVar7),local_3c,0);
              uVar14 = lVar13 + uVar14;
              local_1c = (uint)(uVar14 >> 0x20);
              uVar7 = 0xffffffff;
              local_40 = 0xffffffff;
              local_18 = 0;
              local_14 = 0xffffffff;
            }
            uVar9 = (uint)uVar14;
            uVar8 = local_40;
            if ((int)(uVar14 >> 0x20) == 0) {
              while( true ) {
                local_24 = CONCAT44(local_28,(uint)local_24);
                lVar13 = __allmul((uint)local_8,0,uVar7,local_18);
                uVar3 = (uint)((ulonglong)lVar13 >> 0x20);
                uVar8 = uVar7;
                if ((uVar3 < uVar9) || ((uVar3 <= uVar9 && ((uint)lVar13 <= local_24._4_4_))))
                break;
                bVar11 = uVar7 != 0;
                uVar7 = uVar7 - 1;
                local_18 = local_18 + -1 + (uint)bVar11;
                bVar11 = CARRY4(uVar9,local_3c);
                uVar9 = uVar9 + local_3c;
                local_14 = CONCAT44(local_18,uVar7);
                uVar8 = uVar7;
                if (local_1c + bVar11 != 0) break;
                local_1c = 0;
              }
            }
            local_40 = uVar8;
            if ((local_18 != 0) || (uVar7 != 0)) {
              local_24._4_4_ = 0;
              local_24._0_4_ = 0;
              if (local_2c != (uint *)0x0) {
                puVar2 = param_2 + 1;
                local_28 = local_2c;
                puVar5 = local_48;
                do {
                  lVar13 = (local_14 & 0xffffffff) * (ulonglong)*puVar2;
                  uVar7 = (uint)lVar13;
                  uVar8 = (uint)local_24 + uVar7;
                  local_24._0_4_ =
                       local_24._4_4_ +
                       (int)((ulonglong)lVar13 >> 0x20) + (int)local_14._4_4_ * *puVar2 +
                       (uint)CARRY4((uint)local_24,uVar7);
                  local_24._4_4_ = 0;
                  if (*puVar5 < uVar8) {
                    bVar11 = 0xfffffffe < (uint)local_24;
                    local_24._0_4_ = (uint)local_24 + 1;
                    local_24._4_4_ = (uint)bVar11;
                  }
                  *puVar5 = *puVar5 - uVar8;
                  puVar5 = puVar5 + 1;
                  puVar2 = puVar2 + 1;
                  local_28 = (uint *)((int)local_28 - 1);
                  uVar7 = local_40;
                } while (local_28 != (uint *)0x0);
              }
              if ((local_24._4_4_ != 0) || (local_50._4_4_ < (uint)local_24)) {
                if (local_2c != (uint *)0x0) {
                  local_50 = 0;
                  puVar5 = local_48;
                  puVar2 = param_2 + 1;
                  puVar10 = local_2c;
                  do {
                    uVar7 = *puVar5;
                    uVar8 = *puVar2;
                    uVar9 = uVar7 + *puVar2;
                    *puVar5 = uVar9 + local_50._4_4_;
                    local_50 = (ulonglong)
                               ((uint)CARRY4(uVar7,uVar8) + (uint)CARRY4(uVar9,local_50._4_4_)) <<
                               0x20;
                    puVar10 = (uint *)((int)puVar10 - 1);
                    puVar5 = puVar5 + 1;
                    uVar7 = local_40;
                    puVar2 = puVar2 + 1;
                  } while (puVar10 != (uint *)0x0);
                }
                uVar7 = uVar7 - 1;
              }
              local_c = (uint *)(local_44 - 1);
              local_24 = 0;
            }
            local_48 = local_48 + -1;
            iVar6 = local_58._4_4_ + -1;
            local_44 = local_44 - 1;
            local_58 = CONCAT44(iVar6,(uint)local_58);
            local_30 = local_30 + -1;
            uVar8 = local_5c;
            local_5c = uVar7;
          } while (-1 < iVar6);
        }
        uVar9 = (int)local_c + 1;
        if (uVar9 < *param_1) {
          puVar5 = param_1 + (int)local_c + 2;
          uVar3 = uVar9;
          do {
            *puVar5 = 0;
            puVar5 = puVar5 + 1;
            uVar3 = uVar3 + 1;
          } while (uVar3 < *param_1);
        }
        *param_1 = uVar9;
        while ((uVar9 != 0 && (param_1[uVar9] == 0))) {
          uVar9 = uVar9 - 1;
          *param_1 = uVar9;
        }
        return CONCAT44(uVar8,uVar7);
      }
    }
  }
  return 0;
}

