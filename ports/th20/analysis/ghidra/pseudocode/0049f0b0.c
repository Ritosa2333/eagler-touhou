/* Entry: 0x0049f0b0; symbol: FUN_0049f0b0; body bytes: 219 */

void __fastcall FUN_0049f0b0(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  int *unaff_FS_OFFSET;
  default_delete<class_std::_Facet_base> *local_18;
  default_delete<class_std::_Facet_base> *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567660;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bcf0((undefined4 *)(param_1 + 0xdc));
  local_14 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x34));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_004a0da0(param_1 + 0x34);
  for (; local_14 != pdVar1; local_14 = local_14 + 4) {
    FUN_0044fcd0((undefined4 *)local_14);
  }
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x48));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_004a0da0(param_1 + 0x48);
  for (; local_18 != pdVar1; local_18 = local_18 + 4) {
    FUN_0044fcd0((undefined4 *)local_18);
  }
  FUN_0040b980((undefined4 *)(param_1 + 0xdc));
  *unaff_FS_OFFSET = local_10;
  return;
}

