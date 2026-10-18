/* Entry: 0x00418f90; symbol: FUN_00418f90; body bytes: 114 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall
FUN_00418f90(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  uint *puVar1;
  void *pvVar2;
  char **ppcVar3;
  char **ppcVar4;
  uint *puVar5;
  
  pvVar2 = (void *)move<>(param_1);
  if (this != pvVar2) {
    ppcVar3 = (char **)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)this);
    ppcVar4 = (char **)FUN_0040f160(param_1);
    std::_Adl_verify_range<char*,char_const*>(ppcVar3,ppcVar4);
    puVar1 = *(uint **)(param_1 + 0x10);
    puVar5 = Myptr((undefined4 *)param_1);
    FID_conflict_assign(this,puVar5,puVar1);
  }
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

