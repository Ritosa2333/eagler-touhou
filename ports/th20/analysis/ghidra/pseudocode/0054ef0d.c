/* Entry: 0x0054ef0d; symbol: get; body bytes: 26 */

/* Library Function - Single Match
    public: unsigned short __thiscall __crt_stdio_input::string_input_adapter<wchar_t>::get(void)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

ushort __thiscall
__crt_stdio_input::string_input_adapter<wchar_t>::get(string_input_adapter<wchar_t> *this)

{
  ushort uVar1;
  ushort *puVar2;
  
  puVar2 = *(ushort **)(this + 8);
  if (puVar2 == *(ushort **)(this + 4)) {
    return 0xffff;
  }
  uVar1 = *puVar2;
  *(ushort **)(this + 8) = puVar2 + 1;
  return uVar1;
}

