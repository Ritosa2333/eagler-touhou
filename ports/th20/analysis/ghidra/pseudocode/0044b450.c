/* Entry: 0x0044b450; symbol: FUN_0044b450; body bytes: 139 */

short * __cdecl FUN_0044b450(short *param_1,short *param_2)

{
  short *psVar1;
  short *local_8;
  
  psVar1 = param_2;
  if ((param_1 != param_2) && (local_8 = param_2 + -1, param_1 != local_8)) {
    if (*local_8 == 0x2e) {
      psVar1 = local_8;
      if ((param_1 == param_2 + -2) && (param_2[-2] == 0x2e)) {
        psVar1 = param_2;
      }
    }
    else {
      do {
        local_8 = local_8 + -1;
        if (param_1 == local_8) {
          return param_2;
        }
        psVar1 = local_8;
      } while (*local_8 != 0x2e);
    }
  }
  return psVar1;
}

