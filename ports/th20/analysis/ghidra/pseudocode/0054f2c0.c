/* Entry: 0x0054f2c0; symbol: restore_state; body bytes: 37 */

/* Library Function - Multiple Matches With Same Base Name
    public: bool __thiscall __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::console_input_adapter<wchar_t> >::restore_state(unsigned __int64)
    public: bool __thiscall __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::stream_input_adapter<wchar_t> >::restore_state(unsigned __int64)
    public: bool __thiscall __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::string_input_adapter<wchar_t> >::restore_state(unsigned __int64)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

uint __thiscall restore_state(void *this,int param_1,int param_2)

{
  undefined1 *puVar1;
  uint uVar2;
  
  if ((param_1 == *(int *)((int)this + 0x10)) && (param_2 == *(int *)((int)this + 0x14))) {
    uVar2 = CONCAT31((int3)((uint)param_2 >> 8),1);
  }
  else {
    puVar1 = *(undefined1 **)((int)this + 0x18);
    *puVar1 = 0;
    uVar2 = (uint)puVar1 & 0xffffff00;
  }
  return uVar2;
}

