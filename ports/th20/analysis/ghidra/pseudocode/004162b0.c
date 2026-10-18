/* Entry: 0x004162b0; symbol: FUN_004162b0; body bytes: 59 */

uint __fastcall
FUN_004162b0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  uint *puVar1;
  uint local_10;
  uint local_c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_8;
  
  local_8 = param_1;
  FUN_0040f160(param_1);
  local_c = FUN_00416260();
  local_10 = FUN_0040d0e0();
  puVar1 = _Min_value<>(&local_10,&local_c);
  return *puVar1;
}

