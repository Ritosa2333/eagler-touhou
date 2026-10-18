/* Entry: 0x0054f25b; symbol: process_whitespace; body bytes: 32 */

/* Library Function - Single Match
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::process_whitespace(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

bool __thiscall
__crt_stdio_input::input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
::process_whitespace
          (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> *this)

{
  ushort uVar1;
  
  uVar1 = skip_whitespace<class___crt_stdio_input::string_input_adapter,wchar_t>
                    ((string_input_adapter<wchar_t> *)(this + 8),
                     *(__crt_locale_pointers **)(this + 0x50));
  string_input_adapter<wchar_t>::unget((string_input_adapter<wchar_t> *)(this + 8),uVar1);
  return true;
}

