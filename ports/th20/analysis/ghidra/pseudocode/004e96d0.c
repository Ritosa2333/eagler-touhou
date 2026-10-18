/* Entry: 0x004e96d0; symbol: FUN_004e96d0; body bytes: 52 */

undefined4 * __cdecl FUN_004e96d0(undefined4 *param_1,undefined4 param_2)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *this;
  default_delete<class_std::_Facet_base> *this_00;
  undefined1 *puVar1;
  undefined4 local_8;
  
  puVar1 = &stack0x0000000c;
  this = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)FUN_004f57f0(&param_2,&local_8);
  this_00 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first(this);
  FUN_004f5690(this_00,puVar1);
  *param_1 = param_2;
  return param_1;
}

