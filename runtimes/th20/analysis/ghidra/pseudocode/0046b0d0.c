/* Entry: 0x0046b0d0; symbol: FUN_0046b0d0; body bytes: 88 */

void __fastcall FUN_0046b0d0(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00411ce0(param_1);
  FUN_00449240((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x5f8));
  FUN_00449370(param_1 + 0x14);
  *unaff_FS_OFFSET = local_10;
  return;
}

