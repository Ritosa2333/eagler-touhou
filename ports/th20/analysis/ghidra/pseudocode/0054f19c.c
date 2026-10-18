/* Entry: 0x0054f19c; symbol: process_literal_character; body bytes: 53 */

/* Library Function - Single Match
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::process_literal_character(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

bool __thiscall
__crt_stdio_input::input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
::process_literal_character
          (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> *this)

{
  ushort uVar1;
  
  uVar1 = string_input_adapter<wchar_t>::get((string_input_adapter<wchar_t> *)(this + 8));
  if (uVar1 != 0xffff) {
    if (uVar1 == *(ushort *)(this + 0x2c)) {
      return true;
    }
    string_input_adapter<wchar_t>::unget((string_input_adapter<wchar_t> *)(this + 8),uVar1);
  }
  return false;
}

