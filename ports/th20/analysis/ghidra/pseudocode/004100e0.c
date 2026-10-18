/* Entry: 0x004100e0; symbol: FUN_004100e0; body bytes: 446 */

uint * __cdecl
FUN_004100e0(uint *param_1,uint param_2,byte param_3,char param_4,int param_5,uint param_6)

{
  uint *puVar1;
  uint *puVar2;
  uint local_1c;
  int local_18;
  int local_10;
  uint *local_c;
  uint *local_8;
  
  if ((int)param_2 % param_5 < (int)(param_5 + (param_5 >> 0x1f & 3U)) >> 2) {
    local_18 = (int)param_2 % param_5;
  }
  else {
    local_18 = 0;
  }
  if ((int)param_2 < (int)param_6) {
    local_1c = param_2;
  }
  else {
    local_1c = param_6;
  }
  local_8 = param_1;
  puVar2 = (uint *)FUN_0040d8c0("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\filectrl.cpp:50 u_char"
                                ,local_1c);
  param_2 = param_2 - ((param_2 & 1) + local_18);
  FUN_00543e20(puVar2,param_1,local_1c);
  local_c = puVar2;
  for (; (puVar1 = local_8, 0 < (int)param_2 && (0 < (int)param_6)); param_6 = param_6 - param_5) {
    if ((int)param_2 < param_5) {
      param_5 = param_2;
    }
    local_8 = (uint *)((int)local_8 + param_5 + -1);
    for (local_10 = (param_5 + 1) / 2; 0 < local_10; local_10 = local_10 + -1) {
      *(byte *)local_8 = (byte)*local_c ^ param_3;
      local_8 = (uint *)((int)local_8 + -2);
      param_3 = param_3 + param_4;
      local_c = (uint *)((int)local_c + 1);
    }
    local_8 = (uint *)((int)puVar1 + param_5 + -2);
    for (local_10 = param_5 / 2; 0 < local_10; local_10 = local_10 + -1) {
      *(byte *)local_8 = (byte)*local_c ^ param_3;
      local_8 = (uint *)((int)local_8 + -2);
      param_3 = param_3 + param_4;
      local_c = (uint *)((int)local_c + 1);
    }
    param_2 = param_2 - param_5;
    local_8 = (uint *)((int)puVar1 + param_5);
  }
  FUN_0040d840(puVar2);
  return param_1;
}

