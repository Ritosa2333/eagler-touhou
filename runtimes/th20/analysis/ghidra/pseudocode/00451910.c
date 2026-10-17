/* Entry: 0x00451910; symbol: FUN_00451910; body bytes: 2745 */

void FUN_00451910(int *param_1)

{
  uint local_a4;
  uint local_a0;
  uint local_9c;
  uint local_98;
  uint local_94;
  uint local_90;
  uint local_8c;
  uint local_88;
  undefined1 *local_84;
  ushort *local_80;
  ushort *local_7c;
  ushort *local_78;
  int local_74;
  int local_70;
  int *local_6c;
  undefined4 local_68 [6];
  uint local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  uint local_40;
  uint local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  uint local_28;
  uint local_24;
  uint local_20;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_6c = (int *)0x0;
  (**(code **)(*param_1 + 0x48))(param_1,0,&local_6c);
  if (local_6c != (int *)0x0) {
    (**(code **)(*local_6c + 0x30))(local_6c,local_68);
    (**(code **)(*local_6c + 0x34))(local_6c,&local_74,0,0);
    switch(local_68[0]) {
    case 0:
    case 0x15:
      local_14 = 0;
      local_10 = 0;
      local_c = 0;
      for (local_88 = 0; local_88 < local_4c; local_88 = local_88 + 1) {
        local_84 = (undefined1 *)(local_74 * local_88 + local_70);
        for (local_98 = 0; local_98 < local_50; local_98 = local_98 + 1) {
          if (local_84[3] == '\0') {
            local_c = 0;
            local_10 = 0;
            local_14 = 0;
            local_3c = 0;
            if (local_98 != 0) {
              FUN_00451880((int *)&local_14,local_84 + -4,(int *)&local_3c);
            }
            if (local_98 < local_50 - 1) {
              FUN_00451880((int *)&local_14,local_84 + 4,(int *)&local_3c);
            }
            if (local_88 != 0) {
              FUN_00451880((int *)&local_14,
                           local_84 + ((int)(-local_74 + (-local_74 >> 0x1f & 3U)) >> 2) * 4,
                           (int *)&local_3c);
            }
            if (local_88 < local_4c - 1) {
              FUN_00451880((int *)&local_14,
                           local_84 + ((int)(local_74 + (local_74 >> 0x1f & 3U)) >> 2) * 4,
                           (int *)&local_3c);
            }
            if (1 < local_3c) {
              local_14 = local_14 / local_3c;
              local_10 = local_10 / local_3c;
              local_c = local_c / local_3c;
            }
            local_84[2] = (undefined1)local_14;
            local_84[1] = (undefined1)local_10;
            *local_84 = (undefined1)local_c;
          }
          local_84 = local_84 + 4;
        }
      }
      break;
    case 0x19:
      local_20 = 0;
      local_1c = 0;
      local_18 = 0;
      for (local_8c = 0; local_8c < local_4c; local_8c = local_8c + 1) {
        local_78 = (ushort *)(local_74 * local_8c + local_70);
        for (local_9c = 0; local_9c < local_50; local_9c = local_9c + 1) {
          if (-1 < (short)*local_78) {
            local_18 = 0;
            local_1c = 0;
            local_20 = 0;
            local_40 = 0;
            if (local_9c != 0) {
              FUN_00451640((int *)&local_20,local_78 + -1,(int *)&local_40);
            }
            if (local_9c < local_50 - 1) {
              FUN_00451640((int *)&local_20,local_78 + 1,(int *)&local_40);
            }
            if (local_8c != 0) {
              FUN_00451640((int *)&local_20,local_78 + -local_74 / 2,(int *)&local_40);
            }
            if (local_8c < local_4c - 1) {
              FUN_00451640((int *)&local_20,local_78 + local_74 / 2,(int *)&local_40);
            }
            if (1 < local_40) {
              local_20 = local_20 / local_40;
              local_1c = local_1c / local_40;
              local_18 = local_18 / local_40;
            }
            *local_78 = *local_78 & 0x83ff | ((byte)local_20 & 0x1f) << 10;
            *local_78 = *local_78 & 0xfc1f | ((byte)local_1c & 0x1f) << 5;
            *local_78 = *local_78 & 0xffe0 | (byte)local_18 & 0x1f;
          }
          local_78 = local_78 + 1;
        }
      }
      break;
    case 0x1a:
      local_2c = 0;
      local_28 = 0;
      local_24 = 0;
      for (local_90 = 0; local_90 < local_4c; local_90 = local_90 + 1) {
        local_7c = (ushort *)(local_74 * local_90 + local_70);
        for (local_a0 = 0; local_a0 < local_50; local_a0 = local_a0 + 1) {
          if (*local_7c >> 0xc == 0) {
            local_24 = 0;
            local_28 = 0;
            local_2c = 0;
            local_44 = 0;
            if (local_a0 != 0) {
              FUN_00451700((int *)&local_2c,local_7c + -1,(int *)&local_44);
            }
            if (local_a0 < local_50 - 1) {
              FUN_00451700((int *)&local_2c,local_7c + 1,(int *)&local_44);
            }
            if (local_90 != 0) {
              FUN_00451700((int *)&local_2c,local_7c + -local_74 / 2,(int *)&local_44);
            }
            if (local_90 < local_4c - 1) {
              FUN_00451700((int *)&local_2c,local_7c + local_74 / 2,(int *)&local_44);
            }
            if (1 < local_44) {
              local_2c = local_2c / local_44;
              local_28 = local_28 / local_44;
              local_24 = local_24 / local_44;
            }
            *local_7c = *local_7c & 0xf0ff | ((byte)local_2c & 0xf) << 8;
            *local_7c = *local_7c & 0xff0f | ((byte)local_28 & 0xf) << 4;
            *local_7c = *local_7c & 0xfff0 | (byte)local_24 & 0xf;
          }
          local_7c = local_7c + 1;
        }
      }
      break;
    case 0x1d:
      local_38 = 0;
      local_34 = 0;
      local_30 = 0;
      for (local_94 = 0; local_94 < local_4c; local_94 = local_94 + 1) {
        local_80 = (ushort *)(local_74 * local_94 + local_70);
        for (local_a4 = 0; local_a4 < local_50; local_a4 = local_a4 + 1) {
          if (*local_80 >> 8 == 0) {
            local_30 = 0;
            local_34 = 0;
            local_38 = 0;
            local_48 = 0;
            if (local_a4 != 0) {
              FUN_004517c0((int *)&local_38,local_80 + -1,(int *)&local_48);
            }
            if (local_a4 < local_50 - 1) {
              FUN_004517c0((int *)&local_38,local_80 + 1,(int *)&local_48);
            }
            if (local_94 != 0) {
              FUN_004517c0((int *)&local_38,local_80 + -local_74 / 2,(int *)&local_48);
            }
            if (local_94 < local_4c - 1) {
              FUN_004517c0((int *)&local_38,local_80 + local_74 / 2,(int *)&local_48);
            }
            if (1 < local_48) {
              local_38 = local_38 / local_48;
              local_34 = local_34 / local_48;
              local_30 = local_30 / local_48;
            }
            *local_80 = *local_80 & 0xff1f | ((byte)local_38 & 7) << 5;
            *local_80 = *local_80 & 0xffe3 | ((byte)local_34 & 7) << 2;
            *local_80 = *local_80 & 0xfffc | (byte)local_30 & 3;
          }
          local_80 = local_80 + 1;
        }
      }
    }
    (**(code **)(*local_6c + 0x38))(local_6c);
    (**(code **)(*local_6c + 8))(local_6c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

