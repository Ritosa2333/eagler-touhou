/* Entry: 0x00469720; symbol: FUN_00469720; body bytes: 2202 */

void * __cdecl
FUN_00469720(void *param_1,char *param_2,char *param_3,undefined8 param_4,size_t param_5)

{
  char *pcVar1;
  char *pcVar2;
  bool bVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  longlong lVar7;
  ulonglong uVar8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  int local_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  int local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  int local_94;
  int local_90;
  uint local_8c;
  int local_88;
  uint local_84;
  uint local_80;
  uint local_7c;
  uint local_78;
  uint local_74;
  uint local_70;
  uint local_6c;
  undefined8 local_68;
  int local_60;
  uint local_5c;
  uint local_58;
  size_t local_54;
  int local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  char *local_40;
  uint local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  uint local_28;
  int local_24;
  byte local_1e;
  char local_1d;
  byte local_1c [4];
  char *local_18;
  int local_14;
  char local_d;
  uint local_c;
  char local_6;
  char local_5;
  
  pcVar1 = param_2;
  lVar7 = FUN_0046fc30();
  local_68._4_4_ = (uint)((ulonglong)lVar7 >> 0x20);
  local_80 = (uint)lVar7;
  if (lVar7 == 0) {
    local_84 = (uint)(param_5 != 0);
    local_a8 = local_84 + 1 + param_5;
    local_68 = lVar7;
    if ((int)param_3 - (int)param_2 < local_a8) {
      local_d0 = 0x84;
      FUN_00467cc0(param_1,&param_3,&local_d0);
    }
    else {
      *param_2 = '0';
      pcVar1 = param_2 + 1;
      if (param_5 != 0) {
        param_2[1] = '.';
        param_2 = param_2 + 2;
        local_1c[3] = 0x30;
        FUN_0046a2f0(param_2,param_5,local_1c + 3);
        pcVar1 = param_2 + param_5;
      }
      param_2 = pcVar1;
      local_d4 = 0;
      FUN_00467cc0(param_1,&param_2,&local_d4);
    }
  }
  else {
    local_7c = local_68._4_4_ & 0xfffff;
    local_68._0_4_ = local_80;
    uVar8 = __aullshr(0x34,local_68._4_4_);
    local_88 = (int)uVar8;
    if (local_88 == 0) {
      local_14 = -0x432;
      local_44 = local_7c;
    }
    else {
      local_14 = local_88 + -0x433;
      local_44 = local_7c | 0x100000;
    }
    local_48 = local_80;
    local_6 = '\0';
    if (-0x35 < local_14) {
      if (local_14 < 0) {
        local_6c = 0;
      }
      else {
        local_6c = FUN_0046fc70(local_14);
      }
      local_4c = local_6c;
      local_90 = FUN_0046ffc0(local_6c);
      local_8c = FUN_0046fc80(local_4c);
      local_34 = local_8c;
      while (local_34 = local_34 - 1, -1 < (int)local_34) {
        local_94 = local_90 - local_14;
        iVar4 = local_94 + 8;
        puVar5 = (uint *)(&DAT_0057c5e0 +
                         (*(ushort *)(&DAT_0056f418 + local_4c * 2) + local_34) * 0x18);
        lVar7 = __allshl(8,local_44);
        local_38 = FUN_0046fce0((uint)lVar7,(uint)((ulonglong)lVar7 >> 0x20),puVar5,iVar4);
        if (local_6 == '\0') {
          if (local_38 != 0) {
            local_50 = FUN_0046fb40(local_38);
            if ((int)param_3 - (int)param_2 < local_50) {
              local_9c = 0x84;
              FUN_00467cc0(param_1,&param_3,&local_9c);
              return param_1;
            }
            FUN_004694f0(local_50,local_38,param_2);
            param_2 = param_2 + local_50;
            local_6 = '\x01';
          }
        }
        else {
          if ((int)param_3 - (int)param_2 < 9) {
            local_98 = 0x84;
            FUN_00467cc0(param_1,&param_3,&local_98);
            return param_1;
          }
          FUN_00469640(local_38,param_2);
          param_2 = param_2 + 9;
        }
      }
    }
    if (local_6 == '\0') {
      if (param_2 == param_3) {
        local_a0 = 0x84;
        FUN_00467cc0(param_1,&param_3,&local_a0);
        return param_1;
      }
      *param_2 = '0';
      param_2 = param_2 + 1;
    }
    if (param_5 != 0) {
      if (param_2 == param_3) {
        local_a4 = 0x84;
        FUN_00467cc0(param_1,&param_3,&local_a4);
        return param_1;
      }
      *param_2 = '.';
      param_2 = param_2 + 1;
    }
    if (local_14 < 0) {
      local_24 = (int)(-local_14 + (-local_14 >> 0x1f & 0xfU)) >> 4;
      local_3c = param_5 / 9 + 1;
      local_30 = 0;
      local_c = 0;
      if ((byte)(&UNK_0056f528)[local_24] < local_3c) {
        if ((&UNK_0056f528)[local_24] != '\0') {
          local_c = (uint)(byte)(&UNK_0056f528)[local_24];
          if ((int)param_3 - (int)param_2 < (int)(local_c * 9)) {
            local_ac = 0x84;
            FUN_00467cc0(param_1,&param_3,&local_ac);
            return param_1;
          }
          local_1c[1] = 0x30;
          FUN_0046a2f0(param_2,local_c * 9,local_1c + 1);
          param_2 = param_2 + local_c * 9;
        }
      }
      else {
        local_c = local_3c;
        if ((int)param_3 - (int)param_2 < (int)param_5) {
          local_c0 = 0x84;
          FUN_00467cc0(param_1,&param_3,&local_c0);
          return param_1;
        }
        local_1c[2] = 0x30;
        FUN_0046a2f0(param_2,param_5,local_1c + 2);
        param_2 = param_2 + param_5;
      }
LAB_00469bad:
      if (local_c < local_3c) {
        iVar4 = local_24 * -0x10 - local_14;
        local_b4 = iVar4 + 0x78;
        local_70 = (*(ushort *)(&UNK_0056f498 + local_24 * 2) + local_c) -
                   (uint)(byte)(&UNK_0056f528)[local_24];
        if (local_70 < *(ushort *)(&UNK_0056f49a + local_24 * 2)) {
          iVar4 = iVar4 + 0x80;
          puVar5 = (uint *)(&DAT_005838a0 + local_70 * 0x18);
          lVar7 = __allshl(8,local_44);
          local_28 = FUN_0046fce0((uint)lVar7,(uint)((ulonglong)lVar7 >> 0x20),puVar5,iVar4);
          if (local_c < local_3c - 1) goto code_r0x00469c99;
          local_2c = param_5 + local_c * -9;
          local_5c = 0;
          for (local_58 = 0; local_58 < 9 - local_2c; local_58 = local_58 + 1) {
            local_5c = local_28 % 10;
            local_28 = local_28 / 10;
          }
          if (local_5c == 5) {
            local_60 = (-param_5 - local_14) + -1;
            if ((local_60 < 1) ||
               ((local_60 < 0x3c && (bVar3 = FUN_0046ff30(local_48,local_44,(byte)local_60), bVar3))
               )) {
              local_d = '\x01';
            }
            else {
              local_d = '\0';
            }
            local_1d = local_d;
            if (local_d == '\0') {
              local_78 = 1;
            }
            else {
              local_78 = 2;
            }
            local_30 = local_78;
          }
          else {
            local_74 = (uint)(5 < local_5c);
            local_30 = local_74;
          }
          if (local_2c != 0) {
            if ((int)param_3 - (int)param_2 < (int)local_2c) {
              local_bc = 0x84;
              FUN_00467cc0(param_1,&param_3,&local_bc);
              return param_1;
            }
            FUN_00469450(local_2c,local_28,(int)param_2);
            param_2 = param_2 + local_2c;
          }
        }
        else {
          local_54 = param_5 + local_c * -9;
          if ((int)param_3 - (int)param_2 < (int)local_54) {
            local_b0 = 0x84;
            FUN_00467cc0(param_1,&param_3,&local_b0);
            return param_1;
          }
          local_1c[0] = 0x30;
          FUN_0046a2f0(param_2,local_54,local_1c);
          param_2 = param_2 + local_54;
        }
      }
      if (local_30 == 0) goto LAB_00469f95;
      local_18 = param_2;
      local_40 = param_3;
      pcVar2 = local_40;
      while (local_40 = pcVar2, local_18 != pcVar1) {
        local_18 = local_18 + -1;
        local_5 = *local_18;
        pcVar2 = local_18;
        if (local_5 != '.') {
          if (local_5 != '9') {
            if (local_30 != 1) {
              uVar6 = (int)local_5 & 0x80000001;
              if ((int)uVar6 < 0) {
                uVar6 = (uVar6 - 1 | 0xfffffffe) + 1;
              }
              if (uVar6 == 0) goto LAB_00469f95;
            }
            *local_18 = local_5 + '\x01';
            goto LAB_00469f95;
          }
          *local_18 = '0';
          local_30 = 1;
          pcVar2 = local_40;
        }
      }
      *local_18 = '1';
      if (local_40 != param_3) {
        *local_40 = '0';
        local_40[1] = '.';
      }
      if (param_2 == param_3) {
        local_c4 = 0x84;
        FUN_00467cc0(param_1,&param_3,&local_c4);
        return param_1;
      }
      *param_2 = '0';
      param_2 = param_2 + 1;
    }
    else {
      if ((int)param_3 - (int)param_2 < (int)param_5) {
        local_c8 = 0x84;
        FUN_00467cc0(param_1,&param_3,&local_c8);
        return param_1;
      }
      local_1e = 0x30;
      FUN_0046a2f0(param_2,param_5,&local_1e);
      param_2 = param_2 + param_5;
    }
LAB_00469f95:
    local_cc = 0;
    FUN_00467cc0(param_1,&param_2,&local_cc);
  }
  return param_1;
code_r0x00469c99:
  if ((int)param_3 - (int)param_2 < 9) {
    local_b8 = 0x84;
    FUN_00467cc0(param_1,&param_3,&local_b8);
    return param_1;
  }
  FUN_00469640(local_28,param_2);
  param_2 = param_2 + 9;
  local_c = local_c + 1;
  goto LAB_00469bad;
}

