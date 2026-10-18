/* Entry: 0x0049e3e0; symbol: FUN_0049e3e0; body bytes: 66 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall FUN_0049e3e0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)this);
  *(undefined ***)this = &PTR_FUN_00570370;
  puVar1 = (undefined4 *)move<>(param_1);
  FUN_00486af0((void *)((int)this + 4),puVar1);
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

