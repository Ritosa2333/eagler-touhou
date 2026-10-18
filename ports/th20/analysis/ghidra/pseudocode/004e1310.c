/* Entry: 0x004e1310; symbol: FUN_004e1310; body bytes: 246 */

void __thiscall FUN_004e1310(void *this,int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_c;
  
  iVar1 = FUN_004b8100((int)this);
  if (*(int *)((int)this + 0xb8) < iVar1) {
    *(int *)((int)this + 0xc0) = *(int *)((int)this + 0xc0) + param_1;
    iVar1 = FUN_004640a0(0x5ba568);
    if (iVar1 == 4) {
      local_c = *(int *)(&DAT_005aff54 + *(int *)((int)this + 0xc4) * 4);
    }
    else {
      local_c = *(int *)(&DAT_005afed8 + *(int *)((int)this + 0xc4) * 4);
    }
    while (local_c <= *(int *)((int)this + 0xc0)) {
      *(int *)((int)this + 0xc0) = *(int *)((int)this + 0xc0) - local_c;
      FUN_004e1510(this);
    }
    if (DAT_005c06a4 != (void *)0x0) {
      iVar1 = FUN_004b8100((int)this);
      iVar2 = FUN_004b8150((int)this);
      iVar3 = FUN_004b80b0((int)this);
      FUN_004b8bf0(DAT_005c06a4,iVar3,iVar2,iVar1);
    }
  }
  else {
    *(undefined4 *)((int)this + 0xc0) = 0;
  }
  return;
}

