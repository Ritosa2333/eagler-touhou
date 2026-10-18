/* Entry: 0x004e8d20; symbol: FUN_004e8d20; body bytes: 851 */

undefined8 __cdecl
FUN_004e8d20(char *param_1,char *param_2,undefined4 param_3,undefined4 param_4,size_t param_5)

{
  char cVar1;
  char *pcVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  longlong lVar5;
  undefined8 uVar6;
  char *local_8c;
  undefined4 local_88;
  uint local_44;
  uint local_40;
  uint local_34;
  uint local_30;
  uint local_2c;
  int local_20;
  uint local_1c;
  int local_18;
  uint local_14;
  char local_d;
  uint local_8;
  
  if ((int)param_5 < 0) {
    param_5 = 0xd;
  }
  uVar3 = FUN_004e7ae0((undefined8 *)&param_3);
  local_34 = (uint)uVar3;
  local_30 = (uint)(uVar3 >> 0x20);
  local_8 = local_30 & 0xfffff;
  uVar4 = __aullshr(0x34,local_30);
  if ((int)uVar4 == 0) {
    if (local_34 == 0 && (uVar3 & 0xfffff00000000) == 0) {
      local_14 = 0;
    }
    else {
      local_14 = 0xfffffc02;
    }
  }
  else {
    local_8 = local_8 | 0x100000;
    local_14 = (int)uVar4 - 0x3ff;
  }
  if ((int)local_14 < 0) {
    local_d = '-';
    local_1c = -local_14;
  }
  else {
    local_d = '+';
    local_1c = local_14;
  }
  if (local_1c < 10) {
    local_20 = 1;
  }
  else if (local_1c < 100) {
    local_20 = 2;
  }
  else if (local_1c < 1000) {
    local_20 = 3;
  }
  else {
    local_20 = 4;
  }
  if ((int)param_2 - (int)param_1 < (int)param_5) {
    local_88 = 0x84;
  }
  else {
    local_2c = (uint)(0 < (int)param_5);
    if ((int)(((int)param_2 - (int)param_1) - param_5) < (int)(local_2c + 3 + local_20)) {
      local_88 = 0x84;
    }
    else {
      if ((int)param_5 < 0xd) {
        cVar1 = (char)param_5;
        lVar5 = __allshl(1,local_8);
        local_44 = (uint)lVar5;
        local_40 = (uint)((ulonglong)lVar5 >> 0x20);
        lVar5 = __allshl(('\r' - cVar1) * '\x04',0);
        local_8 = local_8 + ((local_40 - (local_44 == 0) | local_8) & local_40 &
                            (uint)((ulonglong)lVar5 >> 0x20)) +
                  (uint)CARRY4(local_34,(local_44 - 1 | local_34) & local_44 & (uint)lVar5);
      }
      uVar3 = __aullshr(0x34,local_8);
      *param_1 = (char)uVar3 + '0';
      local_8 = local_8 & 0xfffff;
      pcVar2 = param_1 + 1;
      if (0 < (int)param_5) {
        param_1[1] = '.';
        param_1 = param_1 + 2;
        local_18 = 0x34;
        while( true ) {
          local_18 = local_18 + -4;
          uVar3 = __aullshr((byte)local_18,local_8);
          *param_1 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                     [(int)uVar3];
          param_1 = param_1 + 1;
          param_5 = param_5 - 1;
          pcVar2 = param_1;
          if (param_5 == 0) break;
          if (local_18 == 0) {
            _memset(param_1,0x30,param_5);
            pcVar2 = param_1 + param_5;
            break;
          }
          lVar5 = __allshl((byte)local_18,0);
          local_8 = local_8 & (int)((ulonglong)lVar5 >> 0x20) - (uint)((int)lVar5 == 0);
        }
      }
      param_1 = pcVar2;
      *param_1 = 'p';
      param_1[1] = local_d;
      uVar6 = FUN_004710c0(param_1 + 2,param_2,local_1c,10);
      local_8c = (char *)uVar6;
      local_88 = (undefined4)((ulonglong)uVar6 >> 0x20);
      param_2 = local_8c;
    }
  }
  return CONCAT44(local_88,param_2);
}

