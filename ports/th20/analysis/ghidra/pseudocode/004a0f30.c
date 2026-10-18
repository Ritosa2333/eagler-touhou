/* Entry: 0x004a0f30; symbol: FUN_004a0f30; body bytes: 95 */

undefined4 __thiscall FUN_004a0f30(void *this,int param_1)

{
  undefined1 (*pauVar1) [16];
  undefined4 uVar2;
  SIZE_T *pSVar3;
  int iVar4;
  
  iVar4 = 0;
  pSVar3 = (SIZE_T *)0x0;
  pauVar1 = (undefined1 (*) [16])FUN_004bd4a0(param_1);
  uVar2 = FUN_00410aa0(pauVar1,pSVar3,iVar4);
  if (*(int *)((int)this + 0x14) != 0) {
    FUN_0041f670(*(LPVOID *)((int)this + 0x14));
    *(undefined4 *)((int)this + 0x14) = 0;
  }
  *(undefined4 *)((int)this + 0x14) = uVar2;
  return *(undefined4 *)((int)this + 0x14);
}

