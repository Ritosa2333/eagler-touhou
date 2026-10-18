/* Entry: 0x004893f0; symbol: FUN_004893f0; body bytes: 309 */

void __thiscall FUN_004893f0(void *this,int param_1)

{
  int *unaff_FS_OFFSET;
  undefined4 local_1c;
  int local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005692bd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1 != 0) {
                    /* WARNING: Load size is inaccurate */
    FUN_00413f50(&local_1c,*this);
    local_8 = 0;
    if (*(int *)((int)this + 4) == 0) {
      FUN_00497560(&local_1c);
      move<>(local_18 + 4);
      FUN_0048ad50();
      *(int *)((int)this + 8) = local_18;
      *(int *)((int)this + 4) = local_18;
      param_1 = param_1 + -1;
    }
    for (; param_1 != 0; param_1 = param_1 + -1) {
      FUN_00497560(&local_1c);
      move<>(local_18 + 4);
      FUN_0048ad50();
      FUN_0040cd70(*(undefined4 *)((int)this + 4),&local_18);
      *(int *)((int)this + 4) = local_18;
    }
    FUN_00484720((int)&local_1c);
    local_8 = 0xffffffff;
    FUN_0048b750(&local_1c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

