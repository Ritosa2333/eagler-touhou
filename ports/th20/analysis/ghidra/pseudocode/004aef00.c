/* Entry: 0x004aef00; symbol: FUN_004aef00; body bytes: 138 */

undefined4 * __fastcall FUN_004aef00(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 999;
  param_1[3] = 0;
  FUN_004141c0(param_1 + 4);
  FUN_004aeee0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 8));
  FUN_004aeee0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0xd));
  param_1[0x12] = 1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

