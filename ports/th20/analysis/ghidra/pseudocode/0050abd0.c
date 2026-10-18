/* Entry: 0x0050abd0; symbol: FUN_0050abd0; body bytes: 154 */

void FUN_0050abd0(void)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_18 [4];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005691c0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_18,4);
  FID_conflict__Unique_ptr_base<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_><class_std::_Facet_base*>
            (local_18);
  uVar1 = FUN_0040c300((undefined4 *)local_18);
  uVar1 = move<>(uVar1);
  puVar2 = (undefined4 *)move<>(uVar1);
  FUN_0050aea0(puVar2);
  FUN_00540bdb();
  std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::
  ~unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
            ((unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
              *)local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

