/* Entry: 0x00483ad0; symbol: FUN_00483ad0; body bytes: 592 */

void FUN_00483ad0(void)

{
  bool bVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  undefined4 *puVar3;
  int iVar4;
  size_t _Size;
  undefined1 local_80 [8];
  undefined1 local_78 [4];
  undefined1 local_74 [4];
  undefined4 local_70;
  undefined4 local_6c;
  void *local_68;
  void *local_64;
  int local_60;
  void *local_5c;
  undefined4 local_58;
  undefined4 local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  COleCurrency *local_44;
  undefined4 local_40;
  int local_3c;
  void *local_38;
  undefined1 local_32;
  undefined1 local_31;
  void *local_30;
  COleCurrency local_2c [12];
  int local_20 [2];
  undefined1 local_15;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  FUN_004863f0(DAT_005c0028,7,'\0');
  _Size = 0x284da8;
  iVar4 = 0;
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)local_30 + 0x60));
  _memset(pdVar2,iVar4,_Size);
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)local_30 + 0x60));
  *(default_delete<class_std::_Facet_base> **)((int)local_30 + 0x10) = pdVar2;
  local_48 = (void *)((int)local_30 + 0x60);
  iVar4 = FUN_0047c6a0(local_48,2000);
  *(undefined4 *)(iVar4 + 0x50) = 5;
  local_4c = (void *)((int)local_30 + 0x284e08);
  puVar3 = FUN_004142a0(local_74,0);
  FUN_00485360(local_4c,puVar3);
  local_50 = (void *)((int)local_30 + 0x286d4c);
  FUN_0044a640(local_50,0);
  local_3c = 0;
  puVar3 = (undefined4 *)FUN_0047a820(local_78,(void *)((int)local_30 + 0x60));
  local_58 = *puVar3;
  local_54 = local_58;
  iVar4 = FUN_004865f0();
  if (iVar4 < 2000) {
    local_40 = FUN_004865f0();
  }
  else {
    local_40 = 2000;
  }
  puVar3 = FUN_0045f310(local_80,local_58,local_40);
  local_70 = *puVar3;
  local_6c = puVar3[1];
  local_31 = DAT_0056f348;
  local_32 = DAT_0056f348;
  FUN_00467f50((char **)local_2c);
  local_44 = local_2c;
  FUN_0040c080(local_20,8);
  COleCurrency::operator_union_tagCY(local_44);
  FUN_00470540(&local_15);
  while( true ) {
    bVar1 = FUN_0046b1e0((int)local_20);
    if (bVar1) break;
    local_5c = (void *)FUN_0046b250(local_20);
    local_38 = local_5c;
    FUN_0044a670(local_5c,local_5c);
    local_60 = local_3c;
    *(int *)((int)local_38 + 0x28) = local_3c;
    local_64 = (void *)((int)local_30 + 0x286d4c);
    local_3c = local_3c + 1;
    FUN_0044a6b0(local_64,local_38);
    FUN_0047c730(local_20);
  }
  local_68 = (void *)((int)local_30 + 0x286d64);
  FUN_0044a640(local_68,0);
  FUN_00486480(local_30,0,0);
  *(undefined4 *)((int)local_30 + 0x286d80) = 0;
  *(undefined4 *)((int)local_30 + 0x286d90) = 0;
  *(undefined4 *)((int)local_30 + 0x286d94) = 0;
  *(undefined4 *)((int)local_30 + 0x286d98) = 0;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

