/* Entry: 0x0041bdb0; symbol: FUN_0041bdb0; body bytes: 185 */

void __cdecl
FUN_0041bdb0(uint *param_1,undefined4 param_2,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_3)

{
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  int local_a8 [24];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_48 [24];
  undefined1 local_30 [8];
  undefined4 local_28;
  undefined4 local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056803d;
  local_1c = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_1c;
  puVar1 = (undefined4 *)make_error_code(local_30,param_2);
  local_28 = *puVar1;
  local_24 = puVar1[1];
  FUN_0040db70(local_48,param_1);
  local_14 = 0;
  FUN_0040e310(local_a8,local_48,param_3,local_28,local_24);
  FUN_00544a7c(local_a8,&DAT_005a9714);
  local_14 = 0xffffffff;
  FUN_0040e5a0(local_48);
  *unaff_FS_OFFSET = local_1c;
  return;
}

