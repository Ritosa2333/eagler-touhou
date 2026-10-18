/* Entry: 0x0052dc00; symbol: FUN_0052dc00; body bytes: 141 */

void __fastcall FUN_0052dc00(undefined4 *param_1)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR__scalar_deleting_destructor__005756dc;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown TrophyInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  DAT_005c6858 = 0;
  FUN_0052db70((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 5));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

