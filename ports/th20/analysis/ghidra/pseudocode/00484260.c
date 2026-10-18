/* Entry: 0x00484260; symbol: FUN_00484260; body bytes: 131 */

void __fastcall FUN_00484260(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_c;
  
  if (*(int *)(param_1 + 8) != 0) {
    FUN_004127f0(*(int *)(param_1 + 8));
  }
  if (*(int *)(param_1 + 0xc) != 0) {
    FUN_004127f0(*(int *)(param_1 + 0xc));
  }
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)(param_1 + 0x60));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00484830(param_1 + 0x60);
  for (; local_c != pdVar1; local_c = local_c + 0x528) {
    FUN_004862b0();
  }
  return;
}

