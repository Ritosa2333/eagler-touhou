/* Entry: 0x0040dec0; symbol: FUN_0040dec0; body bytes: 140 */

undefined4 * FUN_0040dec0(undefined4 param_1,undefined4 param_2)

{
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_34 [24];
  undefined4 *local_1c;
  undefined4 *local_18;
  undefined4 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005677ad;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_1c = (undefined4 *)FUN_00411420(&param_1,local_34);
  local_8 = 0;
  local_18 = local_1c;
  FUN_0040e4a0(local_14,local_1c);
  local_8 = 0xffffffff;
  FUN_0040e5a0(local_34);
  *local_14 = &PTR_FID_conflict__scalar_deleting_destructor__0056c4e8;
  local_14[3] = param_1;
  local_14[4] = param_2;
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

