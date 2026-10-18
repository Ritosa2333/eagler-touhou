/* Entry: 0x004c5cc0; symbol: FUN_004c5cc0; body bytes: 54 */

void __thiscall FUN_004c5cc0(void *this,int param_1)

{
  default_delete<class_std::_Facet_base> *this_00;
  default_delete<class_std::_Facet_base> *pdVar1;
  
  this_00 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
  pdVar1 = FUN_004b7880((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  FUN_004c5c80(this_00,*(int *)pdVar1 + param_1);
  return;
}

