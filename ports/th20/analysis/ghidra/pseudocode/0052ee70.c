/* Entry: 0x0052ee70; symbol: FUN_0052ee70; body bytes: 68 */

void __fastcall
FUN_0052ee70(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  int *piVar2;
  
  FUN_0052ece0((int)param_1);
  while( true ) {
    bVar1 = FUN_0052f0d0((int)param_1);
    if (bVar1) break;
    FUN_0052f5b0(param_1);
  }
  piVar2 = (int *)FUN_0052eb60((int)param_1);
  if (*piVar2 != 0) {
    FUN_0052ed00(param_1);
  }
  return;
}

