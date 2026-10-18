/* Entry: 0x00479fd0; symbol: FUN_00479fd0; body bytes: 192 */

undefined4 FUN_00479fd0(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_18;
  undefined4 local_14;
  default_delete<class_std::_Facet_base> *local_10;
  void *local_c;
  int local_8;
  
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  FUN_00477b80((void *)(local_8 + 0x28),puVar2);
  FUN_00426d70(&DAT_005ba830,0x31,0);
  iVar1 = FUN_00478ea0(0);
  FUN_00487bb0(iVar1);
  local_c = (void *)FUN_00478060(0);
  FUN_00477f40(local_c,1);
  iVar1 = FUN_00460830(0);
  local_14 = FUN_00421810(iVar1);
  local_10 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_8 + 0x28));
  puVar2 = FUN_004790e0(&local_18,(uint *)&DAT_0056fb8c,0x3d,(float *)local_10,0,-1,
                        (undefined4 *)0x0);
  *(undefined4 *)(local_8 + 0x74) = *puVar2;
  FUN_00423520((void *)(local_8 + 8),0);
  return 0;
}

