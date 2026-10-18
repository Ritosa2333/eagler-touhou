/* Entry: 0x0054f96f; symbol: should_default_to_wide; body bytes: 52 */

/* Library Function - Single Match
    private: bool __thiscall
   __crt_stdio_input::format_string_parser<wchar_t>::should_default_to_wide(wchar_t)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

bool __thiscall
__crt_stdio_input::format_string_parser<wchar_t>::should_default_to_wide
          (format_string_parser<wchar_t> *this,wchar_t param_1)

{
  bool bVar1;
  
  if ((param_1 == L'C') || (param_1 == L'S')) {
    bVar1 = false;
  }
  else if (*(int *)(this + 0x28) == 0xb) {
    bVar1 = true;
  }
  else {
    bVar1 = (*(uint *)this & 2) != 0;
  }
  return bVar1;
}

