/* Entry: 0x00551f02; symbol: __lock_locales; body bytes: 14 */

/* Library Function - Single Match
    __lock_locales
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __fastcall __lock_locales(void *param_1)

{
  ___acrt_eagerly_load_locale_apis(param_1);
  ___acrt_lock(4);
  return;
}

