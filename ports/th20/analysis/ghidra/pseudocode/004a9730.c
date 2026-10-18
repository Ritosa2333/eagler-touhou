/* Entry: 0x004a9730; symbol: FUN_004a9730; body bytes: 140 */

void __fastcall
FUN_004a9730(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  void *this;
  int local_10;
  undefined1 local_a;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_9 = 0;
  FUN_0040de90(&local_a);
  this = (void *)FUN_0040f160(param_1);
  local_10 = FUN_004aa070(this,1);
  FUN_0040cd70(local_10,&local_10);
  FUN_0040cd70(local_10 + 4,&local_10);
  *(int *)(param_1 + 4) = local_10;
  FUN_0040e5e0();
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

