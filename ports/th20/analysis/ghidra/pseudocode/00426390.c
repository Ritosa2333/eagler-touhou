/* Entry: 0x00426390; symbol: FUN_00426390; body bytes: 79 */

void __thiscall FUN_00426390(void *this,int param_1)

{
  int *local_8;
  
  *(undefined4 *)((int)this + 4) = 0xffffffff;
  for (local_8 = &DAT_005ae6e8; (local_8 != (int *)0x0 && (*local_8 != param_1));
      local_8 = local_8 + 5) {
  }
  *(int **)((int)this + 8) = local_8;
  *(int *)((int)this + 0xc) = param_1;
  return;
}

