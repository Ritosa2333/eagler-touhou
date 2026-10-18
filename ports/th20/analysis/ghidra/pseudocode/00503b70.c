/* Entry: 0x00503b70; symbol: FUN_00503b70; body bytes: 458 */

undefined4 __fastcall FUN_00503b70(void *param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  char **in_stack_ffffffc8;
  undefined4 local_34;
  void *local_30;
  int local_2c;
  undefined4 local_28;
  void *local_24;
  default_delete<class_std::_Facet_base> *local_20;
  int local_1c;
  int local_18;
  void *local_14;
  uint local_10;
  char **local_c;
  void *local_8;
  
  local_8 = param_1;
  local_14 = (void *)FUN_0040ff90(*(int *)((int)param_1 + 0x120));
  local_10 = *(uint *)((int)local_8 + 0xd8);
  local_18 = FUN_00506480(local_14,local_10);
  local_2c = FUN_00412710(*(int *)((int)local_8 + 0x120));
  local_24 = (void *)FUN_00412710(*(int *)((int)local_8 + 0x120));
  local_1c = (int)*(short *)(local_18 + 2);
  local_20 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)local_8 + 0x50));
  puVar1 = FUN_004c1b80(local_24,&local_34,(undefined4 *)local_20,DAT_0056f7b4,DAT_0056c8cc,0x10,
                        local_1c);
  local_28 = *puVar1;
  local_c = (char **)FUN_004c0d00(local_2c);
  if (local_c == (char **)0x0) {
    uVar2 = 0;
  }
  else {
    FUN_0047a4e0(local_c,2);
    FUN_00506e10(local_c,DAT_0056e71c);
    FUN_00506c80(local_c,DAT_0056e0f8 / DAT_0056c8d0);
    FUN_00506cf0(local_c,1);
    puVar1 = FUN_005064b0(local_8,(undefined4 *)&stack0xffffffc8);
    FUN_00479040(puVar1);
    *(undefined4 *)((int)local_8 + 0x9c) = 2;
    local_c = (char **)FUN_004c0e10((undefined4 *)((int)local_8 + 0xdc));
    if (local_c == (char **)0x0) {
      uVar2 = 0;
    }
    else {
      FUN_004c1f30(local_c);
      FUN_004117a0((void *)((int)local_8 + 0xdc),0);
      local_30 = (void *)((int)local_8 + 0x50);
      FUN_0047a5c0(local_30,DAT_0056e71c);
      FUN_004c2060(local_c,(undefined4 *)((int)local_8 + 0x50));
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)local_8 + 0x50));
      FUN_00426eb0(&DAT_005ba830,0x41,*(float *)pdVar3);
      std::_Adl_verify_range<char*,char_const*>((char **)0xc8,in_stack_ffffffc8);
      uVar2 = *(undefined4 *)((int)local_8 + 0xac);
    }
  }
  return uVar2;
}

