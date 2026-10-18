/* Entry: 0x0046e9e0; symbol: FUN_0046e9e0; body bytes: 136 */

void __fastcall
FUN_0046e9e0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  int local_10;
  undefined1 local_a;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_9 = 0;
  FUN_0040de90(&local_a);
  FUN_0040f160(param_1);
  local_10 = allocate(1);
  FUN_0040cd70(local_10,&local_10);
  FUN_0040cd70(local_10 + 4,&local_10);
  *(int *)param_1 = local_10;
  FUN_0040e5e0();
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

