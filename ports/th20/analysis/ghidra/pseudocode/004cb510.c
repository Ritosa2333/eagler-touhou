/* Entry: 0x004cb510; symbol: FUN_004cb510; body bytes: 1111 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_004cb510(void *this,float *param_1,float *param_2,float param_3,undefined4 param_4,int param_5,
            int *param_6)

{
  char cVar1;
  float *pfVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined1 local_9c [12];
  undefined1 local_90 [12];
  undefined1 local_84 [12];
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  int *local_50;
  int local_4c;
  float *local_48;
  int local_44;
  int local_40;
  void *local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_3c = this;
  if ((param_5 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_4c = 0;
    local_44 = 0;
    FUN_00422e10(&local_2c);
    FUN_00422e10(&local_14);
    FUN_00422e10(&local_20);
    local_38 = *(undefined4 *)((int)local_3c + 0x58);
    local_34 = *(undefined4 *)((int)local_3c + 0x5c);
    local_30 = *(undefined4 *)((int)local_3c + 0x60);
    pfVar2 = (float *)FUN_00452ff0(param_2,local_84,DAT_0056c8d0);
    local_14 = *pfVar2;
    local_10 = pfVar2[1];
    local_c = pfVar2[2];
    puVar3 = (undefined4 *)FUN_00429440(param_1,local_90,&local_14);
    local_2c = *puVar3;
    local_28 = puVar3[1];
    local_24 = puVar3[2];
    pfVar2 = (float *)FUN_00429570(param_1,local_9c,&local_14);
    local_14 = *pfVar2;
    local_10 = pfVar2[1];
    local_c = pfVar2[2];
    local_48 = *(float **)((int)local_3c + 0x1328);
    for (local_44 = 0; local_44 < *(int *)((int)local_3c + 0x718); local_44 = local_44 + 1) {
      local_20 = *local_48;
      local_1c = local_48[1];
      local_18 = local_48[2];
      if (*param_6 == 0) {
        local_54 = (void *)FUN_00412730(*(int *)((int)local_3c + 0x6f0));
        local_58 = (void *)FUN_00485660(local_54,0);
        iVar4 = FUN_004d6b80(local_58,0);
        iVar4 = FUN_00437cd0(iVar4);
        local_6c = *(float *)(iVar4 + 0x48) * _DAT_00571788;
        local_5c = (void *)FUN_00412730(*(int *)((int)local_3c + 0x6f0));
        local_60 = (void *)FUN_00485660(local_5c,0);
        iVar4 = FUN_004d6b80(local_60,0);
        iVar4 = FUN_00437cd0(iVar4);
        local_70 = *(float *)(iVar4 + 0x4c) * _DAT_00571788;
        local_64 = (void *)FUN_00412730(*(int *)((int)local_3c + 0x6f0));
        iVar4 = FUN_00485660(local_64,0);
        iVar4 = FUN_0047a2c0(iVar4);
        local_74 = *(float *)(iVar4 + 4);
        local_68 = (void *)FUN_00412730(*(int *)((int)local_3c + 0x6f0));
        iVar4 = FUN_00485660(local_68,0);
        pfVar2 = (float *)FUN_0047a2c0(iVar4);
        local_78 = *pfVar2;
        cVar1 = FUN_00457610(local_78,local_74,local_70,local_6c,0.0,local_20,local_1c,
                             DAT_0056cd90 / DAT_0056c8d0);
        if (cVar1 != '\0') {
          *param_6 = 1;
        }
      }
      cVar1 = FUN_00458e90(*param_1,param_1[1],*param_2,param_2[1],param_3,local_20,local_1c);
      if (cVar1 != '\0') {
        local_4c = local_4c + 1;
        local_40 = 0xf;
        if (*(float *)((int)local_3c + 0x7c) < DAT_0056f7a8) {
          if ((DAT_0056c8e0 <= *(float *)((int)local_3c + 0x7c)) &&
             (*(float *)((int)local_3c + 0x7c) <= DAT_0056f7a8 &&
              DAT_0056f7a8 != *(float *)((int)local_3c + 0x7c))) {
            local_40 = (int)((((*(float *)((int)local_3c + 0x7c) - DAT_0056c8e0) / DAT_0056d7bc) *
                              DAT_0056c8d0 + DAT_0056c8cc) * 15.0);
          }
        }
        else {
          local_40 = 0x2d;
        }
        if (*(float *)((int)local_3c + 0x78) < DAT_0056fe7c) {
          if ((DAT_0056cd90 <= *(float *)((int)local_3c + 0x78)) &&
             (*(float *)((int)local_3c + 0x78) <= DAT_0056fe7c &&
              DAT_0056fe7c != *(float *)((int)local_3c + 0x78))) {
            local_40 = (int)((float)local_40 +
                             ((*(float *)((int)local_3c + 0x78) - DAT_0056cd90) / DAT_0056fa30) *
                             DAT_0056c8d8 + DAT_0056c8cc);
          }
        }
        else {
          local_40 = (int)((float)local_40 +
                           ((DAT_0056fe7c - DAT_0056cd90) / DAT_0056fa30) * DAT_0056c8d8 +
                          DAT_0056c8cc);
        }
        iVar4 = FUN_0041ca90(*(int *)((int)local_3c + 0x6f0));
        local_50 = (int *)(iVar4 + 0x4c);
        *local_50 = *local_50 + local_40;
      }
      local_48 = local_48 + 8;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

