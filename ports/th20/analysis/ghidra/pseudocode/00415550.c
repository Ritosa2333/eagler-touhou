/* Entry: 0x00415550; symbol: FUN_00415550; body bytes: 93 */

void * __thiscall FUN_00415550(void *this,void *param_1,int *param_2)

{
  int iVar1;
  char **ppcVar2;
  void *pvVar3;
  
  iVar1 = *param_2;
  ppcVar2 = (char **)FUN_00414f90((void *)((int)this + 4),param_2);
  pvVar3 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  FUN_00413690(pvVar3,ppcVar2);
  FUN_00414b20(this,param_1,iVar1);
  return param_1;
}

