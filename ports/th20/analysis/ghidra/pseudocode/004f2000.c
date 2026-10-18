/* Entry: 0x004f2000; symbol: FUN_004f2000; body bytes: 1575 */

/* WARNING: Type propagation algorithm not settling */

void __cdecl
FUN_004f2000(void *param_1,uint *param_2,int param_3,uint param_4,size_t param_5,int param_6,
            uint param_7,int param_8)

{
  int local_9c [3];
  int local_90;
  int local_8c;
  undefined4 local_88;
  int local_84 [3];
  int local_78;
  int local_74;
  int local_70;
  uint local_6c;
  uint local_68;
  uint local_64;
  undefined4 local_60;
  int local_5c;
  int local_58;
  int local_54;
  uint local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  int local_40;
  int local_3c;
  uint local_38;
  int local_34;
  uint local_30;
  byte local_2a [2];
  uint *local_28;
  bool local_21;
  int local_20;
  size_t local_1c;
  uint local_18;
  uint *local_14;
  uint local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_4;
  local_1c = param_5;
  local_18 = FUN_0046fb40(param_4);
  local_3c = (local_1c - 1) + local_18;
  if (param_6 == 0) {
    if (local_18 == 1) {
      local_5c = -3;
      local_40 = 4;
    }
    else {
      local_5c = -(local_18 + 3);
      local_40 = 5;
    }
    if (((int)local_1c < local_5c) || (local_40 < (int)local_1c)) {
      param_6 = 1;
    }
    else {
      param_6 = 2;
    }
  }
  else if (param_6 == 3) {
    if ((local_3c < -4) || (5 < local_3c)) {
      param_6 = 1;
    }
    else {
      param_6 = 2;
    }
  }
  if (param_6 == 2) {
    local_38 = local_18 + local_1c;
    if ((int)local_1c < 0) {
      if ((int)local_38 < 1) {
        local_30 = 2 - local_1c;
      }
      else {
        local_30 = local_18 + 1;
      }
    }
    else {
      local_30 = local_38;
      if (local_10 == 1) {
        local_30 = local_38 - (byte)(&DAT_00572964)[local_1c];
      }
    }
    if (param_3 - (int)param_2 < (int)local_30) {
      local_60 = 0x84;
      FUN_00467cc0(param_1,&param_3,&local_60);
    }
    else {
      if ((int)local_1c < 1) {
        local_14 = (uint *)((int)param_2 + local_30);
      }
      else {
        if ((int)local_1c < 0xb) {
          local_c = 0;
          if (param_4 != 0) {
            for (; (param_4 >> local_c & 1) == 0; local_c = local_c + 1) {
            }
          }
          local_64 = param_4 >> ((byte)local_c & 0x1f);
          local_21 = local_64 <= *(uint *)(&DAT_0057298c + local_1c * 4);
          local_44 = (uint)local_21;
        }
        else {
          local_21 = false;
        }
        if (local_21 == false) {
          local_6c = param_7 | 0x800000;
          local_68 = param_8 - 0x96;
          FUN_004ed320(param_1,(char *)param_2,param_3,local_6c,local_68);
          goto LAB_004f2619;
        }
        local_14 = (uint *)((int)param_2 + local_18);
      }
      while (9999 < local_10) {
        local_48 = local_10 % 10000;
        local_10 = local_10 / 10000;
        local_70 = (local_48 % 100) * 2;
        local_74 = local_48 / 100 << 1;
        local_14 = (uint *)((int)local_14 + -2);
        FUN_00543e20(local_14,(uint *)(&DAT_0056f350 + local_70),2);
        local_14 = (uint *)((int)local_14 + -2);
        FUN_00543e20(local_14,(uint *)(&DAT_0056f350 + local_74),2);
      }
      if (99 < local_10) {
        local_78 = (local_10 % 100) * 2;
        local_10 = local_10 / 100;
        local_14 = (uint *)((int)local_14 + -2);
        FUN_00543e20(local_14,(uint *)(&DAT_0056f350 + local_78),2);
      }
      if (local_10 < 10) {
        local_14 = (uint *)((int)local_14 + -1);
        *(char *)local_14 = (char)local_10 + '0';
      }
      else {
        local_84[2] = local_10 * 2;
        local_14 = (uint *)((int)local_14 + -2);
        FUN_00543e20(local_14,(uint *)(&DAT_0056f350 + local_84[2]),2);
      }
      if ((int)local_1c < 1) {
        if (local_1c != 0) {
          if ((int)local_38 < 1) {
            *(undefined1 *)param_2 = 0x30;
            *(undefined1 *)((int)param_2 + 1) = 0x2e;
            local_2a[0] = 0x30;
            FUN_0046a230((int)param_2 + 2,-local_38,local_2a);
          }
          else {
            FUN_00543e20(param_2,(uint *)((int)param_2 + 1),local_38);
            *(undefined1 *)((int)param_2 + local_38) = 0x2e;
          }
        }
      }
      else {
        local_2a[1] = 0x30;
        FUN_0046a230((int)param_2 + local_18,local_1c,local_2a + 1);
      }
      local_84[0] = (int)param_2 + local_30;
      local_84[1] = 0;
      FUN_00467cc0(param_1,local_84,local_84 + 1);
    }
  }
  else {
    local_4c = (uint)(1 < local_18);
    local_58 = local_18 + 4 + local_4c;
    if (param_3 - (int)param_2 < local_58) {
      local_88 = 0x84;
      FUN_00467cc0(param_1,&param_3,&local_88);
    }
    else {
      local_28 = param_2;
      local_34 = 0;
      while (9999 < local_10) {
        local_50 = local_10 % 10000;
        local_10 = local_10 / 10000;
        local_8c = (local_50 % 100) * 2;
        local_90 = local_50 / 100 << 1;
        FUN_00543e20((uint *)((int)local_28 + (local_18 - local_34) + -1),
                     (uint *)(&DAT_0056f350 + local_8c),2);
        FUN_00543e20((uint *)((int)local_28 + (local_18 - local_34) + -3),
                     (uint *)(&DAT_0056f350 + local_90),2);
        local_34 = local_34 + 4;
      }
      if (99 < local_10) {
        local_9c[2] = (local_10 % 100) * 2;
        local_10 = local_10 / 100;
        FUN_00543e20((uint *)((int)local_28 + (local_18 - local_34) + -1),
                     (uint *)(&DAT_0056f350 + local_9c[2]),2);
        local_34 = local_34 + 2;
      }
      if (local_10 < 10) {
        *(char *)local_28 = (char)local_10 + '0';
      }
      else {
        local_54 = local_10 * 2;
        *(undefined *)((int)local_28 + 2) = (&DAT_0056f351)[local_54];
        *(undefined *)local_28 = (&DAT_0056f350)[local_54];
      }
      if (local_18 < 2) {
        local_20 = 1;
      }
      else {
        *(char *)((int)local_28 + 1) = '.';
        local_20 = local_18 + 1;
      }
      *(char *)((int)local_28 + local_20) = 'e';
      if (local_3c < 0) {
        *(char *)((int)local_28 + local_20 + 1) = '-';
        local_3c = -local_3c;
      }
      else {
        *(char *)((int)local_28 + local_20 + 1) = '+';
      }
      local_20 = local_20 + 2;
      FUN_00543e20((uint *)((int)local_28 + local_20),(uint *)(&DAT_0056f350 + local_3c * 2),2);
      local_20 = local_20 + 2;
      local_9c[0] = (int)param_2 + local_58;
      local_9c[1] = 0;
      FUN_00467cc0(param_1,local_9c,local_9c + 1);
    }
  }
LAB_004f2619:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

