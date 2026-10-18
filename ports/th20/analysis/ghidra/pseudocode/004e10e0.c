/* Entry: 0x004e10e0; symbol: FUN_004e10e0; body bytes: 187 */

void __thiscall FUN_004e10e0(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  *(int *)((int)this + 0xcc) = *(int *)((int)this + 0xcc) + param_1;
  iVar1 = FUN_004b7e20((int)this);
  if (iVar1 <= *(int *)((int)this + 0xcc)) {
    *(undefined4 *)((int)this + 0xd0) = 0;
  }
  iVar1 = FUN_004b7e20((int)this);
  if (iVar1 < *(int *)((int)this + 0xcc)) {
    uVar2 = FUN_004b7e20((int)this);
    *(undefined4 *)((int)this + 0xcc) = uVar2;
  }
  else {
    FUN_00426d70(&DAT_005ba830,0x2e,0);
  }
  if (DAT_005c06a4 != (void *)0x0) {
    iVar1 = FUN_004b7e20((int)this);
    iVar3 = FUN_004b7e70((int)this);
    iVar4 = FUN_00477ff0((int)this);
    FUN_004b8650(DAT_005c06a4,iVar4,iVar3,iVar1);
  }
  return;
}

