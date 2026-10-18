/* Entry: 0x0046a780; symbol: FUN_0046a780; body bytes: 167 */

void * FUN_0046a780(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *param_1)

{
  int *unaff_FS_OFFSET;
  undefined4 local_2c;
  uint *local_28;
  uint local_24;
  void *local_20;
  undefined4 *local_1c;
  void *local_18;
  byte local_12;
  byte local_11;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568cfd;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040f160(param_1);
  local_1c = FUN_00470a40(&local_2c);
  local_11 = local_12;
  FUN_00467fa0(local_20,(uint)local_12,local_1c);
  local_8 = 0;
  local_24 = *(uint *)(param_1 + 0x14);
  local_28 = Myptr((undefined4 *)(param_1 + 4));
  FUN_00468180(local_18,local_28,local_24);
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

