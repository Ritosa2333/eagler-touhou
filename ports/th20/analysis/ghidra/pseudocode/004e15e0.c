/* Entry: 0x004e15e0; symbol: FUN_004e15e0; body bytes: 154 */

void __thiscall FUN_004e15e0(void *this,undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  *(undefined4 *)((int)this + 0xcc) = param_1;
  if (*(int *)((int)this + 0xcc) < 0) {
    *(undefined4 *)((int)this + 0xcc) = 0;
  }
  iVar1 = FUN_004b7e20((int)this);
  if (iVar1 < *(int *)((int)this + 0xcc)) {
    uVar2 = FUN_004b7e20((int)this);
    *(undefined4 *)((int)this + 0xcc) = uVar2;
  }
  if (DAT_005c06a4 != (void *)0x0) {
    iVar1 = FUN_004b7e20((int)this);
    iVar3 = FUN_004b7e70((int)this);
    iVar4 = FUN_00477ff0((int)this);
    FUN_004b8650(DAT_005c06a4,iVar4,iVar3,iVar1);
  }
  return;
}

