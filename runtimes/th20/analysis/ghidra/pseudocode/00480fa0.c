/* Entry: 0x00480fa0; symbol: FUN_00480fa0; body bytes: 602 */

void FUN_00480fa0(int param_1)

{
  bool bVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int iVar3;
  int *piVar4;
  undefined4 *puVar5;
  undefined1 local_7c [8];
  undefined1 local_74 [4];
  undefined4 local_70;
  undefined4 local_6c;
  void *local_68;
  int local_64;
  undefined4 local_60;
  undefined4 local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  COleCurrency *local_44;
  undefined4 local_40;
  int local_3c;
  int local_38;
  undefined1 local_32;
  undefined1 local_31;
  void *local_30;
  COleCurrency local_2c [12];
  int local_20 [2];
  undefined1 local_15;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_3c = FUN_0044eb30(DAT_005c0028,7,(char **)"bullet.anm");
  *(int *)((int)local_30 + 0x286d8c) = local_3c;
  if (local_3c == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056fc8c);
  }
  else {
    local_4c = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
    FUN_004117a0(local_4c,local_30);
    FUN_00486530(local_30,param_1);
    FUN_004776a0(DAT_005c0028,*(undefined4 *)((int)local_30 + 0x286d9c));
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)local_30 + 0x60));
    *(default_delete<class_std::_Facet_base> **)((int)local_30 + 0x10) = pdVar2;
    local_50 = (void *)((int)local_30 + 0x60);
    iVar3 = FUN_0047c6a0(local_50,2000);
    *(undefined4 *)(iVar3 + 0x50) = 5;
    piVar4 = FUN_00412310(0x26,FUN_004849d0,local_30);
    *(int **)((int)local_30 + 8) = piVar4;
    piVar4 = FUN_004123b0(0x29,FUN_00485310,local_30);
    *(int **)((int)local_30 + 0xc) = piVar4;
    local_54 = (void *)((int)local_30 + 0x286d4c);
    FUN_0044a640(local_54,0);
    local_58 = (void *)((int)local_30 + 0x286d64);
    FUN_0044a640(local_58,0);
    local_38 = 0;
    puVar5 = (undefined4 *)FUN_0047a820(local_74,(void *)((int)local_30 + 0x60));
    local_60 = *puVar5;
    local_5c = local_60;
    iVar3 = FUN_004865f0();
    if (iVar3 < 2000) {
      local_40 = FUN_004865f0();
    }
    else {
      local_40 = 2000;
    }
    puVar5 = FUN_0045f310(local_7c,local_60,local_40);
    local_70 = *puVar5;
    local_6c = puVar5[1];
    local_31 = DAT_0056f348;
    local_32 = DAT_0056f348;
    FUN_00467f50((char **)local_2c);
    local_44 = local_2c;
    FUN_0040c080(local_20,8);
    COleCurrency::operator_union_tagCY(local_44);
    FUN_00470540(&local_15);
    while (bVar1 = FUN_0046b1e0((int)local_20), !bVar1) {
      local_48 = (void *)FUN_0046b250(local_20);
      local_64 = local_38;
      *(int *)((int)local_48 + 0x28) = local_38;
      local_68 = (void *)((int)local_30 + 0x286d4c);
      local_38 = local_38 + 1;
      FUN_0044a6b0(local_68,local_48);
      FUN_0047c730(local_20);
    }
  }
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

