/* Entry: 0x004578b0; symbol: FUN_004578b0; body bytes: 849 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_004578b0(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8,float param_9,float param_10)

{
  char cVar1;
  void *this;
  void *extraout_ECX;
  void *this_00;
  float10 fVar2;
  float local_48;
  float local_44;
  int local_40;
  int local_3c;
  void *local_38;
  int local_34;
  int local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_8 <= param_9) {
    local_44 = param_9;
  }
  else {
    local_44 = param_8;
  }
  if (param_3 <= param_4) {
    local_48 = param_4;
  }
  else {
    local_48 = param_3;
  }
  FUN_00422e10(&local_20);
  FUN_00422e10(&local_14);
  local_20 = param_1 - param_6;
  local_1c = param_2 - param_7;
  FUN_00458fa0(&local_20,&local_20,&local_20,(float)((uint)param_10 ^ _DAT_0056d7d0));
  FUN_00422dd0(&local_2c,param_3,param_4,0);
  fVar2 = FUN_00438540(param_5 - param_10);
  FUN_00458fa0(this,&local_2c,&local_2c,(float)fVar2);
  if (local_48 <= local_44) {
    local_30 = (int)(local_48 / DAT_0056d7bc);
    if (local_30 < 3) {
      local_30 = 3;
    }
    local_10 = (float)((uint)local_28 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    for (local_3c = 0; local_3c < local_30; local_3c = local_3c + 1) {
      local_14 = (float)((uint)local_2c ^ _DAT_0056d7d0) / DAT_0056c8d0;
      for (local_40 = 0; local_40 < local_30; local_40 = local_40 + 1) {
        cVar1 = FUN_00457040(local_14 + local_20,local_10 + local_1c,0.0,0.0,param_8,param_9,0.0);
        if (cVar1 != '\0') goto LAB_00457bf3;
        local_14 = local_2c / (float)(local_30 + -1) + local_14;
      }
      local_10 = local_28 / (float)(local_30 + -1) + local_10;
    }
  }
  else {
    local_34 = (int)((int)(param_8 + param_9) + ((int)(param_8 + param_9) >> 0x1f & 3U)) >> 2;
    if (local_34 < 8) {
      local_34 = 8;
    }
    local_38 = (void *)0x0;
    this_00 = extraout_ECX;
    while ((int)local_38 < local_34) {
      FUN_00459190(this_00,&local_14,param_5,param_8,param_9);
      param_5 = (DAT_0056e0f0 * DAT_0056c8d0) / (float)local_34 + param_5;
      cVar1 = FUN_00457300(local_14,local_10,local_20,local_1c,local_2c,local_28);
      if (cVar1 != '\0') break;
      this_00 = (void *)((int)local_38 + 1);
      local_38 = this_00;
    }
  }
LAB_00457bf3:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

