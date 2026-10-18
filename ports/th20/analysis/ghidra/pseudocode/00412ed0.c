/* Entry: 0x00412ed0; symbol: FUN_00412ed0; body bytes: 66 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall FUN_00412ed0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)this);
  *(undefined ***)this = &PTR_FUN_0056c8a4;
  puVar1 = (undefined4 *)move<>(param_1);
  FUN_00413d80((void *)((int)this + 8),puVar1);
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

