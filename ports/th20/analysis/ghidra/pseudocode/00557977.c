/* Entry: 0x00557977; symbol: replace_current_thread_locale_nolock; body bytes: 75 */

/* Library Function - Single Match
    void __cdecl replace_current_thread_locale_nolock(struct __acrt_ptd * const,struct
   __crt_locale_data * const)
   
   Library: Visual Studio 2019 Release */

void __cdecl replace_current_thread_locale_nolock(__acrt_ptd *param_1,__crt_locale_data *param_2)

{
  undefined **ppuVar1;
  
  if (*(int *)(param_1 + 0x4c) != 0) {
    FUN_00560b07(*(int *)(param_1 + 0x4c));
    ppuVar1 = *(undefined ***)(param_1 + 0x4c);
    if (((ppuVar1 != DAT_005e5664) && (ppuVar1 != &PTR_DAT_005b2788)) &&
       (ppuVar1[3] == (undefined *)0x0)) {
      FUN_0056093c(ppuVar1);
    }
  }
  *(__crt_locale_data **)(param_1 + 0x4c) = param_2;
  if (param_2 != (__crt_locale_data *)0x0) {
    FUN_005608bf((int)param_2);
  }
  return;
}

