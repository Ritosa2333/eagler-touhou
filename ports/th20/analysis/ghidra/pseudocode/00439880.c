/* Entry: 0x00439880; symbol: _vswprintf_s; body bytes: 31 */

/* Library Function - Single Match
    _vswprintf_s
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

int __cdecl _vswprintf_s(wchar_t *_Dst,size_t _SizeInWords,wchar_t *_Format,va_list _ArgList)

{
  int iVar1;
  
  iVar1 = __vswprintf_s_l(_Dst,_SizeInWords,_Format,(_locale_t)0x0,_ArgList);
  return iVar1;
}

