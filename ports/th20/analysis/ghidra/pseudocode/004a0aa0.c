/* Entry: 0x004a0aa0; symbol: FUN_004a0aa0; body bytes: 174 */

void FUN_004a0aa0(float param_1,float param_2)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined4 local_24;
  undefined4 local_20;
  void *local_1c;
  int local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422dd0(local_14,param_1 * DAT_0056c8d0,param_2 * DAT_0056c8d0,0);
  local_1c = (void *)(local_18 + 0x1a24c);
  bVar1 = FUN_00449430(local_1c,0);
  if (bVar1) {
    local_20 = FUN_00437950(local_18);
    puVar2 = FUN_004790e0(&local_24,(uint *)0x0,0x11,local_14,0,-1,(undefined4 *)0x0);
    *(undefined4 *)(local_18 + 0x1a24c) = *puVar2;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

