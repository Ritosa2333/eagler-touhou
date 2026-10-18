/* Entry: 0x0054b5c5; symbol: parse_floating_point_possible_nan_is_snan<>; body bytes: 64 */

/* Library Function - Multiple Matches With Same Base Name
    bool __cdecl __crt_strtox::parse_floating_point_possible_nan_is_snan<wchar_t,class
   __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::console_input_adapter<wchar_t> > >(wchar_t &,class
   __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::console_input_adapter<wchar_t> > &)
    bool __cdecl __crt_strtox::parse_floating_point_possible_nan_is_snan<wchar_t,class
   __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::stream_input_adapter<wchar_t> > >(wchar_t &,class
   __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::stream_input_adapter<wchar_t> > &)
    bool __cdecl __crt_strtox::parse_floating_point_possible_nan_is_snan<wchar_t,class
   __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::string_input_adapter<wchar_t> > >(wchar_t &,class
   __crt_strtox::input_adapter_character_source<class
   __crt_stdio_input::string_input_adapter<wchar_t> > &)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

undefined4 __cdecl parse_floating_point_possible_nan_is_snan<>(ushort *param_1,undefined4 *param_2)

{
  ushort uVar1;
  uint uVar2;
  undefined2 extraout_var;
  undefined1 uVar3;
  int iVar4;
  
  uVar3 = 0;
  iVar4 = 0;
  do {
    uVar1 = *param_1;
    uVar2 = (uint)uVar1;
    if ((uVar1 != *(ushort *)((int)&DAT_00598948 + iVar4)) &&
       (uVar1 != *(ushort *)((int)&DAT_00598954 + iVar4))) goto LAB_0054b5fe;
    uVar1 = FUN_0054eec7(param_2);
    uVar2 = CONCAT22(extraout_var,uVar1);
    iVar4 = iVar4 + 2;
    *param_1 = uVar1;
  } while (iVar4 != 10);
  uVar3 = 1;
LAB_0054b5fe:
  return CONCAT31((int3)(uVar2 >> 8),uVar3);
}

