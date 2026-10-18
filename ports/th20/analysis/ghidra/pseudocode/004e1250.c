/* Entry: 0x004e1250; symbol: FUN_004e1250; body bytes: 188 */

undefined4 __thiscall FUN_004e1250(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  iVar1 = FUN_004b8100((int)this);
  if (iVar1 <= *(int *)((int)this + 0xb8)) {
    *(undefined4 *)((int)this + 0xc0) = 0;
  }
  *(int *)((int)this + 0xb8) = *(int *)((int)this + 0xb8) + param_1;
  iVar1 = FUN_004b8100((int)this);
  if (iVar1 < *(int *)((int)this + 0xb8)) {
    uVar2 = FUN_004b8100((int)this);
    *(undefined4 *)((int)this + 0xb8) = uVar2;
    *(undefined4 *)((int)this + 0xc0) = 0;
  }
  if (DAT_005c06a4 != (void *)0x0) {
    iVar1 = FUN_004b8100((int)this);
    iVar3 = FUN_004b8150((int)this);
    iVar4 = FUN_004b80b0((int)this);
    FUN_004b8bf0(DAT_005c06a4,iVar4,iVar3,iVar1);
  }
  return 1;
}

