/* Entry: 0x004ad990; symbol: FUN_004ad990; body bytes: 174 */

void __cdecl FUN_004ad990(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  _Facet_base *local_24;
  void *local_20;
  undefined4 local_1c;
  _Facet_base *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005691fd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_18 = (_Facet_base *)FUN_0040c770(0x28);
  local_8 = 0;
  local_20 = (void *)move<>(0x28,local_18);
  local_1c = move<>(param_1);
  FUN_004ac7f0(local_20,local_1c);
  local_24 = (_Facet_base *)0x0;
  std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>(&local_18,&local_24);
  local_8 = 0xffffffff;
  FUN_00486de0((int *)&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

