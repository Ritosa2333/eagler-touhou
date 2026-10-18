/* Entry: 0x0054a360; symbol: FUN_0054a360; body bytes: 91 */

void __fastcall FUN_0054a360(int *param_1)

{
  int iVar1;
  
  iVar1 = FUN_00547ea3(param_1);
  param_1[3] = *(int *)(iVar1 + 0x4c);
  param_1[4] = *(int *)(iVar1 + 0x48);
  FUN_0055853e(iVar1,param_1 + 3,param_1[1]);
  FUN_0055859c(iVar1,param_1 + 4,param_1[1]);
  if ((*(uint *)(iVar1 + 0x350) & 2) == 0) {
    *(uint *)(iVar1 + 0x350) = *(uint *)(iVar1 + 0x350) | 2;
    *(undefined1 *)(param_1 + 5) = 2;
  }
  return;
}

