/* Entry: 0x0047a090; symbol: FUN_0047a090; body bytes: 277 */

void __thiscall FUN_0047a090(void *this,undefined4 param_1,undefined4 *param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_28;
  undefined4 local_24;
  void *local_20;
  default_delete<class_std::_Facet_base> *local_1c;
  undefined4 local_18;
  default_delete<class_std::_Facet_base> *local_14;
  void *local_10;
  void *local_c;
  undefined4 *local_8;
  
  local_10 = (void *)((int)this + 4);
  local_8 = (undefined4 *)this;
  FUN_0047a5e0(local_10,param_2);
  iVar1 = FUN_00460830(0);
  local_18 = FUN_00421810(iVar1);
  local_14 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_8 + 1));
  puVar2 = FUN_004790e0(&local_24,(uint *)&DAT_0056fb8c,0x2e,(float *)local_14,0,-1,
                        (undefined4 *)0x0);
  *local_8 = *puVar2;
  local_8[0x28] = 1;
  FUN_00423520(local_8 + 0x29,0);
  local_8[0x32] = param_1;
  local_20 = (void *)FUN_00478ec0(0);
  local_1c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_8 + 1));
  puVar2 = FUN_004c1b80(local_20,&local_28,(undefined4 *)local_1c,DAT_0056fc04,0,9999,param_3);
  local_8[0x33] = *puVar2;
  local_c = (void *)FUN_004c0e10(local_8 + 0x33);
  FUN_0047a4e0(local_c,3);
  FUN_0047a4a0((int)local_c);
  return;
}

