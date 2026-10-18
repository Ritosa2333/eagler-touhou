/* Entry: 0x00503fc0; symbol: FUN_00503fc0; body bytes: 410 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00503fc0(int param_1)

{
  float *pfVar1;
  uint uVar2;
  float10 fVar3;
  undefined4 local_40 [2];
  undefined4 local_38 [2];
  undefined1 local_30 [4];
  undefined4 local_2c;
  default_delete<class_std::_Facet_base> *local_28;
  float local_24;
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
  local_2c = FUN_0045d100(local_18,0);
  local_20 = (void *)FUN_00421830(local_14 + 0x50);
  fVar3 = FUN_004298b0(0x5ba4c4,(DAT_0056e0f0 / _DAT_00570db0) * _DAT_0056ec8c);
  local_1c = (float)fVar3;
  pfVar1 = (float *)FUN_00452fc0(local_20,local_30,local_1c);
  fVar3 = FUN_004292e0(pfVar1);
  local_24 = (float)fVar3;
  local_28 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_14 + 0x50));
  FUN_004790e0(&local_c,(uint *)"effect",0x94,(float *)local_28,local_24,-1,(undefined4 *)0x0);
  local_10 = (void *)FUN_0044ced0(&local_c);
  FUN_00429250(local_38,DAT_0056c8d8,DAT_0056c8d8);
  FUN_00429250(local_40,DAT_0056c8cc,DAT_0056c8cc);
  FUN_00438df0(local_10,0x14,0,local_40,local_38);
  uVar2 = FUN_00423ea0(&DAT_005ba4c4,0x80);
  FUN_00506c60(local_10,(char)uVar2 + '\x7f');
  uVar2 = FUN_00423ea0(&DAT_005ba4c4,0x80);
  FUN_00506c40(local_10,(char)uVar2 + '@');
  uVar2 = FUN_00423ea0(&DAT_005ba4c4,0x40);
  FUN_00506c20(local_10,(char)uVar2 + '@');
  FUN_00504af0(local_14);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

