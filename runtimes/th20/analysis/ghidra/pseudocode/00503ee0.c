/* Entry: 0x00503ee0; symbol: FUN_00503ee0; body bytes: 219 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00503ee0(int param_1)

{
  float *pfVar1;
  float10 fVar2;
  undefined1 local_2c [4];
  undefined4 local_28;
  default_delete<class_std::_Facet_base> *local_24;
  float local_20;
  void *local_1c;
  float local_18;
  void *local_14;
  int local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_1;
  local_14 = (void *)FUN_0041cad0(*(int *)(param_1 + 0x120));
  local_28 = FUN_0045d100(local_14,0);
  local_1c = (void *)FUN_00421830(local_10 + 0x50);
  fVar2 = FUN_004298b0(0x5ba4c4,(DAT_0056e0f0 / _DAT_00570db0) * _DAT_0056ec8c);
  local_18 = (float)fVar2;
  pfVar1 = (float *)FUN_00452fc0(local_1c,local_2c,local_18);
  fVar2 = FUN_004292e0(pfVar1);
  local_20 = (float)fVar2;
  local_24 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_10 + 0x50));
  FUN_004790e0(&local_c,(uint *)"effect",0x94,(float *)local_24,local_20,-1,(undefined4 *)0x0);
  FUN_00504af0(local_10);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

