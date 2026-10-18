/* Entry: 0x004b7b80; symbol: FUN_004b7b80; body bytes: 68 */

void __fastcall
FUN_004b7b80(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  
  FUN_004b78c0(param_1);
  while( true ) {
    bVar1 = FUN_004b7d80(param_1);
    if (bVar1) break;
    FUN_004b84a0(param_1);
  }
  pdVar2 = FUN_004b6da0(param_1);
  if (*(int *)pdVar2 != 0) {
    FUN_004b78e0(param_1);
  }
  return;
}

