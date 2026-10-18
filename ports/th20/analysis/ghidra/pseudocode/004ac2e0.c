/* Entry: 0x004ac2e0; symbol: FUN_004ac2e0; body bytes: 59 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall FUN_004ac2e0(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)this);
  *(undefined ***)this = &PTR_FUN_005709b0;
  puVar2 = (undefined4 *)move<>(param_1);
  uVar1 = puVar2[1];
  *(undefined4 *)((int)this + 4) = *puVar2;
  *(undefined4 *)((int)this + 8) = uVar1;
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

