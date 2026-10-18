/* Entry: 0x00418f20; symbol: FUN_00418f20; body bytes: 105 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall
FUN_00418f20(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  void *pvVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  pvVar1 = (void *)move<>(param_1);
  if (this != pvVar1) {
    uVar2 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    uVar3 = FUN_0040f160(param_1);
    FUN_0040fa70((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    FUN_00418280(uVar2,uVar3);
    FUN_0041bc60(this,(char **)param_1);
  }
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

