/* Entry: 0x00419760; symbol: FUN_00419760; body bytes: 574 */

void FUN_00419760(void)

{
  char cVar1;
  char **ppcVar2;
  int *unaff_FS_OFFSET;
  uint local_164;
  int local_160;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_15c [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_144 [24];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_12c [24];
  wchar_t local_114 [128];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567f76;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0041bfc0(DAT_005c0028);
  if (DAT_005b88a4 == (void *)0x0) {
    local_164 = 0;
  }
  else {
    local_164 = FUN_00419c00(DAT_005b88a4,0x40000);
  }
  if ((local_164 != 0) && ((DAT_005b87e8 >> 6 & 1) == 0)) {
    FUN_00417cd0(local_12c,&DAT_005b67e1);
    local_8 = 0;
    FUN_00417e40("snapshot");
    FUN_0041c2e0(local_12c);
    for (local_160 = 0; local_160 < 10000; local_160 = local_160 + 1) {
      FID_conflict__swprintf(local_114,(wchar_t *)0x100,"th20_%.3d.png",local_160);
      FUN_00417df0(&DAT_005b67e1);
      FUN_00417e40("snapshot");
      FUN_00417e40(local_114);
      FUN_0041e790(local_12c,local_144);
      local_8._0_1_ = 1;
      FUN_0040ff30((undefined4 *)local_144);
      cVar1 = FUN_004107a0();
      if (cVar1 == '\0') {
        local_8 = (uint)local_8._1_3_ << 8;
        FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                      *)local_144);
        break;
      }
      local_8 = (uint)local_8._1_3_ << 8;
      FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_144);
    }
    if (local_160 < 10000) {
      FUN_0041e790(local_12c,local_15c);
      local_8._0_1_ = 2;
      ppcVar2 = (char **)FUN_0040ff30((undefined4 *)local_15c);
      FUN_004de040(&DAT_005c4d40,ppcVar2);
      local_8 = (uint)local_8._1_3_ << 8;
      FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_15c);
    }
    local_8 = 0xffffffff;
    FUN_0040e6c0((function<void___cdecl(void)> *)local_12c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

