/* Entry: 0x0041f1d0; symbol: __vswprintf_s_l; body bytes: 77 */

/* Library Function - Single Match
    __vswprintf_s_l
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

int __cdecl
__vswprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,wchar_t *_Format,_locale_t _Locale,va_list _ArgList
               )

{
  uint *puVar1;
  int local_c;
  
  puVar1 = (uint *)FUN_0041f1c0();
  local_c = FUN_0054a498(*puVar1,puVar1[1],(undefined1 ****)_DstBuf,_DstSize,(int)_Format,
                         &_Locale->locinfo,_ArgList);
  if (local_c < 0) {
    local_c = -1;
  }
  return local_c;
}

