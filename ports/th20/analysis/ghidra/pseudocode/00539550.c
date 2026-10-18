/* Entry: 0x00539550; symbol: FUN_00539550; body bytes: 1036 */

/* WARNING: Removing unreachable block (ram,0x005398f7) */

void __cdecl FUN_00539550(byte *param_1,int param_2,int *param_3)

{
  byte bVar1;
  byte *pbVar2;
  int local_40;
  int local_38;
  int local_34;
  uint local_30;
  uint local_2c;
  byte *local_28;
  int local_24;
  uint local_1c;
  byte *local_18;
  byte local_d;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_d = 0x80;
  bVar1 = 0;
  pbVar2 = (byte *)FUN_0041f610(param_2 << 1);
  if (pbVar2 != (byte *)0x0) {
    local_28 = param_1;
    *param_3 = 0;
    FUN_005399a0();
    local_2c = 1;
    for (local_24 = 0; local_24 < 0x12; local_24 = local_24 + 1) {
      if ((int)local_28 - (int)param_1 < param_2) {
        local_30 = (uint)*local_28;
        local_28 = local_28 + 1;
      }
      else {
        local_30 = 0xffffffff;
      }
      if (local_30 == 0xffffffff) break;
      (&DAT_005c6b39)[local_24] = (undefined1)local_30;
    }
    local_34 = local_24;
    FUN_005399d0(1);
    local_38 = 0;
    local_c = 0;
    local_18 = pbVar2;
    while (0 < local_34) {
      if (local_34 < local_38) {
        local_38 = local_34;
      }
      if (local_38 < 3) {
        local_40 = 1;
        bVar1 = local_d | bVar1;
        local_d = local_d >> 1;
        if (local_d == 0) {
          *local_18 = bVar1;
          local_18 = local_18 + 1;
          bVar1 = 0;
          local_d = 0x80;
        }
        for (local_1c = 0x80; local_1c != 0; local_1c = local_1c >> 1) {
          if (((byte)(&DAT_005c6b38)[local_2c] & local_1c) != 0) {
            bVar1 = local_d | bVar1;
          }
          local_d = local_d >> 1;
          if (local_d == 0) {
            *local_18 = bVar1;
            local_18 = local_18 + 1;
            bVar1 = 0;
            local_d = 0x80;
          }
        }
      }
      else {
        local_d = local_d >> 1;
        if (local_d == 0) {
          *local_18 = bVar1;
          local_18 = local_18 + 1;
          bVar1 = 0;
          local_d = 0x80;
        }
        for (local_1c = 0x1000; local_1c != 0; local_1c = local_1c >> 1) {
          if ((local_1c & local_c) != 0) {
            bVar1 = local_d | bVar1;
          }
          local_d = local_d >> 1;
          if (local_d == 0) {
            *local_18 = bVar1;
            local_18 = local_18 + 1;
            bVar1 = 0;
            local_d = 0x80;
          }
        }
        for (local_1c = 8; local_1c != 0; local_1c = local_1c >> 1) {
          if ((local_38 - 3U & local_1c) != 0) {
            bVar1 = local_d | bVar1;
          }
          local_d = local_d >> 1;
          if (local_d == 0) {
            *local_18 = bVar1;
            local_18 = local_18 + 1;
            bVar1 = 0;
            local_d = 0x80;
          }
        }
        local_40 = local_38;
      }
      for (local_24 = 0; local_24 < local_40; local_24 = local_24 + 1) {
        FUN_005394c0(local_2c + 0x12 & 0x1fff);
        if ((int)local_28 - (int)param_1 < param_2) {
          local_30 = (uint)*local_28;
          local_28 = local_28 + 1;
        }
        else {
          local_30 = 0xffffffff;
        }
        if (local_30 == 0xffffffff) {
          local_34 = local_34 + -1;
        }
        else {
          (&DAT_005c6b38)[local_2c + 0x12 & 0x1fff] = (undefined1)local_30;
        }
        local_2c = local_2c + 1 & 0x1fff;
        if (local_34 != 0) {
          local_38 = FUN_00539060(local_2c,(int *)&local_c);
        }
      }
    }
    local_d = local_d >> 1;
    if (local_d == 0) {
      *local_18 = bVar1;
      local_18 = local_18 + 1;
      bVar1 = 0;
      local_d = 0x80;
    }
    for (local_1c = 0x1000; local_1c != 0; local_1c = local_1c >> 1) {
      local_d = local_d >> 1;
      if (local_d == 0) {
        *local_18 = bVar1;
        local_18 = local_18 + 1;
        bVar1 = 0;
        local_d = 0x80;
      }
    }
    *param_3 = (int)local_18 - (int)pbVar2;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

