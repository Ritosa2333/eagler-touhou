/* Entry: 0x00515f90; symbol: FUN_00515f90; body bytes: 124 */

undefined4 * FUN_00515f90(undefined4 param_1,undefined4 param_2,uint *param_3)

{
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_2c [24];
  undefined4 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ac6d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040db70(local_2c,param_3);
  local_8 = 0;
  FUN_0040df50(param_1,param_2,local_2c);
  local_8 = 0xffffffff;
  FUN_0040e5a0(local_2c);
  *local_14 = &PTR_FID_conflict__scalar_deleting_destructor__0056c4f8;
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

