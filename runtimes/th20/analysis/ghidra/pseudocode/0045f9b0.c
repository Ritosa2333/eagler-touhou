/* Entry: 0x0045f9b0; symbol: FUN_0045f9b0; body bytes: 233 */

void __fastcall FUN_0045f9b0(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  void *this;
  default_delete<class_std::_Facet_base> *local_1c;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00445bc0(DAT_005c0028,*(int *)(param_1 + 4));
  FUN_00422e10(&local_14);
  if (*(char *)(param_1 + 0xba8) == '\0') {
    local_14 = *(float *)(param_1 + 0xb9c);
    local_10 = *(undefined4 *)(param_1 + 0xba0);
    local_c = *(undefined4 *)(param_1 + 0xba4);
  }
  else {
    FUN_004376e0(*(void **)(param_1 + 4),&local_14);
  }
  local_1c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 8));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_0045fd40(param_1 + 8);
  for (; local_1c != pdVar1; local_1c = local_1c + 0x3d4) {
    FUN_0045faa0(local_1c,&local_14);
  }
  this = (void *)FUN_0045f400((void *)(param_1 + 8),0);
  FUN_0045fb10(this,&local_14);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

