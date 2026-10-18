/* Entry: 0x0040df50; symbol: FUN_0040df50; body bytes: 165 */

undefined4 *
FUN_0040df50(undefined4 param_1,undefined4 param_2,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_3)

{
  int *unaff_FS_OFFSET;
  undefined1 auStack_54 [16];
  undefined4 uStack_44;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_38 [24];
  undefined1 *local_20;
  undefined4 *local_1c;
  undefined4 *local_18;
  undefined4 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005677dd;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_20 = auStack_54;
  FUN_0040da50(auStack_54,param_3);
  local_1c = (undefined4 *)FUN_0040f200(local_38);
  local_8 = 0;
  uStack_44 = 0x40dfb9;
  local_18 = local_1c;
  FUN_0040e4a0(local_14,local_1c);
  local_8 = 0xffffffff;
  FUN_0040e5a0(local_38);
  *local_14 = &PTR_FID_conflict__scalar_deleting_destructor__0056c4e8;
  local_14[3] = param_1;
  local_14[4] = param_2;
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

