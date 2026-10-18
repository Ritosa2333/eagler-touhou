/* Entry: 0x00539060; symbol: FUN_00539060; body bytes: 284 */

int __cdecl FUN_00539060(int param_1,int *param_2)

{
  int local_18;
  int *local_14;
  int local_10;
  int local_c;
  int local_8;
  
  local_18 = 0;
  if (param_1 == 0) {
    local_10 = 0;
  }
  else {
    local_c = DAT_005e0b40;
    local_10 = 0;
    while( true ) {
      local_8 = 0;
      while ((local_8 < 0x12 &&
             (local_18 = (uint)(byte)(&DAT_005c6b38)[param_1 + local_8 & 0x1fff] -
                         (uint)(byte)(&DAT_005c6b38)[local_c + local_8 & 0x1fff], local_18 == 0))) {
        local_8 = local_8 + 1;
      }
      if (local_10 <= local_8) {
        local_10 = local_8;
        *param_2 = local_c;
        if (0x11 < local_8) {
          FUN_00539a10(local_c,param_1);
          return local_8;
        }
      }
      if (local_18 < 0) {
        local_14 = (int *)(&DAT_005c8b3c + local_c * 0xc);
      }
      else {
        local_14 = (int *)(&DAT_005c8b40 + local_c * 0xc);
      }
      if (*local_14 == 0) break;
      local_c = *local_14;
    }
    *local_14 = param_1;
    *(int *)(&DAT_005c8b38 + param_1 * 0xc) = local_c;
    *(undefined4 *)(&DAT_005c8b40 + param_1 * 0xc) = 0;
    *(undefined4 *)(&DAT_005c8b3c + param_1 * 0xc) = 0;
  }
  return local_10;
}

