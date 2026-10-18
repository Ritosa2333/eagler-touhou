/* Entry: 0x00503d40; symbol: FUN_00503d40; body bytes: 403 */

undefined4 __fastcall FUN_00503d40(void *param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  undefined4 local_3c;
  undefined4 local_38;
  void *local_34;
  int local_30;
  undefined4 local_2c;
  void *local_28;
  default_delete<class_std::_Facet_base> *local_24;
  undefined4 local_20;
  int local_1c;
  void *local_18;
  uint local_14;
  int local_10;
  char **local_c;
  void *local_8;
  
  local_8 = param_1;
  local_18 = (void *)FUN_0040ff90(*(int *)((int)param_1 + 0x120));
  local_14 = *(uint *)((int)local_8 + 0xd8);
  local_10 = FUN_00506480(local_18,local_14);
  local_30 = FUN_00412710(*(int *)((int)local_8 + 0x120));
  local_28 = (void *)FUN_00412710(*(int *)((int)local_8 + 0x120));
  local_1c = (int)*(short *)(local_10 + 2);
  local_20 = *(undefined4 *)(local_10 + 0x68);
  local_24 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)local_8 + 0x50));
  puVar1 = FUN_004c1b80(local_28,&local_38,(undefined4 *)local_24,DAT_0056f7b4,local_20,0x14,
                        local_1c);
  local_2c = *puVar1;
  local_c = (char **)FUN_004c0d00(local_30);
  if (local_c == (char **)0x0) {
    uVar2 = 0;
  }
  else {
    FUN_0047a4e0(local_c,4);
    FUN_00506cf0(local_c,1);
    puVar1 = FUN_005064b0(local_8,&local_3c);
    FUN_00479040(puVar1);
    *(undefined4 *)((int)local_8 + 0x9c) = 2;
    local_34 = (void *)((int)local_8 + 0x50);
    FUN_0047a5c0(local_34,DAT_0056c8d0);
    FUN_004c2060(local_c,(undefined4 *)((int)local_8 + 0x50));
    local_c = (char **)FUN_004c0e10((undefined4 *)((int)local_8 + 0xdc));
    if (local_c == (char **)0x0) {
      uVar2 = 0;
    }
    else {
      FUN_004c1f30(local_c);
      FUN_004117a0((void *)((int)local_8 + 0xdc),0);
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)local_8 + 0x50));
      FUN_00426eb0(&DAT_005ba830,0x41,*(float *)pdVar3);
      uVar2 = *(undefined4 *)((int)local_8 + 0xac);
    }
  }
  return uVar2;
}

