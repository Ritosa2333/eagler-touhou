/* Entry: 0x0047c570; symbol: FUN_0047c570; body bytes: 121 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall
FUN_0047c570(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  void *pvVar1;
  char **ppcVar2;
  char **ppcVar3;
  
  pvVar1 = (void *)move<>(param_1);
  if (this != pvVar1) {
    ppcVar2 = (char **)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)this);
    ppcVar3 = (char **)FUN_0040f160(param_1);
    std::_Adl_verify_range<char*,char_const*>(ppcVar2,ppcVar3);
    FUN_0047a8d0(this,*(undefined4 **)(param_1 + 4),
                 (*(int *)(param_1 + 8) - *(int *)(param_1 + 4)) / 0x2c);
  }
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

