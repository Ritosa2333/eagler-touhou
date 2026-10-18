/* Entry: 0x00468ca0; symbol: FUN_00468ca0; body bytes: 667 */

undefined8 __cdecl FUN_00468ca0(char *param_1,char *param_2,undefined4 param_3,size_t param_4)

{
  char *pcVar1;
  uint uVar2;
  undefined8 uVar3;
  char *local_78;
  undefined4 local_74;
  uint local_30;
  int local_20;
  uint local_1c;
  int local_14;
  uint local_10;
  uint local_c;
  char local_5;
  
  if ((int)param_4 < 0) {
    param_4 = 6;
  }
  uVar2 = FUN_0040c990(&param_3);
  local_c = (uVar2 & 0x7fffff) << 1;
  if (uVar2 >> 0x17 == 0) {
    if ((uVar2 & 0x7fffff) == 0) {
      local_10 = 0;
    }
    else {
      local_10 = 0xffffff82;
    }
  }
  else {
    local_c = local_c | 0x1000000;
    local_10 = (uVar2 >> 0x17) - 0x7f;
  }
  if ((int)local_10 < 0) {
    local_5 = '-';
    local_1c = -local_10;
  }
  else {
    local_5 = '+';
    local_1c = local_10;
  }
  if (local_1c < 10) {
    local_20 = 1;
  }
  else if (local_1c < 100) {
    local_20 = 2;
  }
  else {
    local_20 = 3;
  }
  if ((int)param_2 - (int)param_1 < (int)param_4) {
    local_74 = 0x84;
  }
  else {
    local_30 = (uint)(0 < (int)param_4);
    if ((int)(((int)param_2 - (int)param_1) - param_4) < (int)(local_30 + 3 + local_20)) {
      local_74 = 0x84;
    }
    else {
      if ((int)param_4 < 6) {
        local_c = local_c + ((local_c * 2 - 1 | local_c) & local_c * 2 &
                            1 << (('\x06' - (char)param_4) * '\x04' & 0x1fU));
      }
      *param_1 = (char)(local_c >> 0x18) + '0';
      local_c = local_c & 0xffffff;
      pcVar1 = param_1 + 1;
      if (0 < (int)param_4) {
        param_1[1] = '.';
        param_1 = param_1 + 2;
        local_14 = 0x18;
        while( true ) {
          local_14 = local_14 + -4;
          *param_1 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                     [local_c >> ((byte)local_14 & 0x1f)];
          param_1 = param_1 + 1;
          param_4 = param_4 - 1;
          pcVar1 = param_1;
          if (param_4 == 0) break;
          if (local_14 == 0) {
            _memset(param_1,0x30,param_4);
            pcVar1 = param_1 + param_4;
            break;
          }
          local_c = local_c & (1 << ((byte)local_14 & 0x1f)) - 1U;
        }
      }
      param_1 = pcVar1;
      *param_1 = 'p';
      param_1[1] = local_5;
      uVar3 = FUN_004710c0(param_1 + 2,param_2,local_1c,10);
      local_78 = (char *)uVar3;
      local_74 = (undefined4)((ulonglong)uVar3 >> 0x20);
      param_2 = local_78;
    }
  }
  return CONCAT44(local_74,param_2);
}

