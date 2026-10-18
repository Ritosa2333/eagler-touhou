/* Entry: 0x00451050; symbol: FUN_00451050; body bytes: 604 */

void FUN_00451050(undefined4 *param_1,int param_2,int param_3,uint param_4)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_50 [12];
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  void *local_34;
  int local_30;
  undefined4 *local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  undefined4 *local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056872d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  *(int *)((int)local_24 + 0x68) = *(int *)((int)local_24 + 0x68) + 1;
  local_1c = FUN_0044c9b0(DAT_005c0028);
  local_20 = *(void **)(param_3 + 0x4e8);
  local_1c[5] = *(undefined4 *)(param_3 + 0x14);
  local_1c[0x127] = local_1c[0x127] | 0x200;
  puVar2 = FUN_00422dd0(local_50,0,0,0);
  local_1c[0x16f] = *puVar2;
  local_1c[0x170] = puVar2[1];
  local_1c[0x171] = puVar2[2];
  local_1c[0x13a] = local_20;
  if ((param_4 & 4) == 0) {
    local_1c[0x127] = local_1c[0x127] & 0xfffbffff;
  }
  else {
    local_1c[0x127] = local_1c[0x127] | 0x40000;
  }
  FUN_004382b0(local_24,local_1c,param_2,param_3);
  FUN_00425cc0(param_1);
  local_1c[0x15d] = param_4;
  if (local_20 != (void *)0x0) {
    if (((param_4 & 4) == 0) || ((param_4 & 2) == 0)) {
      if ((param_4 & 4) == 0) {
        if ((param_4 & 2) == 0) {
          puVar2 = FUN_0044b7f0(local_20,&local_44,(int)local_1c);
          *param_1 = *puVar2;
        }
        else {
          puVar2 = FUN_0044b840(local_20,&local_40,(int)local_1c);
          *param_1 = *puVar2;
        }
      }
      else {
        puVar2 = FUN_0044b8e0(local_20,&local_3c,(int)local_1c);
        *param_1 = *puVar2;
        *(byte *)(local_1c + 0x128) = *(byte *)(local_1c + 0x128) & 0xfc;
      }
    }
    else {
      puVar2 = FUN_0044b890(local_20,&local_38,(int)local_1c);
      *param_1 = *puVar2;
      *(byte *)(local_1c + 0x128) = *(byte *)(local_1c + 0x128) & 0xfc;
    }
  }
  local_28 = (void *)(param_3 + 0x514);
  local_34 = (void *)FUN_004494b0(local_28,1);
  local_2c = local_1c + 0x145;
  local_30 = FUN_004494b0(local_2c,0);
  FUN_00411ee0(local_34,local_30);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

