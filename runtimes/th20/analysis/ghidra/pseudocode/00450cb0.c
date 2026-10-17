/* Entry: 0x00450cb0; symbol: FUN_00450cb0; body bytes: 922 */

/* WARNING: Removing unreachable block (ram,0x00450ee4) */

void FUN_00450cb0(undefined4 *param_1,uint *param_2,int param_3,float *param_4,undefined4 param_5,
                 int param_6,uint param_7,undefined4 *param_8)

{
  char cVar1;
  char **ppcVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  int *unaff_FS_OFFSET;
  undefined1 local_50 [12];
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  void *local_30;
  void *local_2c;
  undefined4 *local_28;
  float local_24 [3];
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056872d;
  local_10 = *unaff_FS_OFFSET;
  ppcVar2 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar2;
  uVar3 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_18,uVar3);
  local_8 = 0;
  *(int *)((int)local_30 + 0x68) = *(int *)((int)local_30 + 0x68) + 1;
  local_28 = FUN_0044c9b0((int)DAT_005c0028);
  if (param_8 != (undefined4 *)0x0) {
    *param_8 = local_28;
  }
  if ((param_2 != (uint *)0x0) &&
     (cVar1 = FUN_00446de0((void *)((int)local_30 + 0x20),param_2), cVar1 == '\0')) {
    std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056e584,ppcVar2);
  }
  local_2c = (void *)FUN_0044c8c0(DAT_005c0028,0);
  local_34 = 0;
  if (((param_7 & 0x10) != 0) || (iVar4 = FUN_0044c8a0((int)DAT_005c0028), iVar4 != 0)) {
    local_34 = 1;
    local_2c = (void *)FUN_0044c8c0(DAT_005c0028,1);
  }
  FUN_00438b70(local_30,local_28,param_3);
  local_28[0x13a] = local_2c;
  local_28[0x127] = local_28[0x127] | 0x200;
  if ((-1 < param_6) && (local_28[5] = param_6, param_6 < 0x18)) {
    *(byte *)((int)local_28 + 0x4a3) = *(byte *)((int)local_28 + 0x4a3) & 0xfc | 1;
  }
  if ((param_4 == (float *)0x0) && ((param_7 & 8) == 0)) {
    puVar5 = FUN_00422dd0(local_50,0,0,0);
    local_28[0x16f] = *puVar5;
    local_28[0x170] = puVar5[1];
    local_28[0x171] = puVar5[2];
  }
  else if ((param_7 & 1) == 0) {
    if (param_4 != (float *)0x0) {
      local_28[0x16f] = *param_4;
      local_28[0x170] = param_4[1];
      local_28[0x171] = param_4[2];
    }
  }
  else if (param_4 != (float *)0x0) {
    local_28[0x16f] =
         (DAT_0056cdac - DAT_0056cda4) / DAT_0056c8d0 + DAT_0056cda4 / DAT_0056c8d0 + *param_4;
    local_28[0x170] = param_4[1] + DAT_0056cd90;
    local_28[0x171] = param_4[2];
  }
  local_28[0x10] = param_5;
  if ((param_7 & 4) == 0) {
    local_28[0x127] = local_28[0x127] & 0xfffbffff;
  }
  else {
    local_28[0x127] = local_28[0x127] | 0x40000;
  }
  FUN_0042b5d0(local_28);
  FUN_00422e10(local_24);
  FUN_004376e0(local_28,local_24);
  FUN_00425cc0(param_1);
  local_28[0x15d] = param_7;
  if (((param_7 & 4) == 0) || ((param_7 & 2) == 0)) {
    if ((param_7 & 4) == 0) {
      if ((param_7 & 2) == 0) {
        puVar5 = FUN_0044b7f0(local_2c,&local_44,(int)local_28);
        *param_1 = *puVar5;
      }
      else {
        puVar5 = FUN_0044b840(local_2c,&local_40,(int)local_28);
        *param_1 = *puVar5;
      }
    }
    else {
      puVar5 = FUN_0044b8e0(local_2c,&local_3c,(int)local_28);
      *param_1 = *puVar5;
      *(byte *)(local_28 + 0x128) = *(byte *)(local_28 + 0x128) & 0xfc;
    }
  }
  else {
    puVar5 = FUN_0044b890(local_2c,&local_38,(int)local_28);
    *param_1 = *puVar5;
    *(byte *)(local_28 + 0x128) = *(byte *)(local_28 + 0x128) & 0xfc;
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

