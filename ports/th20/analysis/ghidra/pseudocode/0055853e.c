/* Entry: 0x0055853e; symbol: FUN_0055853e; body bytes: 49 */

void __cdecl FUN_0055853e(int param_1,int *param_2,int param_3)

{
  int iVar1;
  
  if ((*param_2 != (&DAT_005e5664)[param_3]) && ((*(uint *)(param_1 + 0x350) & DAT_005b29f8) == 0))
  {
    iVar1 = ___acrt_update_thread_locale_data();
    *param_2 = iVar1;
  }
  return;
}

