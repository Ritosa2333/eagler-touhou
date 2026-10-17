/* Entry: 0x0054f295; symbol: reset_token_state_for_error; body bytes: 43 */

/* Library Function - Single Match
    private: void __thiscall
   __crt_stdio_input::format_string_parser<wchar_t>::reset_token_state_for_error(int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall
__crt_stdio_input::format_string_parser<wchar_t>::reset_token_state_for_error
          (format_string_parser<wchar_t> *this,int param_1)

{
  *(undefined4 *)(this + 0x14) = 0;
  *(undefined4 *)(this + 0x10) = 0;
  this[0x18] = (format_string_parser<wchar_t>)0x0;
  *(undefined4 *)(this + 0x20) = 0;
  *(undefined4 *)(this + 0x24) = 0;
  *(undefined4 *)(this + 0x28) = 0;
  this[0x2c] = (format_string_parser<wchar_t>)0x0;
  *(undefined4 *)(this + 0x30) = 0;
  *(int *)(this + 0xc) = param_1;
  return;
}

