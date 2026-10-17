/* Entry: 0x0044b740; symbol: FUN_0044b740; body bytes: 139 */

void __fastcall
FUN_0044b740(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  void *pvVar3;
  undefined1 local_5;
  
  p_Var2 = param_1 + 4;
  FUN_0040e5e0();
  bVar1 = FUN_0040f180((int)p_Var2);
  if (bVar1) {
    pvVar3 = (void *)FUN_0040f160(param_1);
    FUN_0044b380(pvVar3,*(char **)p_Var2,*(int *)(param_1 + 0x18));
    FUN_0040f6a0((char **)p_Var2);
  }
  *(undefined4 *)(param_1 + 0x14) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0xf;
  local_5 = 0;
  FUN_0040fe90(p_Var2,&local_5);
  return;
}

