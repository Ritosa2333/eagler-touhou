/* Entry: 0x0049c1e0; symbol: FUN_0049c1e0; body bytes: 59 */

uint __fastcall
FUN_0049c1e0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  uint *puVar1;
  uint local_10;
  uint local_c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_8;
  
  local_8 = param_1;
  FUN_0040f160(param_1);
  local_c = FUN_0049c140();
  local_10 = FUN_0040d0e0();
  puVar1 = _Min_value<>(&local_10,&local_c);
  return *puVar1;
}

