/* Entry: 0x00548966; symbol: FUN_00548966; body bytes: 128 */

bool __thiscall FUN_00548966(void *this,uint param_1,int param_2)

{
  uint uVar1;
  bool bVar2;
  void *local_8;
  
  if (param_1 < 0x80000000) {
    uVar1 = param_1 * 2;
    if (((*(int *)((int)this + 0x404) == 0) && (uVar1 < 0x401)) ||
       (uVar1 <= *(uint *)((int)this + 0x400))) {
      bVar2 = true;
    }
    else {
      local_8 = this;
      local_8 = FUN_005584c0(uVar1);
      bVar2 = local_8 != (LPVOID)0x0;
      if (bVar2) {
        FUN_005494a8((void *)((int)this + 0x404),&local_8);
        *(uint *)((int)this + 0x400) = uVar1;
      }
      FUN_00557ba0(local_8);
    }
  }
  else {
    *(undefined1 *)(param_2 + 0x1c) = 1;
    *(undefined4 *)(param_2 + 0x18) = 0xc;
    bVar2 = false;
  }
  return bVar2;
}

