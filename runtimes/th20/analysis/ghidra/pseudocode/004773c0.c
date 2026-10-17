/* Entry: 0x004773c0; symbol: FUN_004773c0; body bytes: 133 */

void __thiscall FUN_004773c0(void *this,int param_1)

{
  int *local_8;
  
  local_8 = *(int **)((int)this + 0x3330);
  while( true ) {
    if (*local_8 < 0) {
      return;
    }
    if (((short)local_8[1] == 0x10) && (local_8[2] == param_1)) break;
    local_8 = (int *)((int)*(short *)((int)local_8 + 6) + (int)local_8);
  }
  *(int *)((int)this + 0x20) = (int)local_8 - *(int *)((int)this + 0x3330);
  FUN_00423520((void *)((int)this + 0x10),
               *(int *)(*(int *)((int)this + 0x3330) + *(int *)((int)this + 0x20)));
  return;
}

