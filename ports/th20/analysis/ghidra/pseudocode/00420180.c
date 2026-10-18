/* Entry: 0x00420180; symbol: FUN_00420180; body bytes: 153 */

void FUN_00420180(int param_1)

{
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_20 = 0;
  local_1c = 0;
  local_18 = 0;
  local_14 = 0;
  local_10 = 0;
  local_c = 0;
  if ((*(uint *)(param_1 + 0x18) & 3) != 0) {
    local_20 = 0x18;
    local_1c = 0x10;
    local_14 = 2;
    local_18 = *(undefined4 *)(param_1 + 0x18);
    local_10 = 0xfffffc18;
    local_c = 1000;
    (**(code **)(*DAT_005b88ac + 0x18))(DAT_005b88ac,4,&local_20);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

