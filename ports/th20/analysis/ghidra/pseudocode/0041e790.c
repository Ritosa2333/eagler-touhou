/* Entry: 0x0041e790; symbol: FUN_0041e790; body bytes: 55 */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
FUN_0041e790(void *this,
            basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_5;
  
  pdVar1 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first(&local_5);
  FUN_004189c0(this,param_1,pdVar1);
  return param_1;
}

