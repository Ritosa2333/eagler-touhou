/* Entry: 0x005156d0; symbol: FUN_005156d0; body bytes: 287 */

void __cdecl FUN_005156d0(locale *param_1)

{
  uint uVar1;
  facet *pfVar2;
  int *unaff_FS_OFFSET;
  facet *local_20;
  _Lockit local_1c [4];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_18 [4];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a3d5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Lockit::_Lockit(local_1c,0);
  local_8 = 0;
  local_20 = DAT_005c611c;
  uVar1 = FUN_004ec530((int *)&DAT_005e4e18);
  pfVar2 = std::locale::_Getfacet(param_1,uVar1);
  if ((pfVar2 == (facet *)0x0) && (local_20 == (facet *)0x0)) {
    uVar1 = std::ctype<char>::_Getcat(&local_20,param_1);
    pfVar2 = local_20;
    if (uVar1 == 0xffffffff) {
      FUN_004fccf0();
    }
    else {
      FUN_0040c080(local_18,4);
      FID_conflict__Unique_ptr_base<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_><class_std::_Facet_base*>
                (local_18);
      local_8._0_1_ = 1;
      FUN_00541af1(pfVar2);
      (**(code **)(*(int *)pfVar2 + 4))();
      DAT_005c611c = local_20;
      std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::
      release((unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
               *)local_18);
      local_8 = (uint)local_8._1_3_ << 8;
      std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::
      ~unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
                ((unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
                  *)local_18);
    }
  }
  local_8 = 0xffffffff;
  FUN_005419ac((int *)local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

