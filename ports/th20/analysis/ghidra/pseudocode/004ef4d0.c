/* Entry: 0x004ef4d0; symbol: FUN_004ef4d0; body bytes: 66 */

undefined4 * __cdecl
FUN_004ef4d0(undefined4 *param_1,undefined1 *param_2,undefined1 *param_3,undefined4 param_4)

{
  default_delete<class_std::_Facet_base> *this;
  undefined1 *puVar1;
  
  while (param_2 != param_3) {
    puVar1 = param_2;
    this = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)&param_4);
    FUN_004f5690(this,puVar1);
    param_2 = param_2 + 1;
    std::
    _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
    _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&param_4);
  }
  *param_1 = param_4;
  return param_1;
}

