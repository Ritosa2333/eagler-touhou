/* Entry: 0x00477480; symbol: FUN_00477480; body bytes: 160 */

void __thiscall FUN_00477480(void *this,undefined4 param_1)

{
  void *local_14;
  int local_10;
  int local_8;
  
  if (*(int *)((int)this + 0x3320) != 0) {
    local_14 = *(void **)((int)this + 0x3320);
    for (local_10 = 0; local_10 < *(short *)(*(int *)((int)this + 0x3324) + 2);
        local_10 = local_10 + 1) {
      FUN_00477450(local_14,param_1);
      local_14 = (void *)((int)local_14 + 0x5e4);
    }
  }
  for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
    FUN_00477450((void *)((int)this + local_8 * 0x5e4 + 900),param_1);
  }
  return;
}

