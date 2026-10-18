/* Entry: 0x004aa6a0; symbol: FUN_004aa6a0; body bytes: 93 */

void __fastcall
FUN_004aa6a0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  _Facet_base *p_Var1;
  void *pvVar2;
  _Facet_base *local_10;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_c;
  _Facet_base *local_8;
  
  local_10 = (_Facet_base *)0x0;
  local_c = param_1;
  local_8 = std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>
                      ((_Facet_base **)(param_1 + 4),&local_10);
  pvVar2 = (void *)FUN_0040f160(local_c);
  while (local_8 != (_Facet_base *)0x0) {
    p_Var1 = *(_Facet_base **)local_8;
    FUN_00489ac0(pvVar2,(char **)local_8);
    local_8 = p_Var1;
  }
  return;
}

