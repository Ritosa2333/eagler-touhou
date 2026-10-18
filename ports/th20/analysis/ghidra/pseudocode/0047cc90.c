/* Entry: 0x0047cc90; symbol: FUN_0047cc90; body bytes: 707 */

void FUN_0047cc90(float *param_1,float *param_2,float param_3,int param_4,uint param_5)

{
  bool bVar1;
  char cVar2;
  float *pfVar3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  undefined1 local_70 [12];
  undefined1 local_64 [12];
  undefined1 local_58 [12];
  undefined4 *local_4c;
  int *local_48;
  void *local_44;
  int local_40;
  int local_3c;
  void *local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  float local_28;
  float local_24;
  float local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056908d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00422e10(&local_34);
  FUN_00422e10(&local_28);
  local_40 = 0;
  pfVar3 = (float *)FUN_00452ff0(param_2,local_58,DAT_0056c8d0);
  local_28 = *pfVar3;
  local_24 = pfVar3[1];
  local_20 = pfVar3[2];
  puVar4 = (undefined4 *)FUN_00429440(param_1,local_64,&local_28);
  local_34 = *puVar4;
  local_30 = puVar4[1];
  local_2c = puVar4[2];
  pfVar3 = (float *)FUN_00429570(param_1,local_70,&local_28);
  local_28 = *pfVar3;
  local_24 = pfVar3[1];
  local_20 = pfVar3[2];
  local_44 = (void *)FUN_00485970(local_3c);
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_44,local_1c);
  local_8 = 0;
  local_48 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,local_48), bVar1) {
    local_4c = (undefined4 *)FUN_0040c300(local_1c);
    local_38 = (void *)FUN_0040c300(local_4c);
    if ((((*(int *)((int)local_38 + 0x50) == 2) || (*(int *)((int)local_38 + 0x50) == 1)) &&
        (*(int *)((int)local_38 + 0x18) == 0)) &&
       ((cVar2 = FUN_00457610(*param_1,param_1[1],*param_2,param_2[1],param_3,
                              *(float *)((int)local_38 + 100),*(float *)((int)local_38 + 0x68),
                              *(float *)((int)local_38 + 0x7c) * *(float *)((int)local_38 + 0x48)),
        cVar2 != '\0' &&
        (cVar2 = FUN_00457610(0.0,DAT_0056d7c0 / DAT_0056c8d0,DAT_0056cda4,DAT_0056d7c0,0.0,
                              *(float *)((int)local_38 + 100),*(float *)((int)local_38 + 0x68),
                              *(float *)((int)local_38 + 0x7c) * *(float *)((int)local_38 + 0x48)),
        cVar2 != '\0')))) {
      local_40 = local_40 + 1;
      *(int *)(local_3c + 0x286d88) = *(int *)(local_3c + 0x286d88) + 1;
      *(uint *)((int)local_38 + 0x14) =
           *(uint *)((int)local_38 + 0x14) & 0xffffe7ff | (param_5 & 3) << 0xb;
      FUN_0047c8f0(local_38,param_4);
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

