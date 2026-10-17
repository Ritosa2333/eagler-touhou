/* Entry: 0x0045aff0; symbol: FUN_0045aff0; body bytes: 345 */

void __thiscall FUN_0045aff0(void *this,uint *param_1,uint param_2,uint *param_3)

{
  uint local_14;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)((int)this + 0x7c) == 0) {
    if (((*(int *)((int)this + 0x8c) != 0) && (param_1 != (uint *)0x0)) && (param_3 != (uint *)0x0))
    {
      local_14 = param_2;
      if (*(uint *)((int)this + 8) < param_2) {
        local_14 = *(DWORD *)((int)this + 8);
      }
      *(uint *)((int)this + 8) = *(int *)((int)this + 8) - local_14;
      ReadFile(*(HANDLE *)((int)this + 0x8c),param_1,local_14,&local_c,(LPOVERLAPPED)0x0);
      if (param_3 != (uint *)0x0) {
        *param_3 = local_c;
      }
    }
  }
  else if (*(int *)((int)this + 0x84) != 0) {
    if (param_3 != (uint *)0x0) {
      *param_3 = 0;
    }
    if ((uint)(*(int *)((int)this + 0x80) + *(int *)((int)this + 0x88)) <
        *(int *)((int)this + 0x84) + param_2) {
      param_2 = *(int *)((int)this + 0x88) -
                (*(int *)((int)this + 0x84) - *(int *)((int)this + 0x80));
    }
    FUN_00543e20(param_1,*(uint **)((int)this + 0x84),param_2);
    *(uint *)((int)this + 0x84) = *(int *)((int)this + 0x84) + param_2;
    if (param_3 != (uint *)0x0) {
      *param_3 = param_2;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

