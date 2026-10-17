/* Entry: 0x0040f8b0; symbol: FUN_0040f8b0; body bytes: 172 */

void __cdecl
FUN_0040f8b0(uint *param_1,undefined4 *param_2,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_3)

{
  int *unaff_FS_OFFSET;
  int local_a0 [24];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_40 [24];
  undefined4 local_28;
  undefined4 local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056797d;
  local_1c = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_28 = *param_2;
  local_24 = param_2[1];
  FUN_0040db70(local_40,param_1);
  local_14 = 0;
  FUN_0040e310(local_a0,local_40,param_3,local_28,local_24);
  FUN_00544a7c(local_a0,&DAT_005a9714);
  local_14 = 0xffffffff;
  FUN_0040e5a0(local_40);
  *unaff_FS_OFFSET = local_1c;
  return;
}

