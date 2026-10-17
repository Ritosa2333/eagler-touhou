/* Entry: 0x00417e40; symbol: FUN_00417e40; body bytes: 120 */

undefined4 * FUN_00417e40(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  function<void___cdecl(void)> local_38 [24];
  undefined4 *local_20;
  void *local_1c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_18;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567eed;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_18 = FUN_00417cd0(local_38,param_1);
  local_8 = 0;
  local_14 = local_18;
  local_20 = FUN_00419180(local_1c,local_18);
  local_8 = 0xffffffff;
  FUN_0040e6c0(local_38);
  *unaff_FS_OFFSET = local_10;
  return local_20;
}

