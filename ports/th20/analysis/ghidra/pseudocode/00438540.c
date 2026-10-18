/* Entry: 0x00438540; symbol: FUN_00438540; body bytes: 187 */

float10 __cdecl FUN_00438540(float param_1)

{
  undefined4 local_c;
  undefined4 local_8;
  
  if (param_1 <= DAT_0056e0f0) {
    if (param_1 < DAT_0056e0f8) {
      local_c = 0;
      do {
        param_1 = DAT_0056e0f0 * DAT_0056c8d0 + param_1;
        if (0x20 < local_c) break;
        local_c = local_c + 1;
      } while (param_1 < DAT_0056e0f8);
    }
  }
  else {
    local_8 = 0;
    do {
      param_1 = param_1 - DAT_0056e0f0 * DAT_0056c8d0;
      if (0x20 < local_8) break;
      local_8 = local_8 + 1;
    } while (DAT_0056e0f0 < param_1);
  }
  return (float10)param_1;
}

