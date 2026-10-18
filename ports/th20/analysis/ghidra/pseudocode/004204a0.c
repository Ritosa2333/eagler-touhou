/* Entry: 0x004204a0; symbol: FUN_004204a0; body bytes: 98 */

uint __cdecl FUN_004204a0(uint *param_1,short param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint local_10;
  uint local_8;
  
  if (param_2 < 0) {
    local_10 = 0;
  }
  else {
    uVar1 = 1 << ((byte)param_2 & 0x1f);
    if ((param_4 & uVar1) == 0) {
      local_8 = 0;
    }
    else {
      local_8 = param_3;
    }
    *param_1 = *param_1 | local_8;
    local_10 = param_3;
    if ((param_4 & uVar1) == 0) {
      local_10 = 0;
    }
  }
  return local_10;
}

