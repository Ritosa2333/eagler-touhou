/* Entry: 0x00420760; symbol: FUN_00420760; body bytes: 550 */

void __cdecl FUN_00420760(int param_1)

{
  uint *puVar1;
  MMRESULT MVar2;
  uint uVar3;
  uint local_5c;
  uint local_58;
  uint local_54;
  uint local_50;
  uint local_4c;
  joyinfoex_tag local_40;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = 0;
  puVar1 = &DAT_005b88b0 + param_1 * 0xb0;
  _memset(&local_40,0,0x34);
  local_40.dwSize = 0x34;
  local_40.dwFlags = 0xff;
  local_4c = (uint)(param_1 != 0);
  MVar2 = joyGetPosEx(local_4c,&local_40);
  if (MVar2 == 0) {
    FUN_004204a0(&local_c,0,1,local_40.dwButtons);
    FUN_004204a0(&local_c,1,1,local_40.dwButtons);
    FUN_004204a0(&local_c,2,1,local_40.dwButtons);
    FUN_004204a0(&local_c,3,1,local_40.dwButtons);
    uVar3 = (uint)(*(int *)(&DAT_005b9f38 + param_1 * 0x2d8) -
                  *(int *)(&DAT_005b9f34 + param_1 * 0x2d8)) >> 2;
    if (((uint)(*(int *)(&DAT_005b9f34 + param_1 * 0x2d8) +
               *(int *)(&DAT_005b9f38 + param_1 * 0x2d8)) >> 1) + uVar3 < local_40.dwXpos) {
      local_50 = 0x80;
    }
    else {
      local_50 = 0;
    }
    if (local_40.dwXpos <
        ((uint)(*(int *)(&DAT_005b9f34 + param_1 * 0x2d8) +
               *(int *)(&DAT_005b9f38 + param_1 * 0x2d8)) >> 1) - uVar3) {
      local_54 = 0x40;
    }
    else {
      local_54 = 0;
    }
    uVar3 = (uint)(*(int *)(&DAT_005b9f40 + param_1 * 0x2d8) -
                  *(int *)(&DAT_005b9f3c + param_1 * 0x2d8)) >> 2;
    if (((uint)(*(int *)(&DAT_005b9f3c + param_1 * 0x2d8) +
               *(int *)(&DAT_005b9f40 + param_1 * 0x2d8)) >> 1) + uVar3 < local_40.dwYpos) {
      local_58 = 0x20;
    }
    else {
      local_58 = 0;
    }
    if (local_40.dwYpos <
        ((uint)(*(int *)(&DAT_005b9f3c + param_1 * 0x2d8) +
               *(int *)(&DAT_005b9f40 + param_1 * 0x2d8)) >> 1) - uVar3) {
      local_5c = 0x10;
    }
    else {
      local_5c = 0;
    }
    local_c = local_c | local_50 | local_54 | local_58 | local_5c;
    *(uint *)(&DAT_005b88b4 + param_1 * 0x2c0) = *puVar1;
    *puVar1 = local_c;
    FUN_004228b0(puVar1);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

