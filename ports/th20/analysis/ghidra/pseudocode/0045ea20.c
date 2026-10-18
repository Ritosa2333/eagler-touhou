/* Entry: 0x0045ea20; symbol: FUN_0045ea20; body bytes: 156 */

void __fastcall FUN_0045ea20(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00445bc0(DAT_005c0028,*(int *)(param_1 + 4));
  FUN_00422e10(local_14);
  FUN_004376e0(*(void **)(param_1 + 4),local_14);
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 8));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_0045eca0(param_1 + 8);
  for (; local_18 != pdVar1; local_18 = local_18 + 0x3c0) {
    FUN_0045eac0(local_18,local_14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

