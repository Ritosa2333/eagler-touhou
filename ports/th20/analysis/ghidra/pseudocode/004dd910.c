/* Entry: 0x004dd910; symbol: FUN_004dd910; body bytes: 255 */

void FUN_004dd910(void)

{
  char **ppcVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_44 [24];
  function<void___cdecl(void)> local_2c [24];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569e05;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00417cd0(local_2c,&DAT_005b67e1);
  local_8 = 0;
  FUN_00417e40("th20.cfg");
  FUN_0041e790(local_2c,local_44);
  local_8._0_1_ = 1;
  ppcVar1 = (char **)FUN_0040ff30((undefined4 *)local_44);
  iVar2 = FUN_00410e70(ppcVar1,&DAT_005c4f08,0xb0);
  if (iVar2 == 0) {
    local_8 = (uint)local_8._1_3_ << 8;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_44);
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_2c);
  }
  else {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_00571dd0);
    local_8 = (uint)local_8._1_3_ << 8;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_44);
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_2c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

