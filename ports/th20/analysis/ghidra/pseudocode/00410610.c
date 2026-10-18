/* Entry: 0x00410610; symbol: FUN_00410610; body bytes: 104 */

void __cdecl
FUN_00410610(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  undefined4 local_10 [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(local_10,8);
  FUN_0040e130(local_10);
  FUN_00410680((undefined4 *)param_1,local_10);
  bVar1 = FUN_0040e800(local_10);
  if (bVar1) {
    FUN_0040f8b0((uint *)"exists",local_10,param_1);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

