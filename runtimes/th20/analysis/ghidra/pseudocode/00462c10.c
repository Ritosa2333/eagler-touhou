/* Entry: 0x00462c10; symbol: FUN_00462c10; body bytes: 112 */

undefined4 __fastcall FUN_00462c10(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_8;
  
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)(param_1 + 0x128));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00421970(param_1 + 0x128);
  for (; local_8 != pdVar1; local_8 = local_8 + 4) {
    FUN_0044fcd0((undefined4 *)local_8);
  }
  FUN_0044fcd0((undefined4 *)(param_1 + 0x138));
  FUN_0044fcd0((undefined4 *)(param_1 + 0x13c));
  return 0;
}

