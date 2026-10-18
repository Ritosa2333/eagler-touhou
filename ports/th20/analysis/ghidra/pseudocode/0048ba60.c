/* Entry: 0x0048ba60; symbol: FUN_0048ba60; body bytes: 157 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall
FUN_0048ba60(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  char cVar1;
  void *pvVar2;
  char **ppcVar3;
  char **ppcVar4;
  
  pvVar2 = (void *)move<>(param_1);
  if (this != pvVar2) {
    ppcVar3 = (char **)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)this);
    ppcVar4 = (char **)FUN_0040f160(param_1);
    cVar1 = FUN_0048bc80(ppcVar3,ppcVar4);
    if (cVar1 == '\0') {
      FUN_00497900(this,(int)param_1);
    }
    else {
      FUN_00484790((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)this);
      std::_Adl_verify_range<char*,char_const*>(ppcVar3,ppcVar4);
      FID_conflict__Assign_rv((void *)((int)this + 4),(undefined4 *)(param_1 + 4));
    }
  }
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

