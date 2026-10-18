/* Entry: 0x00411280; symbol: FUN_00411280; body bytes: 116 */

void __fastcall
FUN_00411280(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  uint *puVar1;
  uint local_20;
  uint local_1c;
  uint local_18;
  uint local_14;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_1;
  FUN_0040f160(param_1);
  local_c = FUN_004111f0();
  local_14 = 8;
  puVar1 = _Max_value<>(&local_c,&local_14);
  local_18 = *puVar1;
  local_1c = local_18 - 1;
  local_20 = FUN_0040d0e0();
  _Min_value<>(&local_20,&local_1c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

