/* Entry: 0x00536120; symbol: FUN_00536120; body bytes: 199 */

void FUN_00536120(void)

{
  bool bVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  default_delete<class_std::_Facet_base> *local_c;
  default_delete<class_std::_Facet_base> *local_8;
  
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_0041cab0(iVar2);
  if (iVar2 == 5) {
    iVar2 = FUN_00464230(0);
    bVar1 = FUN_004ff840(iVar2);
    if (bVar1) goto LAB_005361a2;
  }
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)&DAT_005c6984);
  pdVar3 = (default_delete<class_std::_Facet_base> *)FUN_00421970(0x5c6984);
  for (; local_8 != pdVar3; local_8 = local_8 + 4) {
    FUN_0044fcd0((undefined4 *)local_8);
    FUN_004117a0(local_8,0);
  }
LAB_005361a2:
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)&DAT_005c6998);
  pdVar3 = (default_delete<class_std::_Facet_base> *)FUN_00421970(0x5c6998);
  for (; local_c != pdVar3; local_c = local_c + 4) {
    FUN_004c1f00((undefined4 *)local_c);
  }
  return;
}

