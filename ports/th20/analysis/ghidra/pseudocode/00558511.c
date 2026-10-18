/* Entry: 0x00558511; symbol: ___acrt_update_locale_info; body bytes: 45 */

/* Library Function - Single Match
    ___acrt_update_locale_info
   
   Library: Visual Studio 2019 Release */

void __cdecl ___acrt_update_locale_info(int param_1,int *param_2)

{
  int iVar1;
  
  if ((*param_2 != DAT_005e5664) && ((*(uint *)(param_1 + 0x350) & DAT_005b29f8) == 0)) {
    iVar1 = ___acrt_update_thread_locale_data();
    *param_2 = iVar1;
  }
  return;
}

