/* Entry: 0x004b89a0; symbol: FUN_004b89a0; body bytes: 387 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004b89a0(void *this,void *param_1)

{
  void *this_00;
  float10 fVar1;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  this_00 = (void *)FUN_0044c670((void *)((int)this + 0x6c),*(int *)((int)this + 0x13c) + 0xa6,0);
  if (this_00 != (void *)0x0) {
    FUN_00422e10(&local_14);
    FUN_004376e0(this_00,&local_14);
    local_14 = (DAT_0056c8d0 / DAT_005b8818) * local_14;
    local_10 = (DAT_0056c8d0 / DAT_005b8818) * local_10;
    if (*(int *)((int)this + 0x114) == 1) {
      fVar1 = FUN_004379c0((int)this_00);
      if (DAT_0056c8cc <= (float)fVar1) {
        local_14 = (DAT_0056c8cc * DAT_0056cd94 - DAT_0056d7bc) + local_14;
      }
      else {
        fVar1 = FUN_004379c0((int)this_00);
        local_14 = (((float)fVar1 + _DAT_00570aac) * DAT_0056cd94 - DAT_0056d7bc) + local_14;
      }
    }
    else if (*(int *)((int)this + 0x114) == 0) {
      local_14 = local_14 - _DAT_00570ac4;
    }
    else if (*(int *)((int)this + 0x114) == 2) {
      fVar1 = FUN_004379c0((int)this_00);
      local_14 = (((float)fVar1 + _DAT_00570aac) * DAT_0056cd90 - DAT_0056d7bc) + local_14;
    }
    FUN_0045dd60(param_1,&local_14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

