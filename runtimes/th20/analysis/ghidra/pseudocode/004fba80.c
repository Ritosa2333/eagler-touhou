/* Entry: 0x004fba80; symbol: FUN_004fba80; body bytes: 713 */

undefined8 __cdecl FUN_004fba80(byte *param_1,byte *param_2,uint *param_3)

{
  uint local_58;
  uint local_50;
  uint local_48;
  uint local_40;
  uint local_38;
  uint local_30;
  uint local_28;
  uint local_20;
  uint local_18;
  int local_c;
  int local_8;
  
  *param_3 = (uint)*param_1;
  if (*param_3 < 0x80) {
    param_2 = param_1 + 1;
    local_18 = CONCAT31(local_18._1_3_,1);
    goto LAB_004fbd47;
  }
  if ((*param_3 < 0xc2) || (0xdf < *param_3)) {
    if ((*param_3 < 0xe0) || (0xef < *param_3)) {
      if ((*param_3 < 0xf0) || (0xf4 < *param_3)) {
        *param_3 = 0xfffd;
        param_2 = param_1 + 1;
        local_20 = local_20 & 0xffffff00;
        local_18 = local_20;
        goto LAB_004fbd47;
      }
      local_c = 4;
    }
    else {
      local_c = 3;
    }
  }
  else {
    local_c = 2;
  }
  if (param_1 + 1 == param_2) {
    *param_3 = 0xfffd;
    local_28 = local_28 & 0xffffff00;
    local_18 = local_28;
    goto LAB_004fbd47;
  }
  switch(*param_3) {
  case 0xe0:
    if (param_1[1] < 0xa0) {
      *param_3 = 0xfffd;
      param_2 = param_1 + 1;
      local_30 = local_30 & 0xffffff00;
      local_18 = local_30;
      goto LAB_004fbd47;
    }
    break;
  case 0xed:
    if (0x9f < param_1[1]) {
      *param_3 = 0xfffd;
      param_2 = param_1 + 1;
      local_38 = local_38 & 0xffffff00;
      local_18 = local_38;
      goto LAB_004fbd47;
    }
    break;
  case 0xf0:
    if (param_1[1] < 0x90) {
      *param_3 = 0xfffd;
      param_2 = param_1 + 1;
      local_40 = local_40 & 0xffffff00;
      local_18 = local_40;
      goto LAB_004fbd47;
    }
    break;
  case 0xf4:
    if (0x8f < param_1[1]) {
      *param_3 = 0xfffd;
      param_2 = param_1 + 1;
      local_48 = local_48 & 0xffffff00;
      local_18 = local_48;
      goto LAB_004fbd47;
    }
  }
  if (local_c == 2) {
LAB_004fbc88:
    *param_3 = *param_3 & 0x1f;
  }
  else if (local_c == 3) {
    *param_3 = *param_3 & 0xf;
  }
  else {
    if (local_c != 4) goto LAB_004fbc88;
    *param_3 = *param_3 & 7;
  }
  for (local_8 = 1; local_8 < local_c; local_8 = local_8 + 1) {
    if (((param_2 <= param_1 + local_8) || (param_1[local_8] < 0x80)) || (0xbf < param_1[local_8]))
    {
      *param_3 = 0xfffd;
      param_2 = param_1 + local_8;
      local_50 = local_50 & 0xffffff00;
      local_18 = local_50;
      goto LAB_004fbd47;
    }
    *param_3 = *param_3 << 6 | param_1[local_8] & 0x3f;
  }
  param_2 = param_1 + local_c;
  local_58 = CONCAT31(local_58._1_3_,1);
  local_18 = local_58;
LAB_004fbd47:
  return CONCAT44(local_18,param_2);
}

