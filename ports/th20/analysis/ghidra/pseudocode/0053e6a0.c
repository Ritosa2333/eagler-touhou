/* Entry: 0x0053e6a0; symbol: FUN_0053e6a0; body bytes: 120 */

void __thiscall FUN_0053e6a0(void *this,int param_1)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_0053f750((int)this);
  iVar2 = *(int *)((int)this + 0x14) + param_1;
  if (uVar1 < ((int)(iVar2 + (iVar2 >> 0x1f & 3U)) >> 2) + 1U) {
    iVar2 = *(int *)((int)this + 0x14) + param_1;
    FUN_0053f370(this,((int)(iVar2 + (iVar2 >> 0x1f & 3U)) >> 2) + 1);
  }
  iVar2 = *(int *)((int)this + 0x14) + param_1;
  FUN_0053b530(this,(int)(iVar2 + (iVar2 >> 0x1f & 3U)) >> 2);
  return;
}

