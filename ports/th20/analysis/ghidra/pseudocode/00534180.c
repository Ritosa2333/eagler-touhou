/* Entry: 0x00534180; symbol: FUN_00534180; body bytes: 134 */

int __fastcall FUN_00534180(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0xc);
  if (iVar2 == 0) {
    iVar1 = *(int *)(param_1 + 4);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_004b81a0(iVar2);
    iVar2 = iVar2 + iVar1 * 0x14;
  }
  else if (iVar2 == 1) {
    iVar1 = *(int *)(param_1 + 4);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_004b81a0(iVar2);
    iVar2 = iVar1 * 0x14 + 5 + iVar2;
  }
  else if (iVar2 == 2) {
    iVar1 = *(int *)(param_1 + 4);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_004b81a0(iVar2);
    iVar2 = iVar1 * 0x14 + 10 + iVar2;
  }
  else {
    iVar2 = 0;
  }
  return iVar2;
}

