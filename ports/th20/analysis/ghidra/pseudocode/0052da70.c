/* Entry: 0x0052da70; symbol: FUN_0052da70; body bytes: 190 */

undefined4 * __fastcall FUN_0052da70(undefined4 *param_1)

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
  FUN_0041fd20(param_1);
  *param_1 = &PTR__scalar_deleting_destructor__005756dc;
  param_1[4] = 0;
  FUN_0052d8c0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 5));
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  FUN_00422d90(param_1 + 0xe);
  FUN_00425cc0(param_1 + 0x12);
  FUN_00425cc0(param_1 + 0x13);
  FUN_00425cc0(param_1 + 0x14);
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize TrophyInf\n",ppcVar1);
  DAT_005c6858 = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

