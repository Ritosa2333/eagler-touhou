/* Entry: 0x00555b94; symbol: _expandlocale_locale_name_cache; body bytes: 73 */

/* Library Function - Single Match
    public: __thiscall _expandlocale_locale_name_cache::_expandlocale_locale_name_cache(wchar_t *
   const,unsigned int,struct __crt_qualified_locale_data * const)
   
   Library: Visual Studio 2019 Release */

_expandlocale_locale_name_cache * __thiscall
_expandlocale_locale_name_cache::_expandlocale_locale_name_cache
          (_expandlocale_locale_name_cache *this,wchar_t *param_1,uint param_2,
          __crt_qualified_locale_data *param_3)

{
  int iVar1;
  
  *(__crt_qualified_locale_data **)(this + 8) = param_3;
  *(wchar_t **)this = param_1;
  *(uint *)(this + 4) = param_2;
  this[0xc] = (_expandlocale_locale_name_cache)0x0;
  iVar1 = FUN_0055f860(param_1,param_2,(int)(param_3 + 0x250),0x55);
  if (iVar1 == 0) {
    return this;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

