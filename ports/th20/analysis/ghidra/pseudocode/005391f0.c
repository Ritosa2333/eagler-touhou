/* Entry: 0x005391f0; symbol: FUN_005391f0; body bytes: 706 */

undefined1 * __cdecl FUN_005391f0(byte *param_1,int param_2,undefined1 *param_3,SIZE_T param_4)

{
  byte bVar1;
  undefined1 uVar2;
  uint uVar3;
  byte bVar4;
  int local_28;
  uint local_20;
  undefined1 *local_1c;
  uint local_14;
  uint local_10;
  byte *local_c;
  byte local_5;
  
  local_5 = 0x80;
  bVar1 = 0;
  if ((param_3 == (undefined1 *)0x0) &&
     (param_3 = (undefined1 *)FUN_0041f610(param_4), param_3 == (undefined1 *)0x0)) {
    param_3 = (undefined1 *)0x0;
  }
  else {
    local_c = param_1;
    local_1c = param_3;
    local_20 = 1;
    while( true ) {
      while( true ) {
        if (local_5 == 0x80) {
          bVar1 = *local_c;
          if ((int)local_c - (int)param_1 < param_2) {
            local_c = local_c + 1;
          }
          else {
            bVar1 = 0;
          }
        }
        bVar4 = local_5 & bVar1;
        local_5 = local_5 >> 1;
        if (local_5 == 0) {
          local_5 = 0x80;
        }
        if (bVar4 == 0) break;
        local_10 = 0x80;
        local_14 = 0;
        while (local_10 != 0) {
          if (local_5 == 0x80) {
            bVar1 = *local_c;
            if ((int)local_c - (int)param_1 < param_2) {
              local_c = local_c + 1;
            }
            else {
              bVar1 = 0;
            }
          }
          if ((local_5 & bVar1) != 0) {
            local_14 = local_14 | local_10;
          }
          local_10 = local_10 >> 1;
          local_5 = local_5 >> 1;
          if (local_5 == 0) {
            local_5 = 0x80;
          }
        }
        *local_1c = (undefined1)local_14;
        local_1c = local_1c + 1;
        (&DAT_005c6b38)[local_20] = (undefined1)local_14;
        local_20 = local_20 + 1 & 0x1fff;
      }
      local_10 = 0x1000;
      local_14 = 0;
      while (uVar3 = local_14, local_10 != 0) {
        if (local_5 == 0x80) {
          bVar1 = *local_c;
          if ((int)local_c - (int)param_1 < param_2) {
            local_c = local_c + 1;
          }
          else {
            bVar1 = 0;
          }
        }
        if ((local_5 & bVar1) != 0) {
          local_14 = local_14 | local_10;
        }
        local_10 = local_10 >> 1;
        local_5 = local_5 >> 1;
        if (local_5 == 0) {
          local_5 = 0x80;
        }
      }
      if (local_14 == 0) break;
      local_10 = 8;
      local_14 = 0;
      while (local_10 != 0) {
        if (local_5 == 0x80) {
          bVar1 = *local_c;
          if ((int)local_c - (int)param_1 < param_2) {
            local_c = local_c + 1;
          }
          else {
            bVar1 = 0;
          }
        }
        if ((local_5 & bVar1) != 0) {
          local_14 = local_14 | local_10;
        }
        local_10 = local_10 >> 1;
        local_5 = local_5 >> 1;
        if (local_5 == 0) {
          local_5 = 0x80;
        }
      }
      for (local_28 = 0; local_28 <= (int)(local_14 + 2); local_28 = local_28 + 1) {
        uVar2 = (&DAT_005c6b38)[uVar3 + local_28 & 0x1fff];
        *local_1c = uVar2;
        local_1c = local_1c + 1;
        (&DAT_005c6b38)[local_20] = uVar2;
        local_20 = local_20 + 1 & 0x1fff;
      }
    }
  }
  return param_3;
}

