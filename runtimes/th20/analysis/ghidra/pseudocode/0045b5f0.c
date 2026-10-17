/* Entry: 0x0045b5f0; symbol: FUN_0045b5f0; body bytes: 396 */

undefined4 __thiscall FUN_0045b5f0(void *this,char param_1,uint param_2)

{
  if (*(int *)((int)this + 0x7c) == 0) {
    if ((*(int *)((int)this + 0x8c) == -1) || (*(int *)((int)this + 0x8c) == 0)) {
      return 0x800401f0;
    }
    if ((param_1 == '\0') || (*(int *)(*(int *)((int)this + 0x90) + 0x18) < 1)) {
      if (*(uint *)(*(int *)((int)this + 0x90) + 0x1c) <= param_2) {
        param_2 = param_2 - (*(int *)(*(int *)((int)this + 0x90) + 0x1c) -
                            *(int *)(*(int *)((int)this + 0x90) + 0x18));
      }
      SetFilePointer(*(HANDLE *)((int)this + 0x8c),
                     DAT_005c0004 + *(int *)(*(int *)((int)this + 0x90) + 0x10) + param_2,(PLONG)0x0
                     ,0);
      *(uint *)((int)this + 8) = *(int *)(*(int *)((int)this + 0x90) + 0x1c) - param_2;
    }
    else {
      SetFilePointer(*(HANDLE *)((int)this + 0x8c),
                     DAT_005c0004 + *(int *)(*(int *)((int)this + 0x90) + 0x18) +
                     *(int *)(*(int *)((int)this + 0x90) + 0x10),(PLONG)0x0,0);
      *(int *)((int)this + 8) =
           *(int *)(*(int *)((int)this + 0x90) + 0x1c) - *(int *)(*(int *)((int)this + 0x90) + 0x18)
      ;
    }
  }
  else {
    *(undefined4 *)((int)this + 0x84) = *(undefined4 *)((int)this + 0x80);
    if (0 < *(int *)(*(int *)((int)this + 0x90) + 0x1c)) {
      *(undefined4 *)((int)this + 0x88) = *(undefined4 *)(*(int *)((int)this + 0x90) + 0x1c);
    }
    if ((param_1 != '\0') && (0 < *(int *)(*(int *)((int)this + 0x90) + 0x18))) {
      *(int *)((int)this + 0x84) =
           *(int *)((int)this + 0x84) + *(int *)(*(int *)((int)this + 0x90) + 0x18);
    }
  }
  return 0;
}

