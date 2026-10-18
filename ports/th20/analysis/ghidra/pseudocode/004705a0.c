/* Entry: 0x004705a0; symbol: FUN_004705a0; body bytes: 90 */

void * __thiscall FUN_004705a0(void *this,void *param_1,int *param_2)

{
  int iVar1;
  char **ppcVar2;
  undefined4 uVar3;
  
  iVar1 = *param_2;
  ppcVar2 = (char **)FUN_00414f90(this,param_2);
  uVar3 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  FUN_00469070(uVar3,ppcVar2);
  FUN_0046ec50(this,param_1,iVar1);
  return param_1;
}

