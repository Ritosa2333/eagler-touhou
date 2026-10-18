/* Entry: 0x0045b490; symbol: FUN_0045b490; body bytes: 349 */

void __thiscall FUN_0045b490(void *this,uint param_1)

{
  int iVar1;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((**(int **)((int)this + 8) != 0) && (*(int *)((int)this + 0x10) != 0)) {
    *(undefined4 *)((int)this + 0x88) = 0;
    *(undefined4 *)((int)this + 0x8c) = 0;
    *(undefined4 *)((int)this + 0x90) = 0;
    *(undefined4 *)((int)this + 0x94) = 0;
    *(undefined4 *)((int)this + 0x98) = 0;
    iVar1 = FUN_0045b780(this,(int *)**(undefined4 **)((int)this + 8),&local_c);
    if ((-1 < iVar1) &&
       ((local_c == 0 ||
        (iVar1 = FUN_0045a240(this,(int *)**(undefined4 **)((int)this + 8),0,param_1), -1 < iVar1)))
       ) {
      FUN_0045b5f0(*(void **)((int)this + 0x10),*(int *)(*(int *)((int)this + 0x10) + 0x9c) == 1,
                   param_1);
      (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x34))
                (**(undefined4 **)((int)this + 8),0);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

