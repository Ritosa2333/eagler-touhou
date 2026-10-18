/* Entry: 0x0045f5e0; symbol: FUN_0045f5e0; body bytes: 140 */

undefined4 __fastcall FUN_0045f5e0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  default_delete<class_std::_Facet_base> *pdVar2;
  float10 fVar3;
  default_delete<class_std::_Facet_base> *local_8;
  
  bVar1 = FUN_0045d030((int *)(param_1 + 0xb88));
  if (CONCAT31(extraout_var,bVar1) != 0) {
    local_8 = std::
              _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                            *)(param_1 + 8));
    pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_0045fd40(param_1 + 8);
    for (; local_8 != pdVar2; local_8 = local_8 + 0x3d4) {
      fVar3 = FUN_004379c0(*(int *)(param_1 + 4));
      FUN_0045f670(local_8,(float)fVar3);
    }
  }
  FUN_00423540((undefined4 *)(param_1 + 0xb88));
  return 0;
}

