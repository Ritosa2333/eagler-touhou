/* Entry: 0x004845f0; symbol: FUN_004845f0; body bytes: 123 */

void __fastcall FUN_004845f0(_Container_proxy *param_1)

{
  uint uVar1;
  int *unaff_FS_OFFSET;
  allocator<struct_std::_Container_proxy> local_15;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005691c0;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = uVar1;
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)(param_1 + 0xc));
  FUN_0040b120(&local_15);
  (**(code **)(*(int *)param_1 + 8))(0,uVar1);
  std::_Deallocate_plain<class_std::allocator<struct_std::_Container_proxy>_>(&local_15,param_1);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

