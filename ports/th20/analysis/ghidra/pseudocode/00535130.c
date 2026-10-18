/* Entry: 0x00535130; symbol: FUN_00535130; body bytes: 153 */

void FUN_00535130(void)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_c;
  default_delete<class_std::_Facet_base> *local_8;
  
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)&DAT_005c6960);
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00421970(0x5c6960);
  for (; local_8 != pdVar1; local_8 = local_8 + 4) {
    FUN_0044fcd0((undefined4 *)local_8);
    FUN_004117a0(local_8,0);
  }
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)&DAT_005c6974);
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00421970(0x5c6974);
  for (; local_c != pdVar1; local_c = local_c + 4) {
    FUN_004c1f00((undefined4 *)local_c);
  }
  return;
}

