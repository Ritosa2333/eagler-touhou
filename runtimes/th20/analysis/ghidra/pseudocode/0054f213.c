/* Entry: 0x0054f213; symbol: process_string_specifier; body bytes: 72 */

/* Library Function - Single Match
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::process_string_specifier(enum
   __crt_stdio_input::conversion_mode)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

bool __thiscall
__crt_stdio_input::input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
::process_string_specifier
          (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> *this,
          conversion_mode param_1)

{
  undefined1 uVar1;
  uint uVar2;
  
  if (param_1 == 1) {
    process_whitespace(this);
  }
  uVar2 = FUN_0054ef86((int)(this + 0x18));
  if (uVar2 == 1) {
    uVar2 = FUN_0054bfd0(this,param_1);
    uVar1 = (undefined1)uVar2;
  }
  else if (uVar2 == 2) {
    uVar2 = FUN_0054c145(this,param_1);
    uVar1 = (undefined1)uVar2;
  }
  else {
    uVar1 = 0;
  }
  return (bool)uVar1;
}

