/* Entry: 0x0054f788; symbol: scan_optional_wide_modifier; body bytes: 42 */

/* Library Function - Single Match
    private: void __thiscall
   __crt_stdio_input::format_string_parser<wchar_t>::scan_optional_wide_modifier(void)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall
__crt_stdio_input::format_string_parser<wchar_t>::scan_optional_wide_modifier
          (format_string_parser<wchar_t> *this)

{
  wchar_t wVar1;
  bool bVar2;
  
  wVar1 = **(wchar_t **)(this + 8);
  if (wVar1 == L'w') {
    *(wchar_t **)(this + 8) = *(wchar_t **)(this + 8) + 1;
  }
  else {
    bVar2 = should_default_to_wide(this,wVar1);
    if (!bVar2) {
      return;
    }
  }
  this[0x2c] = (format_string_parser<wchar_t>)0x1;
  return;
}

