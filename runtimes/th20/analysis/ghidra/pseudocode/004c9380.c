/* Entry: 0x004c9380; symbol: FUN_004c9380; body bytes: 266 */

void __thiscall FUN_004c9380(void *this,undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  int local_24;
  float *local_20;
  int local_1c;
  void *local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = this;
  if ((param_2 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_2c = 0;
    FUN_00422e10(&local_14);
    local_20 = *(float **)((int)local_18 + 0x1328);
    for (local_1c = 0; local_1c < *(int *)((int)local_18 + 0x718); local_1c = local_1c + 1) {
      local_14 = *local_20;
      local_10 = local_20[1];
      local_c = local_20[2];
      if (local_1c % 3 == 0) {
        iVar1 = FUN_0040c300(*(undefined4 **)((int)local_18 + 0x6f0));
        local_28 = FUN_004859b0(iVar1);
        local_24 = *(int *)((int)local_18 + 0x714) * 2 + 0xd4;
        FUN_004790e0(&local_30,(uint *)"bullet",local_24,&local_14,0,-1,(undefined4 *)0x0);
      }
      local_20 = local_20 + 8;
    }
    *(undefined4 *)((int)local_18 + 0x20) = 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

