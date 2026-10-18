/* Entry: 0x004155b0; symbol: FUN_004155b0; body bytes: 79 */

void FUN_004155b0(void)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_8;
  
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)&DAT_005b66f0);
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00414f50(0x5b66f0);
  for (; local_8 != pdVar1; local_8 = local_8 + 4) {
    DeleteObject(*(HGDIOBJ *)local_8);
  }
  return;
}

