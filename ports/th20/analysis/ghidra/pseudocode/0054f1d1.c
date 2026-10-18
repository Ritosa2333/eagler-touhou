/* Entry: 0x0054f1d1; symbol: process_state; body bytes: 66 */

/* Library Function - Single Match
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::process_state(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

bool __thiscall
__crt_stdio_input::input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
::process_state(input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
                *this)

{
  int iVar1;
  undefined1 uVar2;
  bool bVar3;
  uint uVar4;
  
  iVar1 = *(int *)(this + 0x28);
  if (iVar1 == 2) {
    bVar3 = process_whitespace(this);
    return bVar3;
  }
  if (iVar1 != 3) {
    if (iVar1 == 4) {
      uVar4 = FUN_0054f093(this);
      uVar2 = (undefined1)uVar4;
      if ((((bool)uVar2 != false) && (*(int *)(this + 0x48) != 9)) &&
         (this[0x30] ==
          (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>)0x0)) {
        *(int *)(this + 0x58) = *(int *)(this + 0x58) + 1;
        return (bool)uVar2;
      }
    }
    else {
      uVar2 = 0;
    }
    return (bool)uVar2;
  }
  bVar3 = process_literal_character(this);
  return bVar3;
}

