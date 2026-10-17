/* Entry: 0x0054c2bd; symbol: skip_whitespace<class___crt_stdio_input::string_input_adapter,wchar_t>; body bytes: 47 */

/* Library Function - Single Match
    unsigned short __cdecl __crt_stdio_input::skip_whitespace<class
   __crt_stdio_input::string_input_adapter,wchar_t>(class
   __crt_stdio_input::string_input_adapter<wchar_t> &,struct __crt_locale_pointers * const)
   
   Library: Visual Studio 2019 Release */

ushort __cdecl
__crt_stdio_input::skip_whitespace<class___crt_stdio_input::string_input_adapter,wchar_t>
          (string_input_adapter<wchar_t> *param_1,__crt_locale_pointers *param_2)

{
  ushort _C;
  int iVar1;
  
  do {
    _C = string_input_adapter<wchar_t>::get(param_1);
    if (_C == 0xffff) {
      return 0xffff;
    }
    iVar1 = _iswctype(_C,8);
  } while (iVar1 != 0);
  return _C;
}

