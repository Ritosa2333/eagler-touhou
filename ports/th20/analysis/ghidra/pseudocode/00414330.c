/* Entry: 0x00414330; symbol: ~input_processor<>; body bytes: 23 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::stream_input_adapter<wchar_t> >::~input_processor<wchar_t,class
   __crt_stdio_input::stream_input_adapter<wchar_t> >(void)
    public: __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::~input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >(void)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2017 Debug, Visual Studio 2019 Debug */

void __fastcall ~input_processor<>(int param_1)

{
  __crt_stdio_input::format_string_parser<wchar_t>::~format_string_parser<wchar_t>
            ((format_string_parser<wchar_t> *)(param_1 + 8));
  return;
}

