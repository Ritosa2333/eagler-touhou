/* Entry: 0x004ed320; symbol: FUN_004ed320; body bytes: 626 */

void __cdecl FUN_004ed320(void *param_1,char *param_2,int param_3,uint param_4,uint param_5)

{
  sbyte sVar1;
  longlong lVar2;
  ulonglong uVar3;
  uint auStack_74 [4];
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  int local_58;
  uint local_54;
  int local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  int local_40;
  uint local_3c;
  int local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  int local_28;
  int local_24;
  int local_20;
  char *local_1c;
  uint local_18 [5];
  
  local_18[4] = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_64 = 4;
  local_18[0] = 0;
  local_18[1] = 0;
  local_18[2] = 0;
  local_18[3] = 0;
  local_20 = (param_5 + 0x37 >> 5) - 1;
  local_34 = param_5 % 0x20;
  sVar1 = (sbyte)local_34;
  if (local_34 < 9) {
    local_18[local_20] = param_4 << sVar1;
  }
  else {
    (&local_1c)[local_20] = (char *)(param_4 << sVar1);
    local_18[local_20] = param_4 >> (0x20U - sVar1 & 0x1f);
  }
  local_28 = 0;
  while (local_20 != 0) {
    do {
      local_44 = local_18[local_20];
      local_54 = local_44 % 1000000000;
      local_4c = local_44 / 1000000000;
      local_18[local_20] = local_4c;
      local_24 = local_20;
      local_30 = local_54;
      do {
        local_2c = 0;
        local_24 = local_24 + -1;
        lVar2 = __allshl(0x20,0);
        local_2c = (uint)((ulonglong)lVar2 >> 0x20);
        local_30 = (uint)lVar2 | local_18[local_24];
        uVar3 = FUN_0046fbd0(local_30,local_2c);
        local_48 = (uint)uVar3;
        local_30 = local_30 + local_48 * -1000000000;
        local_2c = 0;
        local_18[local_24] = local_48;
      } while (local_24 != 0);
      auStack_74[local_28] = local_30;
      local_28 = local_28 + 1;
    } while (local_4c != 0);
    local_20 = local_20 + -1;
    local_4c = 0;
  }
  for (local_3c = 1; local_3c < 4; local_3c = local_3c + 1) {
  }
  if (local_18[0] < 1000000000) {
    local_50 = FUN_0046fb40(local_18[0]);
  }
  else {
    local_50 = 10;
  }
  local_40 = local_50;
  local_58 = local_28 * 9 + local_50;
  if (param_3 - (int)param_2 < local_58) {
    local_5c = 0x84;
    FUN_00467cc0(param_1,&param_3,&local_5c);
  }
  else {
    local_1c = param_2;
    FUN_004694f0(local_50,local_18[0],param_2);
    local_1c = local_1c + local_40;
    for (local_38 = local_28 + -1; -1 < local_38; local_38 = local_38 + -1) {
      FUN_00469640(auStack_74[local_38],local_1c);
      local_1c = local_1c + 9;
    }
    local_60 = 0;
    FUN_00467cc0(param_1,&local_1c,&local_60);
  }
  FUN_005429ee(local_18[4] ^ (uint)&stack0xfffffffc);
  return;
}

