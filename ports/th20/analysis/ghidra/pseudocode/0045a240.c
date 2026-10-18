/* Entry: 0x0045a240; symbol: FUN_0045a240; body bytes: 540 */

void __thiscall FUN_0045a240(void *this,int *param_1,int param_2,uint param_3)

{
  int iVar1;
  int local_28;
  int local_24;
  uint local_20;
  uint *local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = (uint *)0x0;
  local_c = 0;
  local_10 = 0;
  if (((param_1 != (int *)0x0) && (iVar1 = FUN_0045b780(this,param_1,(undefined4 *)0x0), -1 < iVar1)
      ) && (iVar1 = (**(code **)(*param_1 + 0x2c))
                              (param_1,0,*(undefined4 *)((int)this + 0xc),&local_14,&local_c,0,0,0),
           -1 < iVar1)) {
    FUN_0045b5f0(*(void **)((int)this + 0x10),'\0',param_3);
    iVar1 = FUN_0045aff0(*(void **)((int)this + 0x10),local_14,local_c,&local_10);
    if (-1 < iVar1) {
      if (local_10 == 0) {
        if (*(short *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x2e) == 8) {
          local_24 = 0x80;
        }
        else {
          local_24 = 0;
        }
        _memset(local_14,local_24,local_c);
      }
      else if (local_10 < local_c) {
        if (param_2 == 0) {
          if (*(short *)(*(int *)(*(int *)((int)this + 0x10) + 0x90) + 0x2e) == 8) {
            local_28 = 0x80;
          }
          else {
            local_28 = 0;
          }
          _memset((void *)((int)local_14 + local_10),local_28,local_c - local_10);
        }
        else {
          for (local_20 = local_10; local_20 < local_c; local_20 = local_20 + local_10) {
            iVar1 = FUN_0045b5f0(*(void **)((int)this + 0x10),'\x01',0);
            if ((iVar1 < 0) ||
               (iVar1 = FUN_0045aff0(*(void **)((int)this + 0x10),(uint *)((int)local_14 + local_20)
                                     ,local_c - local_20,&local_10), iVar1 < 0)) goto LAB_0045a44c;
          }
        }
      }
      (**(code **)(*param_1 + 0x4c))(param_1,local_14,local_c,0,0);
    }
  }
LAB_0045a44c:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

