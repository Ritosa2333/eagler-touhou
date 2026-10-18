/* Entry: 0x0054a6b7; symbol: FID_conflict:convert_hexadecimal_string_to_floating_type<double>; body bytes: 33 */

/* Library Function - Multiple Matches With Different Base Names
    enum SLD_STATUS __cdecl __crt_strtox::convert_decimal_string_to_floating_type<double>(struct
   __crt_strtox::floating_point_string const &,double &)
    enum SLD_STATUS __cdecl __crt_strtox::convert_hexadecimal_string_to_floating_type<double>(struct
   __crt_strtox::floating_point_string const &,double &)
   
   Library: Visual Studio 2019 Release */

void __thiscall
FID_conflict_convert_hexadecimal_string_to_floating_type<double>
          (void *this,uint *param_1,undefined4 param_2)

{
  undefined4 local_c;
  undefined1 local_8;
  undefined3 uStack_7;
  
  local_c = param_2;
  _local_8 = CONCAT31((int3)((uint)this >> 8),1);
  FUN_0054cbf0(param_1,(floating_point_value *)&local_c);
  return;
}

