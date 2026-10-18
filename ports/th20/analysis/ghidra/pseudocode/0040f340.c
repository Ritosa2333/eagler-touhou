/* Entry: 0x0040f340; symbol: FUN_0040f340; body bytes: 533 */

void __cdecl
FUN_0040f340(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1,undefined4 param_2,undefined4 param_3,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_4,_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     *param_5)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *unaff_FS_OFFSET;
  undefined1 local_ac [8];
  undefined1 local_a4 [8];
  undefined1 local_9c [8];
  undefined1 local_94 [8];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_7c;
  default_delete<class_std::_Facet_base> *local_74;
  default_delete<class_std::_Facet_base> *local_70;
  default_delete<class_std::_Facet_base> *local_6c;
  default_delete<class_std::_Facet_base> *local_68;
  int local_64;
  uint local_60;
  uint local_5c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_56;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_55;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_54 [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_3c [24];
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  int local_14;
  
  puStack_20 = &stack0xfffffffc;
  puStack_18 = &LAB_00567941;
  local_1c = *unaff_FS_OFFSET;
  local_24 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_14 = 0;
  local_5c = 0;
  local_8c = FUN_0040dc60(param_1);
  local_14 = 0;
  local_5c = local_5c | 1;
  local_60 = ___std_fs_code_page_0();
  local_6c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(&local_55);
  local_68 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(param_4);
  puVar2 = (undefined4 *)FUN_0040e7c0(local_68,local_94);
  local_80 = *puVar2;
  local_7c = puVar2[1];
  FUN_0040ce80(local_54,local_60,local_80,local_7c,local_6c);
  local_14 = 1;
  local_74 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(&local_56);
  local_70 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(param_5);
  puVar2 = (undefined4 *)FUN_0040e7c0(local_70,local_9c);
  local_88 = *puVar2;
  local_84 = puVar2[1];
  FUN_0040ce80(local_3c,local_60,local_88,local_84,local_74);
  local_14 = CONCAT31(local_14._1_3_,2);
  bVar1 = FUN_00410360((int)local_3c);
  if (bVar1) {
    local_64 = 4;
  }
  else {
    local_64 = 8;
  }
  iVar3 = FUN_0040ff90((int)&param_2);
  iVar3 = iVar3 + local_64;
  iVar4 = FUN_00411700((int)local_54);
  iVar5 = FUN_00411700((int)local_3c);
  FUN_00411610(param_1,iVar3 + iVar4 + iVar5);
  FUN_0040c750(param_1,&param_2);
  puVar2 = (undefined4 *)FUN_0040eb80(local_a4,&DAT_0056c564,3);
  FUN_0040c750(param_1,puVar2);
  FUN_0040e9f0(param_1,local_54);
  bVar1 = FUN_00410360((int)local_3c);
  if (!bVar1) {
    puVar2 = (undefined4 *)FUN_0040eb80(local_ac,&DAT_0056c568,4);
    FUN_0040c750(param_1,puVar2);
    FUN_0040e9f0(param_1,local_3c);
  }
  std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::operator+=
            ((basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)param_1
             ,'\"');
  local_14._0_1_ = 1;
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_3c);
  local_14 = (uint)local_14._1_3_ << 8;
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_54);
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

