/* Entry: 0x004562e0; symbol: FUN_004562e0; body bytes: 932 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_004562e0(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8)

{
  char cVar1;
  bool bVar2;
  float local_40;
  float local_3c;
  int local_38;
  void *local_34;
  int local_30;
  int local_2c;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_6 <= param_7) {
    local_3c = param_7;
  }
  else {
    local_3c = param_6;
  }
  if (param_7 <= param_6) {
    local_40 = param_7;
  }
  else {
    local_40 = param_6;
  }
  if ((local_40 <= param_3) ||
     (cVar1 = FUN_00457040(param_1,param_2,param_4,param_5,param_6 - param_3,param_7 - param_3,
                           param_8), cVar1 == '\0')) {
    FUN_00422e10(&local_14);
    local_14 = param_1 - param_4;
    local_10 = param_2 - param_5;
    FUN_00458fa0(&local_14,&local_14,&local_14,(float)((uint)param_8 ^ _DAT_0056d7d0));
    if (((param_3 <= local_3c) ||
        ((param_3 - local_3c) * (param_3 - local_3c) < local_14 * local_14 + local_10 * local_10))
       && ((DAT_0056c8cc <= param_3 && (DAT_0056c8cc <= local_3c)))) {
      FUN_00422e10(&local_20);
      local_24 = DAT_0056e0f8;
      if (param_3 <= local_3c) {
        local_30 = (int)(param_3 / DAT_0056c8e0);
        if (local_30 < 8) {
          local_30 = 8;
        }
        for (local_38 = 0; local_38 < local_30; local_38 = local_38 + 1) {
          FUN_00439330(&local_20,&local_20,local_24,param_3);
          local_24 = (DAT_0056e0f0 * DAT_0056c8d0) / (float)local_30 + local_24;
          cVar1 = FUN_00457040(local_20 + local_14,local_1c + local_10,0.0,0.0,param_6,param_7,0.0);
          if (cVar1 != '\0') break;
        }
      }
      else {
        local_2c = (int)((int)(param_6 + param_7) + ((int)(param_6 + param_7) >> 0x1f & 3U)) >> 2;
        if (local_2c < 8) {
          local_2c = 8;
        }
        for (local_34 = (void *)0x0; (int)local_34 < local_2c;
            local_34 = (void *)((int)local_34 + 1)) {
          FUN_00459190(local_34,&local_20,local_24,param_6,param_7);
          local_24 = (DAT_0056e0f0 * DAT_0056c8d0) / (float)local_2c + local_24;
          bVar2 = FUN_00456fe0(local_20,local_1c,local_14,local_10,param_3);
          if (bVar2) break;
        }
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

