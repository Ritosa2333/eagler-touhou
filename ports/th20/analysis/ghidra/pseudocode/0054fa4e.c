/* Entry: 0x0054fa4e; symbol: unget; body bytes: 75 */

/* Library Function - Single Match
    public: void __thiscall __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::string_input_adapter<wchar_t> >::unget(wchar_t)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
__crt_strtox::
input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
          (input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
           *this,wchar_t param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *(int *)(this + 0x10) - 1;
  uVar2 = *(int *)(this + 0x14) + -1 + (uint)(*(int *)(this + 0x10) != 0);
  *(uint *)(this + 0x10) = uVar1;
  *(uint *)(this + 0x14) = uVar2;
  if ((*(int *)(this + 8) == 0 && *(int *)(this + 0xc) == 0) ||
     ((uVar2 <= *(uint *)(this + 0xc) &&
      ((uVar2 < *(uint *)(this + 0xc) || (uVar1 <= *(uint *)(this + 8))))))) {
    if ((param_1 != L'\0') && (param_1 != L'\xffff')) {
      __crt_stdio_input::string_input_adapter<wchar_t>::unget
                (*(string_input_adapter<wchar_t> **)this,param_1);
    }
  }
  return;
}

