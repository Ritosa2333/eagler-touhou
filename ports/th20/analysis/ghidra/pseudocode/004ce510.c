/* Entry: 0x004ce510; symbol: FUN_004ce510; body bytes: 972 */

void __thiscall FUN_004ce510(void *this,int param_1,int param_2)

{
  float *pfVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_b8 [12];
  undefined1 local_ac [12];
  undefined1 local_a0 [12];
  void *local_94;
  int local_90;
  int local_8c;
  void *local_88;
  int local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  float local_70;
  float local_6c;
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
  puStack_c = &LAB_00569b8d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_2c = *(undefined4 *)((int)this + 0x58);
  local_28 = *(undefined4 *)((int)this + 0x5c);
  local_24 = *(undefined4 *)((int)this + 0x60);
  local_84 = 0;
  local_88 = this;
  FUN_00422e10(local_20);
  FUN_00439330(local_88,local_20,*(float *)((int)local_88 + 0x70),DAT_0056cd90 / DAT_0056c8d0);
  local_18 = 0;
  FUN_004296e0(local_20,local_20);
  for (; (local_84 < param_2 && (*(char *)(param_1 + local_84) != '\0')); local_84 = local_84 + 1) {
  }
  if (local_84 != 0) {
    pfVar1 = (float *)FUN_004292f0(local_20,local_a0,(float)local_84);
    FUN_004296e0((void *)((int)local_88 + 0x58),pfVar1);
    *(float *)((int)local_88 + 0x74) =
         *(float *)((int)local_88 + 0x74) - (float)local_84 * DAT_0056cd90;
    if (*(float *)((int)local_88 + 0x74) <= DAT_0056cd90 * DAT_0056cd68) {
      *(uint *)((int)local_88 + 0x1c) = *(uint *)((int)local_88 + 0x1c) & 0xfffffff9 | 2;
      goto LAB_004ce8c1;
    }
    *(undefined4 *)((int)local_88 + 0x708) = *(undefined4 *)((int)local_88 + 0x74);
    *(float *)((int)local_88 + 0x80) = (float)local_84 * DAT_0056cd90;
  }
  local_8c = 0;
  for (; (local_84 < param_2 && (*(char *)(param_1 + local_84) == '\0')); local_84 = local_84 + 1) {
    local_8c = local_8c + 1;
  }
  if (local_84 < param_2) {
    *(float *)((int)local_88 + 0x708) =
         *(float *)((int)local_88 + 0x708) -
         (*(float *)((int)local_88 + 0x74) - (float)local_8c * DAT_0056cd90);
    *(float *)((int)local_88 + 0x74) = (float)local_8c * DAT_0056cd90;
    if (*(float *)((int)local_88 + 0x74) <= DAT_0056cd90 * DAT_0056cd68 &&
        DAT_0056cd90 * DAT_0056cd68 != *(float *)((int)local_88 + 0x74)) {
      *(uint *)((int)local_88 + 0x1c) = *(uint *)((int)local_88 + 0x1c) & 0xfffffff9 | 2;
    }
    while (local_84 < param_2) {
      for (; (local_84 < param_2 && (*(char *)(param_1 + local_84) != '\0'));
          local_84 = local_84 + 1) {
      }
      if (param_2 <= local_84) break;
      local_8c = 0;
      local_90 = local_84;
      for (; (local_84 < param_2 && (*(char *)(param_1 + local_84) == '\0'));
          local_84 = local_84 + 1) {
        local_8c = local_8c + 1;
      }
      FUN_004c8940(&local_80,(undefined4 *)((int)local_88 + 0x6f8));
      local_8 = 0;
      local_70 = (float)local_8c * DAT_0056cd90;
      local_6c = local_70;
      if (DAT_0056cd90 * DAT_0056cd68 < local_70) {
        pfVar1 = (float *)FUN_004292f0(local_20,local_ac,(float)local_90);
        puVar2 = (undefined4 *)FUN_00429570(&local_2c,local_b8,pfVar1);
        local_80 = *puVar2;
        local_7c = puVar2[1];
        local_78 = puVar2[2];
        local_94 = (void *)FUN_0041ca90(*(int *)((int)local_88 + 0x6f0));
        FUN_004d47f0(local_94,0,&local_80);
      }
      local_8 = 0xffffffff;
      FUN_0047c4b0((int)&local_80);
    }
  }
LAB_004ce8c1:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

