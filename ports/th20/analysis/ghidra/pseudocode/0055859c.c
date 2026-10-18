/* Entry: 0x0055859c; symbol: FUN_0055859c; body bytes: 49 */

void __cdecl FUN_0055859c(int param_1,int *param_2,int param_3)

{
  int iVar1;
  
  if ((*param_2 != (&DAT_005e596c)[param_3]) && ((*(uint *)(param_1 + 0x350) & DAT_005b29f8) == 0))
  {
    iVar1 = FUN_0055ee1e();
    *param_2 = iVar1;
  }
  return;
}

