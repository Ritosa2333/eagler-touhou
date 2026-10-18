/* Entry: 0x00466690; symbol: FUN_00466690; body bytes: 157 */

void __fastcall FUN_00466690(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int iVar3;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00445bc0(DAT_005c0028,*(int *)(param_1 + 4));
  FUN_00422e10(local_14);
  FUN_004376e0(*(void **)(param_1 + 4),local_14);
  pdVar1 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(param_1 + 0xf8));
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(param_1 + 8));
  iVar3 = FUN_0040ff90(param_1 + 0x174);
  FUN_0043d1a0(DAT_005c0028,iVar3,local_14,(float *)pdVar2,(float *)pdVar1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

