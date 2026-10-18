/* Entry: 0x004f3910; symbol: FUN_004f3910; body bytes: 287 */

void __cdecl FUN_004f3910(locale *param_1)

{
  int *piVar1;
  uint uVar2;
  facet *pfVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  int *local_20;
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
  local_20 = DAT_005c60f8;
  uVar2 = FUN_004ec530((int *)&DAT_005c60f4);
  pfVar3 = std::locale::_Getfacet(param_1,uVar2);
  if ((pfVar3 == (facet *)0x0) && (local_20 == (int *)0x0)) {
    iVar4 = FUN_004fc030((int *)&local_20,param_1);
    piVar1 = local_20;
    if (iVar4 == -1) {
      FUN_004fccf0();
    }
    else {
      FUN_0040c080(local_18,4);
      FID_conflict__Unique_ptr_base<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_><class_std::_Facet_base*>
                (local_18);
      local_8._0_1_ = 1;
      FUN_00541af1(piVar1);
      (**(code **)(*piVar1 + 4))();
      DAT_005c60f8 = local_20;
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

