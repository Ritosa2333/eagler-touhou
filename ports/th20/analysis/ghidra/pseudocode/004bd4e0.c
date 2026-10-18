/* Entry: 0x004bd4e0; symbol: FUN_004bd4e0; body bytes: 115 */

undefined4 __fastcall FUN_004bd4e0(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_004b7fa0(0x5ba568);
  if (iVar1 == 0) {
    iVar1 = FUN_00464080(0);
    iVar1 = FUN_00412730(iVar1);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_0041cab0(iVar2);
    *(int *)(param_1 + 0x2b8) = iVar2 + iVar1 * 8;
  }
  else {
    iVar1 = FUN_00464080(0);
    iVar1 = FUN_00412730(iVar1);
    *(int *)(param_1 + 0x2b8) = iVar1 + 0x10;
  }
  return *(undefined4 *)(param_1 + 0x2b8);
}

