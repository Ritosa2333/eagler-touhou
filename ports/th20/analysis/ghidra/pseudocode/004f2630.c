/* Entry: 0x004f2630; symbol: FUN_004f2630; body bytes: 2632 */

/* WARNING: Type propagation algorithm not settling */

void __cdecl
FUN_004f2630(void *param_1,uint *param_2,char *param_3,uint param_4,uint param_5,size_t param_6,
            undefined4 param_7,int param_8,undefined8 param_9)

{
  uint uVar1;
  int iVar2;
  ulonglong uVar3;
  longlong lVar4;
  int local_110;
  int local_10c;
  int local_108;
  int local_104;
  uint local_100;
  uint local_fc;
  undefined4 local_f4;
  int local_f0 [3];
  int local_e4;
  int local_e0;
  int local_dc;
  int local_d8;
  int local_d4;
  int local_d0;
  int local_cc;
  undefined4 local_c8;
  int local_c4 [3];
  int local_b8;
  int local_b4;
  int local_b0;
  int local_ac;
  int local_a8;
  int local_a4;
  int local_a0;
  undefined8 local_9c;
  undefined8 local_94;
  int local_8c;
  int local_88;
  int local_84;
  uint local_80;
  uint local_7c;
  uint local_78;
  int local_74;
  uint local_70;
  uint local_6c;
  uint local_68;
  uint local_64;
  undefined4 local_60;
  size_t local_5c;
  uint local_58;
  int local_54;
  uint local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  byte local_3e [2];
  uint local_3c;
  uint local_38;
  char local_31;
  int local_30;
  uint local_2c;
  uint local_28;
  int local_24;
  size_t local_20;
  int local_1c;
  uint *local_18;
  uint local_14;
  uint *local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_3c = param_4;
  local_38 = param_5;
  local_20 = param_6;
  local_14 = FUN_004fd950(param_4,param_5);
  local_30 = (local_20 - 1) + local_14;
  if (param_8 == 0) {
    if (local_14 == 1) {
      local_8c = -3;
      local_54 = 4;
    }
    else {
      local_8c = -(local_14 + 3);
      local_54 = 5;
    }
    if (((int)local_20 < local_8c) || (local_54 < (int)local_20)) {
      param_8 = 1;
    }
    else {
      param_8 = 2;
    }
  }
  else if (param_8 == 3) {
    if ((local_30 < -4) || (5 < local_30)) {
      param_8 = 1;
    }
    else {
      param_8 = 2;
    }
  }
  if (param_8 == 2) {
    local_48 = local_14 + local_20;
    if ((int)local_20 < 0) {
      if ((int)local_48 < 1) {
        local_44 = 2 - local_20;
      }
      else {
        local_44 = local_14 + 1;
      }
    }
    else {
      local_44 = local_48;
      if ((local_3c == 1) && (local_38 == 0)) {
        local_44 = local_48 - (byte)(&DAT_005729b8)[local_20];
      }
    }
    if ((int)param_3 - (int)param_2 < (int)local_44) {
      local_f4 = 0x84;
      FUN_00467cc0(param_1,&param_3,&local_f4);
    }
    else {
      uVar1 = local_44;
      if (0 < (int)local_20) {
        if ((int)local_20 < 0x17) {
          local_58 = param_4;
          if (param_4 == 0) {
            uVar3 = __aullshr(0x20,param_5);
            local_c = 0;
            if ((uint)uVar3 != 0) {
              for (; ((uint)uVar3 >> local_c & 1) == 0; local_c = local_c + 1) {
              }
            }
            local_c = local_c + 0x20;
          }
          else {
            local_c = 0;
            if (param_4 != 0) {
              for (; (param_4 >> local_c & 1) == 0; local_c = local_c + 1) {
              }
            }
          }
          uVar3 = __aullshr((byte)local_c,param_5);
          local_5c = local_20;
          local_fc = (uint)(uVar3 >> 0x20);
          if ((*(uint *)(&DAT_00572af4 + local_20 * 8) < local_fc) ||
             ((*(uint *)(&DAT_00572af4 + local_20 * 8) <= local_fc &&
              (local_100 = (uint)uVar3, *(uint *)(&DAT_00572af0 + local_20 * 8) < local_100)))) {
            local_60 = 0;
          }
          else {
            local_60 = 1;
          }
          local_31 = (char)local_60;
        }
        else {
          local_31 = '\0';
        }
        uVar1 = local_14;
        if (local_31 == '\0') {
          FUN_00469720(param_1,(char *)param_2,param_3,param_9,0);
          goto LAB_004f306a;
        }
      }
      local_10 = (uint *)((int)param_2 + uVar1);
      uVar3 = __aullshr(0x20,local_38);
      local_108 = (int)uVar3;
      local_104 = (int)(uVar3 >> 0x20);
      if (local_108 != 0 || local_104 != 0) {
        uVar3 = FUN_004fdb50(local_3c,local_38);
        local_94._4_4_ = (uint)(uVar3 >> 0x20);
        iVar2 = local_94._4_4_;
        local_94._0_4_ = (uint)uVar3;
        uVar1 = (uint)local_94;
        local_94 = uVar3;
        lVar4 = __allmul(uVar1,iVar2,100000000,0);
        local_4c = local_3c - (int)lVar4;
        local_3c = (uint)local_94;
        local_38 = local_94._4_4_;
        local_64 = local_4c % 10000;
        local_4c = local_4c / 10000;
        local_68 = local_4c % 10000;
        local_a0 = (local_64 % 100) * 2;
        local_a4 = local_64 / 100 << 1;
        local_a8 = local_68 % 100 << 1;
        local_ac = local_68 / 100 << 1;
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_a0),2);
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_a4),2);
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_a8),2);
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_ac),2);
      }
      local_28 = local_3c;
      while (9999 < local_28) {
        local_6c = local_28 % 10000;
        local_28 = local_28 / 10000;
        local_b0 = (local_6c % 100) * 2;
        local_b4 = local_6c / 100 << 1;
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_b0),2);
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_b4),2);
      }
      if (99 < local_28) {
        local_b8 = (local_28 % 100) * 2;
        local_28 = local_28 / 100;
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_b8),2);
      }
      if (local_28 < 10) {
        local_10 = (uint *)((int)local_10 + -1);
        *(char *)local_10 = (char)local_28 + '0';
      }
      else {
        local_c4[2] = local_28 * 2;
        local_10 = (uint *)((int)local_10 + -2);
        FUN_00543e20(local_10,(uint *)(&DAT_0056f350 + local_c4[2]),2);
      }
      if ((int)local_20 < 1) {
        if (local_20 != 0) {
          if ((int)local_48 < 1) {
            *(undefined1 *)param_2 = 0x30;
            *(undefined1 *)((int)param_2 + 1) = 0x2e;
            local_3e[0] = 0x30;
            FUN_0046a230((int)param_2 + 2,-local_48,local_3e);
          }
          else {
            FUN_00543e20(param_2,(uint *)((int)param_2 + 1),local_48);
            *(undefined1 *)((int)param_2 + local_48) = 0x2e;
          }
        }
      }
      else {
        local_3e[1] = 0x30;
        FUN_0046a230((int)param_2 + local_14,local_20,local_3e + 1);
      }
      local_c4[0] = (int)param_2 + local_44;
      local_c4[1] = 0;
      FUN_00467cc0(param_1,local_c4,local_c4 + 1);
    }
  }
  else {
    local_70 = (uint)(1 < local_14);
    if ((local_30 < -99) || (99 < local_30)) {
      local_74 = 5;
    }
    else {
      local_74 = 4;
    }
    local_88 = local_14 + local_70 + local_74;
    if ((int)param_3 - (int)param_2 < local_88) {
      local_c8 = 0x84;
      FUN_00467cc0(param_1,&param_3,&local_c8);
    }
    else {
      local_18 = param_2;
      local_24 = 0;
      uVar3 = __aullshr(0x20,local_38);
      local_110 = (int)uVar3;
      local_10c = (int)(uVar3 >> 0x20);
      if (local_110 != 0 || local_10c != 0) {
        uVar3 = FUN_004fdb50(local_3c,local_38);
        local_9c._0_4_ = (uint)uVar3;
        local_50 = local_3c + (uint)local_9c * -100000000;
        local_9c._4_4_ = (uint)(uVar3 >> 0x20);
        local_3c = (uint)local_9c;
        local_38 = local_9c._4_4_;
        local_78 = local_50 % 10000;
        local_50 = local_50 / 10000;
        local_7c = local_50 % 10000;
        local_cc = (local_78 % 100) * 2;
        local_d0 = local_78 / 100 << 1;
        local_d4 = local_7c % 100 << 1;
        local_d8 = local_7c / 100 << 1;
        local_9c = uVar3;
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -1),
                     (uint *)(&DAT_0056f350 + local_cc),2);
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -3),
                     (uint *)(&DAT_0056f350 + local_d0),2);
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -5),
                     (uint *)(&DAT_0056f350 + local_d4),2);
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -7),
                     (uint *)(&DAT_0056f350 + local_d8),2);
        local_24 = local_24 + 8;
      }
      local_2c = local_3c;
      while (9999 < local_2c) {
        local_80 = local_2c % 10000;
        local_2c = local_2c / 10000;
        local_dc = (local_80 % 100) * 2;
        local_e0 = local_80 / 100 << 1;
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -1),
                     (uint *)(&DAT_0056f350 + local_dc),2);
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -3),
                     (uint *)(&DAT_0056f350 + local_e0),2);
        local_24 = local_24 + 4;
      }
      if (99 < local_2c) {
        local_e4 = (local_2c % 100) * 2;
        local_2c = local_2c / 100;
        FUN_00543e20((uint *)((int)local_18 + (local_14 - local_24) + -1),
                     (uint *)(&DAT_0056f350 + local_e4),2);
        local_24 = local_24 + 2;
      }
      if (local_2c < 10) {
        *(char *)local_18 = (char)local_2c + '0';
      }
      else {
        local_84 = local_2c * 2;
        *(undefined *)((int)local_18 + 2) = (&DAT_0056f351)[local_84];
        *(undefined *)local_18 = (&DAT_0056f350)[local_84];
      }
      if (local_14 < 2) {
        local_1c = 1;
      }
      else {
        *(char *)((int)local_18 + 1) = '.';
        local_1c = local_14 + 1;
      }
      *(char *)((int)local_18 + local_1c) = 'e';
      if (local_30 < 0) {
        *(char *)((int)local_18 + local_1c + 1) = '-';
        local_30 = -local_30;
      }
      else {
        *(char *)((int)local_18 + local_1c + 1) = '+';
      }
      local_1c = local_1c + 2;
      if (local_30 < 100) {
        FUN_00543e20((uint *)((int)local_18 + local_1c),(uint *)(&DAT_0056f350 + local_30 * 2),2);
        local_1c = local_1c + 2;
      }
      else {
        local_f0[2] = local_30 % 10;
        FUN_00543e20((uint *)((int)local_18 + local_1c),
                     (uint *)(&DAT_0056f350 + (local_30 / 10) * 2),2);
        *(char *)((int)local_18 + local_1c + 2) = (char)local_f0[2] + '0';
        local_1c = local_1c + 3;
      }
      local_f0[0] = (int)param_2 + local_88;
      local_f0[1] = 0;
      FUN_00467cc0(param_1,local_f0,local_f0 + 1);
    }
  }
LAB_004f306a:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

