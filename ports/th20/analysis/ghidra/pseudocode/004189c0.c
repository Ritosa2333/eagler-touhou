/* Entry: 0x004189c0; symbol: FUN_004189c0; body bytes: 75 */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
FUN_004189c0(void *this,
            basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *param_1,
            undefined4 param_2)

{
  undefined4 *puVar1;
  undefined1 local_18 [8];
  void *local_10;
  void *local_c;
  undefined4 local_8;
  
  local_8 = 0;
  local_10 = this;
  local_c = this;
  puVar1 = (undefined4 *)FUN_0040e7c0(this,local_18);
  FUN_00418100(param_1,*puVar1,puVar1[1],param_2);
  return param_1;
}

