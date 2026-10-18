/* Entry: 0x00502f70; symbol: FUN_00502f70; body bytes: 711 */

void __fastcall FUN_00502f70(int param_1)

{
  bool bVar1;
  float *pfVar2;
  float10 fVar3;
  undefined1 local_7c [12];
  void *local_70;
  default_delete<class_std::_Facet_base> *local_6c;
  float local_68;
  undefined4 local_64;
  default_delete<class_std::_Facet_base> *local_60;
  float local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  float local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  uint local_30;
  float local_2c;
  int local_28;
  int local_24;
  float local_20 [3];
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = param_1;
  local_34 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x120));
  local_30 = *(uint *)(local_24 + 0xd8);
  local_28 = FUN_00506480(local_34,local_30);
  local_38 = (void *)(local_24 + 0x1c);
  bVar1 = FUN_004785e0(local_38,*(int *)(local_28 + 0x40));
  if (bVar1) {
    local_3c = (void *)(local_24 + 0x50);
    FUN_0047a5c0(local_3c,0);
    local_40 = (void *)(local_24 + 0x50);
    FUN_00506e70(local_40,1);
    FUN_00486310((void *)(local_24 + 0x18));
    if (*(int *)(local_24 + 0x118) == 0) {
      local_2c = DAT_005732dc;
    }
    else {
      local_2c = DAT_0056e04c;
    }
    FUN_00426eb0(&DAT_005ba830,0x1b,local_2c);
  }
  else {
    local_44 = (void *)(local_24 + 0x1c);
    bVar1 = FUN_00461070(local_44,*(int *)(local_28 + 0x40));
    if (bVar1) {
      local_48 = (void *)(local_24 + 0x1c);
      bVar1 = FUN_00423590(local_48,*(int *)(local_28 + 0x40) + 10);
      if (bVar1) {
        FUN_00422e10(local_14);
        pfVar2 = (float *)FUN_00421830(local_24 + 0x50);
        fVar3 = FUN_004292e0(pfVar2);
        local_4c = (float)fVar3;
        FUN_00422dd0(local_7c,DAT_0056c8e0,0,0);
        FUN_00458fa0(local_7c,local_14,(float *)local_7c,local_4c);
        local_50 = (void *)(local_24 + 0x50);
        FUN_00506010(local_50,local_14);
      }
    }
  }
  local_54 = (void *)(local_24 + 0x1c);
  bVar1 = FUN_004235c0(local_54,*(int *)(local_28 + 0x40) + 10);
  if (bVar1) {
    local_58 = (void *)(local_24 + 0x1c);
    bVar1 = FUN_00423560(local_58,*(int *)(local_28 + 0x40) + 0x14);
    if (bVar1) {
      FUN_00422dd0(local_20,DAT_0056f7b4,DAT_0056fc04,0);
      local_64 = FUN_0040c300(*(undefined4 **)(local_24 + 0x120));
      pfVar2 = (float *)FUN_00421830(local_24 + 0x50);
      fVar3 = FUN_004292e0(pfVar2);
      local_5c = (float)fVar3;
      local_60 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_24 + 0x50));
      FUN_0047cc90((float *)local_60,local_20,local_5c,0,0);
      local_70 = (void *)FUN_0041ca90(*(int *)(local_24 + 0x120));
      pfVar2 = (float *)FUN_00421830(local_24 + 0x50);
      fVar3 = FUN_004292e0(pfVar2);
      local_68 = (float)fVar3;
      local_6c = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_24 + 0x50));
      FUN_004c9db0(local_70,(undefined4 *)local_6c,local_20,local_68,0,1);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

