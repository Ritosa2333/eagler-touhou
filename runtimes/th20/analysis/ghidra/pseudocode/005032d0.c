/* Entry: 0x005032d0; symbol: FUN_005032d0; body bytes: 380 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005032d0(int param_1)

{
  float *pfVar1;
  uint uVar2;
  float10 fVar3;
  char **in_stack_ffffffc8;
  undefined1 local_34 [4];
  undefined4 local_30;
  default_delete<class_std::_Facet_base> *local_2c;
  float local_28;
  void *local_24;
  void *local_20;
  float local_1c;
  void *local_18;
  int local_14;
  void *local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = param_1;
  local_18 = (void *)FUN_0041cad0(*(int *)(param_1 + 0x120));
  local_30 = FUN_0045d100(local_18,0);
  local_20 = (void *)FUN_00421830(local_14 + 0x50);
  fVar3 = FUN_004298b0(0x5ba4a8,(DAT_0056e0f0 / _DAT_00570db0) * _DAT_0056ec8c);
  local_1c = (float)fVar3;
  local_24 = FUN_00452fc0(local_20,local_34,local_1c);
  pfVar1 = (float *)FUN_00452fc0(local_24,&stack0xffffffc8,DAT_0056e0f0);
  fVar3 = FUN_004292e0(pfVar1);
  local_28 = (float)fVar3;
  local_2c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_14 + 0x50));
  FUN_004790e0(&local_c,(uint *)"effect",0x94,(float *)local_2c,local_28,-1,(undefined4 *)0x0);
  local_10 = (void *)FUN_0044ced0(&local_c);
  uVar2 = FUN_00423ea0(&DAT_005ba4a8,0x80);
  FUN_00506c60(local_10,(char)uVar2 + '\x7f');
  uVar2 = FUN_00423ea0(&DAT_005ba4a8,0x40);
  FUN_00506c40(local_10,(char)uVar2 + '@');
  uVar2 = FUN_00423ea0(&DAT_005ba4a8,0x40);
  FUN_00506c20(local_10,(char)uVar2 + '@');
  uVar2 = FUN_00423ea0(&DAT_005ba4a8,0x40);
  FUN_00470b60(local_10,(char)uVar2 + '`');
  std::_Adl_verify_range<char*,char_const*>((char **)0x32,in_stack_ffffffc8);
  FUN_00504af0(local_14);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

