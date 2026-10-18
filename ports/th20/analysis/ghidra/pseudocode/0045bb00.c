/* Entry: 0x0045bb00; symbol: FUN_0045bb00; body bytes: 211 */

uint __thiscall FUN_0045bb00(void *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined4 local_10;
  undefined4 local_c;
  
  if (*(int *)((int)this + 8) == 0) {
    local_10 = 0x800401f0;
  }
  else {
    local_10 = 0;
    *(undefined4 *)((int)this + 0x58) = 0;
    *(undefined4 *)((int)this + 0x5c) = 0;
    for (local_c = 0; local_c < *(uint *)((int)this + 0x14); local_c = local_c + 1) {
      uVar1 = (**(code **)(**(int **)(*(int *)((int)this + 8) + local_c * 4) + 0x48))
                        (*(undefined4 *)(*(int *)((int)this + 8) + local_c * 4));
      uVar2 = (**(code **)(**(int **)(*(int *)((int)this + 8) + local_c * 4) + 0x34))
                        (*(undefined4 *)(*(int *)((int)this + 8) + local_c * 4),0);
      local_10 = uVar2 | uVar1 | local_10;
    }
    *(undefined4 *)((int)this + 0x20) = 0;
    if (param_1 != 0) {
      FUN_00459a30(*(char ***)((int)this + 0x10));
    }
  }
  return local_10;
}

