/* Entry: 0x0043d370; symbol: FUN_0043d370; body bytes: 1204 */

void __thiscall
FUN_0043d370(void *this,int param_1,float *param_2,float *param_3,float *param_4,float param_5,
            char param_6)

{
  undefined4 *puVar1;
  float *pfVar2;
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *extraout_ECX_01;
  void *this_00;
  float fVar3;
  float local_ac [2];
  float local_a4 [2];
  undefined1 local_9c [8];
  undefined1 local_94 [8];
  float local_8c [2];
  undefined1 local_84 [8];
  undefined1 local_7c [8];
  undefined1 local_74 [8];
  float local_6c [2];
  undefined1 local_64 [8];
  undefined1 local_5c [8];
  code *local_54;
  undefined4 local_50;
  code *local_4c;
  float *local_48;
  float *local_44;
  float *local_40;
  float *local_3c;
  float *local_38;
  float *local_34;
  float *local_30;
  int *local_2c;
  int *local_28;
  int local_24;
  void *local_20;
  float *local_1c;
  float local_18;
  float local_14;
  void *local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_20 = this;
  if (((uint)((param_1 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_1)) {
    local_1c = *(float **)((int)this + 0x7d40e80);
    FUN_004455c0((int)this);
    FUN_004398a0(&local_10);
    FUN_004398a0(&local_18);
    fVar3 = param_5 / DAT_0056c8d0;
    for (local_24 = 0; local_24 < param_1; local_24 = local_24 + 1) {
      if (local_24 == 0) {
        local_30 = param_3 + 2;
        puVar1 = (undefined4 *)FUN_00429520(local_30,local_5c,param_3);
        this_00 = (void *)*puVar1;
        local_c = (float)puVar1[1];
        local_10 = this_00;
        if (param_6 != '\0') {
          local_34 = param_3;
          pfVar2 = (float *)FUN_00429520(param_3,local_64,param_3 + param_1 * 2 + -2);
          local_18 = *pfVar2;
          local_14 = pfVar2[1];
          FUN_00439960(&local_10,local_6c,&local_18);
          this_00 = extraout_ECX;
        }
      }
      else if (local_24 == param_1 + -1) {
        local_38 = param_3 + param_1 * 2 + -2;
        puVar1 = (undefined4 *)FUN_00429520(local_38,local_74,param_3 + param_1 * 2 + -4);
        this_00 = (void *)*puVar1;
        local_c = (float)puVar1[1];
        local_10 = this_00;
        if (param_6 != '\0') {
          local_3c = param_3 + 2;
          puVar1 = (undefined4 *)FUN_00429520(local_3c,local_7c,param_3);
          local_10 = (void *)*puVar1;
          local_c = (float)puVar1[1];
          local_40 = param_3;
          pfVar2 = (float *)FUN_00429520(param_3,local_84,param_3 + param_1 * 2 + -2);
          local_18 = *pfVar2;
          local_14 = pfVar2[1];
          FUN_00439960(&local_10,local_8c,&local_18);
          this_00 = extraout_ECX_00;
        }
      }
      else {
        local_44 = param_3 + local_24 * 2;
        puVar1 = (undefined4 *)FUN_00429520(local_44,local_94,param_3 + local_24 * 2 + -2);
        local_10 = (void *)*puVar1;
        local_c = (float)puVar1[1];
        local_48 = param_3 + local_24 * 2 + 2;
        pfVar2 = (float *)FUN_00429520(local_48,local_9c,param_3 + local_24 * 2);
        local_18 = *pfVar2;
        local_14 = pfVar2[1];
        FUN_00439960(&local_10,local_a4,&local_18);
        this_00 = extraout_ECX_01;
      }
      FUN_00458fa0(this_00,&local_18,(float *)&local_10,DAT_0056e0f8 / DAT_0056c8d0);
      FUN_0043e910((float *)&local_10,&local_18);
      FUN_00439910(&local_10,local_ac,fVar3);
      *local_1c = param_3[local_24 * 2] + *param_2 + (float)local_10;
      local_1c[1] = param_3[local_24 * 2 + 1] + param_2[1] + local_c;
      local_1c[2] = 0.0;
      local_1c[3] = DAT_0056c8cc;
      local_1c[4] = *param_4;
      *local_1c = *local_1c + *(float *)((int)local_20 + 0xd0);
      local_1c[1] = local_1c[1] + *(float *)((int)local_20 + 0xd4);
      pfVar2 = local_1c + 5;
      *pfVar2 = (param_3[local_24 * 2] + *param_2) - (float)local_10;
      local_1c[6] = (param_3[local_24 * 2 + 1] + param_2[1]) - local_c;
      local_1c[7] = 0.0;
      local_1c[8] = DAT_0056c8cc;
      local_1c[9] = *param_4;
      *pfVar2 = *pfVar2 + *(float *)((int)local_20 + 0xd0);
      local_1c[6] = local_1c[6] + *(float *)((int)local_20 + 0xd4);
      local_1c = local_1c + 10;
      param_4 = param_4 + 1;
    }
    *(undefined1 *)((int)local_20 + 0x6000e0e) = 1;
    FUN_00446690(local_20,'\x02');
    local_28 = (int *)FUN_00412730(0x5c4d40);
    local_4c = *(code **)(*local_28 + 0x164);
    (*local_4c)(local_28,0x44);
    local_2c = (int *)FUN_00412730(0x5c4d40);
    local_54 = *(code **)(*local_2c + 0x14c);
    local_50 = *(undefined4 *)((int)local_20 + 0x7d40e80);
    (*local_54)(local_2c,5,param_1 * 2 + -2,local_50,0x14);
    *(int *)((int)local_20 + 0x7d40e80) = param_1 * 0x14 + *(int *)((int)local_20 + 0x7d40e80);
    *(int *)((int)local_20 + 0xc4) = *(int *)((int)local_20 + 0xc4) + 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

