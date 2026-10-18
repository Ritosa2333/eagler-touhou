/* Entry: 0x004ce220; symbol: FUN_004ce220; body bytes: 742 */

void FUN_004ce220(int param_1,int param_2)

{
  float *pfVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_a4 [12];
  undefined1 local_98 [12];
  void *local_8c;
  void *local_88;
  int local_84;
  int local_80;
  int local_7c;
  int local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  float local_64;
  float local_60;
  float local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  uint local_40;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  float local_20 [2];
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569b4d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_78 = 0;
  FUN_00422e10(local_20);
  FUN_00439330(local_20,local_20,*(float *)(local_7c + 0x70),DAT_0056cd90 / DAT_0056c8d0);
  local_18 = 0;
  FUN_004296e0(local_20,local_20);
  for (; (local_78 < param_2 && (*(char *)(param_1 + local_78) != '\0')); local_78 = local_78 + 1) {
  }
  if (local_78 == 0) {
    local_80 = 0;
    for (; (local_78 < param_2 && (*(char *)(param_1 + local_78) == '\0')); local_78 = local_78 + 1)
    {
      local_80 = local_80 + 1;
    }
    if (param_2 <= local_78) goto LAB_004ce4e9;
    *(float *)(local_7c + 0x74) = (float)local_80 * DAT_0056cd90;
  }
  else {
    *(undefined4 *)(local_7c + 0x74) = 0;
    local_80 = 0;
  }
  while (local_78 < param_2) {
    for (; (local_78 < param_2 && (*(char *)(param_1 + local_78) != '\0')); local_78 = local_78 + 1)
    {
    }
    if (param_2 <= local_78) break;
    local_80 = 0;
    local_84 = local_78;
    for (; (local_78 < param_2 && (*(char *)(param_1 + local_78) == '\0')); local_78 = local_78 + 1)
    {
      local_80 = local_80 + 1;
    }
    FUN_0047c130(&local_74);
    local_8 = 0;
    local_64 = (float)local_80 * DAT_0056cd90;
    local_88 = (void *)(local_7c + 0x58);
    local_60 = local_64;
    pfVar1 = (float *)FUN_004292f0(local_20,local_98,(float)local_84);
    puVar2 = (undefined4 *)FUN_00429570(local_88,local_a4,pfVar1);
    local_74 = *puVar2;
    local_70 = puVar2[1];
    local_6c = puVar2[2];
    local_54 = DAT_0056d7bc;
    local_68 = *(undefined4 *)(local_7c + 0x70);
    local_58 = *(undefined4 *)(local_7c + 0x78);
    local_50 = *(undefined4 *)(local_7c + 0x74c);
    local_4c = *(undefined4 *)(local_7c + 0x750);
    local_5c = *(float *)(local_7c + 0x718) - (float)local_84 * DAT_0056cd90;
    local_40 = local_40 & 0xfffffffe | *(uint *)(local_7c + 0x754) >> 1 & 1;
    local_48 = 0;
    local_2c = 0xffffffff;
    local_28 = 0xffffffff;
    local_24 = *(undefined4 *)(local_7c + 0x6ec);
    local_8c = (void *)FUN_0041ca90(*(int *)(local_7c + 0x6f0));
    FUN_004d47f0(local_8c,0,&local_74);
    local_8 = 0xffffffff;
    FUN_0047c4b0((int)&local_74);
  }
LAB_004ce4e9:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

