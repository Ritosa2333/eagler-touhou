/* Entry: 0x004c9ab0; symbol: FUN_004c9ab0; body bytes: 735 */

void __thiscall
FUN_004c9ab0(void *this,float *param_1,float *param_2,float param_3,undefined4 param_4,int param_5)

{
  char cVar1;
  float *pfVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined1 local_84 [12];
  undefined1 local_78 [12];
  undefined1 local_6c [12];
  undefined4 local_60;
  code *local_5c;
  undefined4 local_58;
  int local_54;
  int *local_50;
  float *local_4c;
  int local_48;
  void *local_44;
  int local_40;
  int *local_3c;
  int local_38;
  int local_34;
  int local_30;
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
  local_3c = (int *)this;
  if ((param_5 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_48 = 0;
    local_40 = 0;
    FUN_00422e10(&local_2c);
    FUN_00422e10(&local_14);
    FUN_00422e10(&local_20);
    local_38 = local_3c[0x16];
    local_34 = local_3c[0x17];
    local_30 = local_3c[0x18];
    local_44 = (void *)FUN_0041f610(local_3c[0x1c6]);
    if (local_44 != (void *)0x0) {
      _memset(local_44,0,local_3c[0x1c6]);
      pfVar2 = (float *)FUN_00452ff0(param_2,local_6c,DAT_0056c8d0);
      local_14 = *pfVar2;
      local_10 = pfVar2[1];
      local_c = pfVar2[2];
      puVar3 = (undefined4 *)FUN_00429440(param_1,local_78,&local_14);
      local_2c = *puVar3;
      local_28 = puVar3[1];
      local_24 = puVar3[2];
      pfVar2 = (float *)FUN_00429570(param_1,local_84,&local_14);
      local_14 = *pfVar2;
      local_10 = pfVar2[1];
      local_c = pfVar2[2];
      local_4c = (float *)local_3c[0x4ca];
      for (local_40 = 0; local_40 < local_3c[0x1c6]; local_40 = local_40 + 1) {
        local_20 = *local_4c;
        local_1c = local_4c[1];
        local_18 = local_4c[2];
        cVar1 = FUN_00458e90(*param_1,param_1[1],*param_2,param_2[1],param_3,local_20,local_1c);
        if (cVar1 != '\0') {
          *(undefined1 *)((int)local_44 + local_40) = 1;
          local_48 = local_48 + 1;
          iVar4 = FUN_0040c300((undefined4 *)local_3c[0x1bc]);
          local_50 = (int *)(iVar4 + 0x286d88);
          *local_50 = *local_50 + 1;
          if (local_40 % 0x14 == 0) {
            iVar4 = FUN_0040c300((undefined4 *)local_3c[0x1bc]);
            local_58 = FUN_004859b0(iVar4);
            local_54 = local_3c[0x1c5] * 2 + 0xd4;
            FUN_004790e0(&local_60,(uint *)"bullet",local_54,&local_20,0,-1,(undefined4 *)0x0);
          }
        }
        local_4c = local_4c + 8;
      }
      if (local_48 < local_3c[0x1c6]) {
        if (0 < local_48) {
          local_5c = *(code **)(*local_3c + 0x38);
          (*local_5c)(local_44,local_40);
        }
      }
      else {
        local_3c[7] = local_3c[7] & 0xfffffff9U | 2;
      }
      if (local_44 != (LPVOID)0x0) {
        FUN_0041f670(local_44);
        local_44 = (void *)0x0;
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

