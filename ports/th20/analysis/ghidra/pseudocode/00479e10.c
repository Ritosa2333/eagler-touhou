/* Entry: 0x00479e10; symbol: FUN_00479e10; body bytes: 371 */

void __fastcall FUN_00479e10(undefined4 *param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  int iVar2;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 *local_24;
  void *local_20;
  default_delete<class_std::_Facet_base> *local_1c;
  void *local_18;
  default_delete<class_std::_Facet_base> *local_14;
  void *local_10;
  default_delete<class_std::_Facet_base> *local_c;
  undefined4 *local_8;
  
  local_8 = param_1;
  if (param_1[0x34] == 0) {
    if (param_1[0x28] != 0) {
      pdVar1 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 1));
      FUN_00426eb0(&DAT_005ba830,0x1b,*(float *)pdVar1);
      local_10 = (void *)FUN_00478e80(0);
      local_c = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)(local_8 + 1));
      FUN_0047cf60(local_10,(float *)local_c,DAT_0056cd98,0,99999,0);
      local_18 = (void *)FUN_00478ee0(0);
      local_14 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_8 + 1));
      FUN_004caad0(local_18,(undefined4 *)local_14,DAT_0056cd98,1,1);
      local_20 = (void *)FUN_00478ec0(0);
      local_1c = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_8 + 1));
      local_24 = FUN_004c1b80(local_20,&local_28,(undefined4 *)local_1c,DAT_0056fa28,DAT_0056d7bc,
                              0xb,100);
      FUN_004c1c10(local_24,&local_2c);
    }
    FUN_00479040(local_8);
    local_8[0x28] = 0;
    iVar2 = FUN_0040c300(local_8 + 0x33);
    if (iVar2 != 0) {
      FUN_004c1f00(local_8 + 0x33);
    }
    FUN_004117a0(local_8 + 0x33,0);
  }
  else {
    FUN_0044fcd0(param_1);
  }
  return;
}

