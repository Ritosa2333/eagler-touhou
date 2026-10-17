/* Entry: 0x004aa700; symbol: FUN_004aa700; body bytes: 100 */

void __fastcall
FUN_004aa700(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  undefined4 *puVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  void *pvVar3;
  
  p_Var2 = param_1 + 4;
  FUN_0040e5e0();
  puVar1 = *(undefined4 **)p_Var2;
  pvVar3 = (void *)FUN_0040f160(param_1);
  FUN_004a1dc0(pvVar3,puVar1);
  **(int **)p_Var2 = *(int *)p_Var2;
  *(int *)(*(int *)p_Var2 + 4) = *(int *)p_Var2;
  *(undefined4 *)(param_1 + 8) = 0;
  return;
}

