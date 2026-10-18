/* Entry: 0x0049d850; symbol: FUN_0049d850; body bytes: 503 */

void __thiscall FUN_0049d850(void *this,float param_1,float param_2,float param_3,float param_4)

{
  int iVar1;
  int iVar2;
  float fVar3;
  float fVar4;
  int local_28;
  int local_24;
  float *local_20;
  float *local_1c;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
    iVar1 = FUN_0049df20(&DAT_005b6758,*(int *)((int)this + 0x1c));
    iVar2 = FUN_0049df00(&DAT_005b6758,*(int *)((int)this + 0x1c));
    FUN_00422dd0(&local_14,(float)iVar2 + param_1,(float)iVar1 + param_2,0);
                    /* WARNING: Load size is inaccurate */
    fVar3 = (float)*this - DAT_0056c8cc;
    fVar4 = (float)*(int *)((int)this + 4) - DAT_0056c8cc;
    local_1c = *(float **)((int)this + 0x14);
    local_20 = *(float **)((int)this + 0x18);
                    /* WARNING: Load size is inaccurate */
    for (local_24 = 0; local_24 < *this; local_24 = local_24 + 1) {
      for (local_28 = 0; local_28 < *(int *)((int)this + 4); local_28 = local_28 + 1) {
        *local_20 = local_14;
        local_20[1] = local_10;
        local_20[2] = local_c;
        *local_1c = *local_20;
        local_1c[1] = local_20[1];
        local_1c[2] = local_20[2];
        FUN_0049df40(*local_20,local_20[1],local_20[2],local_1c + 5,local_1c + 6);
        local_1c[3] = DAT_0056c8cc;
        local_1c[4] = -NAN;
        local_10 = local_10 + param_4 / fVar4;
        local_20 = local_20 + 3;
        local_1c = local_1c + 7;
      }
      iVar1 = FUN_0049df20(&DAT_005b6758,*(int *)((int)this + 0x1c));
      local_10 = (float)iVar1 + param_2;
      local_14 = local_14 + param_3 / fVar3;
    }
    FUN_0049ddc0((int *)this);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

