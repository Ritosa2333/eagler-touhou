/* Entry: 0x004e11a0; symbol: FUN_004e11a0; body bytes: 172 */

void __thiscall FUN_004e11a0(void *this,int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = FUN_004b7e20((int)this);
  if (*(int *)((int)this + 0xcc) < iVar1) {
    *(int *)((int)this + 0xd0) = *(int *)((int)this + 0xd0) + param_1;
    if (2 < *(int *)((int)this + 0xd0)) {
      *(undefined4 *)((int)this + 0xd0) = 0;
      FUN_004e10e0(this,1);
    }
    if (DAT_005c06a4 != (void *)0x0) {
      iVar1 = FUN_004b7e20((int)this);
      iVar2 = FUN_004b7e70((int)this);
      iVar3 = FUN_00477ff0((int)this);
      FUN_004b8650(DAT_005c06a4,iVar3,iVar2,iVar1);
    }
  }
  else {
    *(undefined4 *)((int)this + 0xd0) = 0;
  }
  return;
}

