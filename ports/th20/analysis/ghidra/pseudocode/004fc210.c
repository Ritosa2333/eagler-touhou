/* Entry: 0x004fc210; symbol: FUN_004fc210; body bytes: 79 */

void __fastcall FUN_004fc210(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00412730(param_1);
  if (iVar1 == 0x100) {
    iVar1 = FUN_00412730(param_1);
    *(int *)(param_1 + 0x110) = *(int *)(param_1 + 0x110) + iVar1;
    FUN_004fb980(param_1);
  }
  return;
}

