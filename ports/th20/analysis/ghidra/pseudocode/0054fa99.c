/* Entry: 0x0054fa99; symbol: unget; body bytes: 38 */

/* Library Function - Single Match
    public: void __thiscall __crt_stdio_input::string_input_adapter<wchar_t>::unget(unsigned short)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
__crt_stdio_input::string_input_adapter<wchar_t>::unget
          (string_input_adapter<wchar_t> *this,ushort param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(this + 8);
  if ((iVar1 != *(int *)this) && ((iVar1 != *(int *)(this + 4) || (param_1 != 0xffff)))) {
    *(int *)(this + 8) = iVar1 + -2;
  }
  return;
}

