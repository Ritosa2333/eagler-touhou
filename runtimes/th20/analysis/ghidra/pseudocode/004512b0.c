/* Entry: 0x004512b0; symbol: FUN_004512b0; body bytes: 619 */

void FUN_004512b0(undefined4 *param_1,int param_2,void *param_3,uint param_4)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  void *local_24;
  void *local_20;
  undefined4 *local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056824d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  *(int *)((int)local_24 + 0x68) = *(int *)((int)local_24 + 0x68) + 1;
  local_1c = FUN_0044c9b0(DAT_005c0028);
  local_20 = *(void **)((int)param_3 + 0x4e8);
  FUN_00438b70(local_24,local_1c,param_2);
  local_1c[0x13a] = local_20;
  local_1c[5] = *(undefined4 *)((int)param_3 + 0x14);
  local_1c[0x127] = local_1c[0x127] | 0x200;
  local_1c[0x127] =
       local_1c[0x127] & 0xfeffffff | (*(uint *)((int)param_3 + 0x49c) >> 0x18 & 1) << 0x18;
  if ((param_4 & 4) == 0) {
    local_1c[0x127] = local_1c[0x127] & 0xfffbffff;
  }
  else {
    local_1c[0x127] = local_1c[0x127] | 0x40000;
  }
  FUN_0044cb80(param_3,local_1c + 0x16f);
  local_1c[0xe] = *(undefined4 *)((int)param_3 + 0x38);
  local_1c[0xf] = *(undefined4 *)((int)param_3 + 0x3c);
  local_1c[0x10] = *(undefined4 *)((int)param_3 + 0x40);
  local_1c[0x121] = *(undefined4 *)((int)param_3 + 0x2c);
  local_1c[0x122] = *(undefined4 *)((int)param_3 + 0x30);
  local_1c[0x123] = *(undefined4 *)((int)param_3 + 0x34);
  FUN_0042b5d0(local_1c);
  FUN_00425cc0(param_1);
  local_1c[0x15d] = param_4;
  if (local_20 != (void *)0x0) {
    if (((param_4 & 4) == 0) || ((param_4 & 2) == 0)) {
      if ((param_4 & 4) == 0) {
        if ((param_4 & 2) == 0) {
          puVar2 = FUN_0044b7f0(local_20,&local_34,(int)local_1c);
          *param_1 = *puVar2;
        }
        else {
          puVar2 = FUN_0044b840(local_20,&local_30,(int)local_1c);
          *param_1 = *puVar2;
        }
      }
      else {
        puVar2 = FUN_0044b8e0(local_20,&local_2c,(int)local_1c);
        *param_1 = *puVar2;
        *(byte *)(local_1c + 0x128) = *(byte *)(local_1c + 0x128) & 0xfc;
      }
    }
    else {
      puVar2 = FUN_0044b890(local_20,&local_28,(int)local_1c);
      *param_1 = *puVar2;
      *(byte *)(local_1c + 0x128) = *(byte *)(local_1c + 0x128) & 0xfc;
    }
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

