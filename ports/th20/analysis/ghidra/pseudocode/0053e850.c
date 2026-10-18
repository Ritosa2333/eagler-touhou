/* Entry: 0x0053e850; symbol: FUN_0053e850; body bytes: 134 */

undefined4 __thiscall FUN_0053e850(void *this,int param_1)

{
  code *pcVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_005403d0((int)this);
  if (((uint)*(ushort *)(iVar2 + 8) & 1 << ((byte)param_1 & 0x1f)) == 0) {
    uVar3 = 0;
  }
  else if (*(int *)(iVar2 + 0x10 + param_1 * 4) < 0) {
    pcVar1 = *(code **)(**(int **)((int)this + 0x28) + 0xc);
    uVar3 = (*pcVar1)(*(undefined4 *)(iVar2 + 0x10 + param_1 * 4),pcVar1,
                      *(undefined4 *)((int)this + 0x28));
  }
  else {
    uVar3 = FUN_0053e6a0((void *)((int)this + 0xc),*(int *)(iVar2 + 0x10 + param_1 * 4));
  }
  return uVar3;
}

