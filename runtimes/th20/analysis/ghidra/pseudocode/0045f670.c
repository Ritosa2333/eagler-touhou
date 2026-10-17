/* Entry: 0x0045f670; symbol: FUN_0045f670; body bytes: 827 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0045f670(void *this,float param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  float *pfVar3;
  void *pvVar4;
  void *this_00;
  void *this_01;
  float10 fVar5;
  undefined1 local_a0 [4];
  undefined1 local_9c [4];
  undefined4 local_98;
  undefined4 local_94;
  void *local_8c;
  void *local_88;
  float *local_84;
  float local_80;
  float local_7c;
  void *local_78;
  undefined4 local_74;
  float local_70;
  float local_6c;
  float local_68;
  undefined4 local_64;
  float *local_60;
  float local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  undefined1 *local_4c;
  undefined1 *local_48;
  int local_44;
  float *local_40;
  void *local_3c;
  undefined1 local_38 [8];
  undefined1 local_30 [8];
  float local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_50 = (void *)((int)this + 0x2dc);
  local_3c = this;
  puVar2 = (undefined4 *)FUN_00414580(local_50,0);
  *puVar2 = 0;
  local_54 = (void *)((int)local_3c + 0x2dc);
  pfVar3 = (float *)FUN_00414580(local_54,0);
  local_5c = *pfVar3;
  local_58 = local_3c;
  local_60 = (float *)FUN_0045d670(local_3c,0);
  FUN_00439330(this_00,local_60,0.0,local_5c);
  local_64 = 1;
  pvVar4 = FUN_0045eff0(local_9c,&local_64);
  make_pair<>(local_30,(int)local_3c + 0x2dc,pvVar4);
  local_48 = local_30;
  FUN_0040c080(&local_18,4);
  FUN_0045fda0(&local_18);
  FUN_0040c080(&local_20,4);
  FUN_0045ff00(local_48,&local_20);
  while( true ) {
    bVar1 = FUN_00449400(&local_18,&local_20);
    if (bVar1) break;
    local_40 = (float *)FUN_0045f420(&local_18);
    fVar5 = FUN_004298e0(0x5ba4c4);
    local_68 = (float)fVar5;
    local_70 = local_68 * DAT_0056c8d0;
    local_6c = *local_40;
    *local_40 = local_6c + local_70;
    if (*local_40 <= param_1 + DAT_0056c8e0) {
      if (*local_40 <= param_1 - DAT_0056c8e0 && param_1 - DAT_0056c8e0 != *local_40) {
        *local_40 = param_1 - DAT_0056c8e0;
      }
    }
    else {
      *local_40 = param_1 + DAT_0056c8e0;
    }
    FUN_0045f480(&local_18);
  }
  local_44 = 1;
  FUN_00429210(&local_28,0.0);
  local_74 = 1;
  pvVar4 = FUN_0045eff0(local_a0,&local_74);
  make_pair<>(local_38,local_3c,pvVar4);
  local_4c = local_38;
  FUN_0040c080(&local_1c,4);
  FUN_0045fe00(&local_1c);
  FUN_0040c080(&local_24,4);
  FUN_0045ff20(local_4c,&local_24);
  while( true ) {
    bVar1 = FUN_00449400(&local_1c,&local_24);
    if (bVar1) break;
    local_84 = (float *)FUN_0045f420(&local_1c);
    local_78 = (void *)((int)local_3c + 0x2dc);
    pfVar3 = (float *)FUN_00414580(local_78,local_44);
    local_7c = *pfVar3;
    fVar5 = FUN_004292e0(&local_28);
    local_80 = (float)fVar5;
    FUN_00439330(this_01,local_84,local_80,local_7c);
    FUN_004530c0(&local_28,(DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056ee48);
    local_44 = local_44 + 1;
    FUN_0045f4a0(&local_1c);
  }
  local_88 = local_3c;
  puVar2 = (undefined4 *)FUN_0045d670(local_3c,1);
  local_98 = *puVar2;
  local_94 = puVar2[1];
  local_8c = local_3c;
  puVar2 = (undefined4 *)FUN_0045d670(local_3c,0x3c);
  *puVar2 = local_98;
  puVar2[1] = local_94;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

