/* Entry: 0x0049d5b0; symbol: FUN_0049d5b0; body bytes: 480 */

void __thiscall
FUN_0049d5b0(void *this,undefined4 param_1,float param_2,float param_3,float param_4)

{
  float fVar1;
  float fVar2;
  int local_28;
  int local_24;
  float *local_20;
  float *local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
    FUN_00422dd0(&local_14,param_1,param_2,0);
                    /* WARNING: Load size is inaccurate */
    fVar1 = (float)*this - DAT_0056c8cc;
    fVar2 = (float)*(int *)((int)this + 4) - DAT_0056c8cc;
    local_18 = *(float **)((int)this + 0x14);
    local_20 = *(float **)((int)this + 0x18);
                    /* WARNING: Load size is inaccurate */
    for (local_24 = 0; local_24 < *this; local_24 = local_24 + 1) {
      local_10 = param_2;
      for (local_28 = 0; local_28 < *(int *)((int)this + 4); local_28 = local_28 + 1) {
        *local_20 = local_14;
        local_20[1] = local_10;
        local_20[2] = local_c;
        *local_18 = *local_20;
        local_18[1] = local_20[1];
        local_18[2] = local_20[2];
        FUN_0049df40(*local_20,local_20[1],local_20[2],local_18 + 5,local_18 + 6);
        *local_18 = (float)DAT_005c538c + *local_18;
        local_18[1] = (float)DAT_005c5390 + local_18[1];
        local_18[3] = DAT_0056c8cc;
        local_18[4] = -NAN;
        local_10 = local_10 + param_4 / fVar2;
        local_20 = local_20 + 3;
        local_18 = local_18 + 7;
      }
      local_14 = local_14 + param_3 / fVar1;
    }
    FUN_0049ddc0((int *)this);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

