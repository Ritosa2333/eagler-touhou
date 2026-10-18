/* Entry: 0x004cdef0; symbol: FUN_004cdef0; body bytes: 802 */

void __thiscall FUN_004cdef0(void *this,int param_1,int param_2)

{
  int iVar1;
  void *this_00;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  float10 fVar5;
  int local_84;
  int local_80;
  int local_7c;
  int local_74;
  undefined1 local_70 [32];
  int local_50;
  undefined4 local_34;
  int local_28;
  float local_24;
  undefined4 local_20;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569b0d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  for (local_74 = 0;
      (local_74 < *(int *)((int)this + 0x718) && (*(char *)(param_1 + local_74) != '\0'));
      local_74 = local_74 + 1) {
  }
  if (local_74 != 0) {
    local_7c = 0;
    for (local_80 = local_74; local_80 < *(int *)((int)this + 0x718); local_80 = local_80 + 1) {
      *(undefined1 *)(param_1 + local_7c) = *(undefined1 *)(param_1 + local_80);
      local_7c = local_7c + 1;
    }
    if (*(int *)((int)this + 0x1324) != 0) {
      local_7c = 0;
      for (local_84 = local_74; local_84 < *(int *)((int)this + 0x718); local_84 = local_84 + 1) {
        puVar3 = (undefined4 *)(*(int *)((int)this + 0x1328) + local_84 * 0x20);
        puVar4 = (undefined4 *)(*(int *)((int)this + 0x1328) + local_7c * 0x20);
        for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar4 = *puVar3;
          puVar3 = puVar3 + 1;
          puVar4 = puVar4 + 1;
        }
        local_7c = local_7c + 1;
      }
    }
    FUN_004c9170((void *)((int)this + 0x48),local_74);
    *(int *)((int)this + 0x718) = *(int *)((int)this + 0x718) - local_74;
    if (*(int *)((int)this + 0x718) < 4) {
      *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) & 0xfffffff9 | 2;
      goto LAB_004ce1f5;
    }
    local_74 = 0;
  }
  for (; (iVar2 = local_74, local_74 < *(int *)((int)this + 0x718) &&
         (*(char *)(param_1 + local_74) == '\0')); local_74 = local_74 + 1) {
  }
  while (local_74 < *(int *)((int)this + 0x718)) {
    for (; (iVar1 = local_74, local_74 < *(int *)((int)this + 0x718) &&
           (*(char *)(param_1 + local_74) != '\0')); local_74 = local_74 + 1) {
    }
    if (param_2 <= local_74) break;
    local_7c = 0;
    for (; (local_74 < *(int *)((int)this + 0x718) && (*(char *)(param_1 + local_74) == '\0'));
        local_74 = local_74 + 1) {
      local_7c = local_7c + 1;
    }
    if ((*(int *)((int)this + 0x1324) == 0) && (3 < local_7c)) {
      FUN_004c84b0(local_70,(undefined4 *)((int)this + 0x6f8));
      local_8 = 0;
      local_28 = (int)this + 0x1330;
      local_50 = local_7c;
      fVar5 = FUN_00423bd0((int)this + 0x48);
      local_24 = (float)fVar5 - (float)iVar1;
      local_34 = 0xffffffff;
      local_20 = 999;
      this_00 = (void *)FUN_0041ca90(*(int *)((int)this + 0x6f0));
      FUN_004d47f0(this_00,2,local_70);
      local_8 = 0xffffffff;
      FUN_0048b8b0((int)local_70);
    }
  }
  if (iVar2 < 4) {
    *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) & 0xfffffff9 | 2;
  }
  else {
    *(int *)((int)this + 0x718) = iVar2;
    *(undefined4 *)((int)this + 0x1320) = 9999;
  }
LAB_004ce1f5:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

