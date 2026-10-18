/* Entry: 0x00420510; symbol: FUN_00420510; body bytes: 105 */

uint __cdecl FUN_00420510(uint *param_1,short param_2,uint param_3,int param_4)

{
  uint local_c;
  uint local_8;
  
  if (param_2 < 0) {
    local_c = 0;
  }
  else {
    if ((*(byte *)(param_4 + param_2) & 0x80) == 0) {
      local_8 = 0;
    }
    else {
      local_8 = param_3;
    }
    *param_1 = *param_1 | local_8;
    local_c = param_3;
    if ((*(byte *)(param_4 + param_2) & 0x80) == 0) {
      local_c = 0;
    }
  }
  return local_c;
}

