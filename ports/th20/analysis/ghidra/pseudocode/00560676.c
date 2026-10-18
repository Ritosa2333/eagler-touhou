/* Entry: 0x00560676; symbol: ___acrt_locale_initialize_time; body bytes: 123 */

/* Library Function - Single Match
    ___acrt_locale_initialize_time
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl ___acrt_locale_initialize_time(__crt_locale_data *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined **ppuVar3;
  __crt_lc_time_data *p_Var4;
  
  if (*(int *)(param_1 + 0xb4) == 0) {
    ppuVar3 = &PTR_DAT_00599ed8;
LAB_005606d8:
    ___acrt_locale_release_lc_time_reference(*(undefined ***)(param_1 + 0x9c));
    *(undefined ***)(param_1 + 0x9c) = ppuVar3;
    uVar2 = 0;
  }
  else {
    ppuVar3 = (undefined **)FUN_00557b40(1,0x164);
    p_Var4 = (__crt_lc_time_data *)0x0;
    if ((__crt_lc_time_data *)ppuVar3 != (__crt_lc_time_data *)0x0) {
      bVar1 = initialize_lc_time((__crt_lc_time_data *)ppuVar3,param_1);
      if (bVar1) {
        *(undefined4 *)((int)ppuVar3 + 0xb0) = 1;
        FUN_00557ba0((LPVOID)0x0);
        goto LAB_005606d8;
      }
      ___acrt_locale_free_time(ppuVar3);
      p_Var4 = (__crt_lc_time_data *)ppuVar3;
    }
    FUN_00557ba0(p_Var4);
    uVar2 = 1;
  }
  return uVar2;
}

