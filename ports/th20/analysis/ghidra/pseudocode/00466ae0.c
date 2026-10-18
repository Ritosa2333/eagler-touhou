/* Entry: 0x00466ae0; symbol: FUN_00466ae0; body bytes: 304 */

void __fastcall FUN_00466ae0(int param_1)

{
  bool bVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int local_28;
  int local_24;
  default_delete<class_std::_Facet_base> *local_1c;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00445bc0(DAT_005c0028,*(int *)(param_1 + 4));
  FUN_00422e10(local_14);
  FUN_004376e0(*(void **)(param_1 + 4),local_14);
  local_1c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 8));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_00466fd0(param_1 + 8);
  for (; local_1c != pdVar2; local_1c = local_1c + 0x168) {
    bVar1 = FUN_00423560((void *)(param_1 + 0x2d08),0x1e);
    if (bVar1) {
      local_24 = FUN_0040ff90(param_1 + 0x2d08);
    }
    else {
      local_24 = FUN_0040ff90(param_1 + 0x2d08);
      local_24 = 0x3c - local_24;
    }
    bVar1 = FUN_00423560((void *)(param_1 + 0x2d08),0x1e);
    if (bVar1) {
      local_28 = 0;
    }
    else {
      local_28 = FUN_00466a00((void *)(param_1 + 0x2d08),0x1e);
    }
    FUN_00466c10(local_1c,local_14,local_28,local_24);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

