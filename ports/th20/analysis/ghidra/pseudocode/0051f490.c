/* Entry: 0x0051f490; symbol: FUN_0051f490; body bytes: 841 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */

void FUN_0051f490(void)

{
  uint uVar1;
  undefined4 *puVar2;
  BOOL BVar3;
  int *unaff_FS_OFFSET;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_3330 [24];
  function<void___cdecl(void)> local_3318 [24];
  undefined4 *local_3300;
  void *local_32fc;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_32f8;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_32f4;
  undefined4 *local_32f0;
  void *local_32ec;
  HANDLE local_32e8;
  undefined1 (*local_32e4) [16];
  int local_32e0;
  int local_32dc;
  int local_32d8;
  _WIN32_FIND_DATAW local_32d4;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_3084 [24];
  function<void___cdecl(void)> local_306c [24];
  WCHAR local_3054 [4096];
  CHAR local_1054 [4096];
  wchar_t local_54 [32];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056af0b;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = uVar1;
  for (local_32dc = 1; local_32dc < 0x1a; local_32dc = local_32dc + 1) {
    FID_conflict__swprintf(local_54,(wchar_t *)0x40,"th20_%.2d.rpy",local_32dc,uVar1);
    local_32f0 = FUN_0050a8e0((undefined1 (*) [16])local_54);
    local_32ec = (void *)(local_32d8 + 0x5740);
    puVar2 = (undefined4 *)FUN_00414580(local_32ec,local_32dc + -1);
    *puVar2 = local_32f0;
    if ((*(uint *)(local_32d8 + 0x58d4) >> 2 & 1) != 0) break;
  }
  FUN_00417cd0(local_306c,&DAT_005b67e1);
  local_8 = 0;
  FUN_00417e40("replay");
  FUN_00417e40("th20_ud????.rpy");
  FUN_0041e790(local_306c,local_3084);
  local_8 = CONCAT31(local_8._1_3_,1);
  local_32e4 = (undefined1 (*) [16])FUN_0040ff30((undefined4 *)local_3084);
  MultiByteToWideChar(0x3a4,0,(LPCSTR)local_32e4,-1,local_3054,0x2000);
  local_32e8 = FindFirstFileW(local_3054,&local_32d4);
  if (local_32e8 != (HANDLE)0xffffffff) {
    for (local_32e0 = 0x19; local_32e0 < 0x4b; local_32e0 = local_32e0 + 1) {
      WideCharToMultiByte(0x3a4,0,local_32d4.cFileName,-1,local_1054,0x1000,(LPCSTR)0x0,(LPBOOL)0x0)
      ;
      FUN_00417df0(local_1054);
      local_32f4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)FUN_0044c590(local_306c,local_3318);
      FUN_00419010(local_306c,local_32f4);
      FUN_0040e6c0(local_3318);
      local_32f8 = FUN_0041e790(local_306c,local_3330);
      FUN_00418eb0(local_3084,
                   (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_32f8);
      FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_3330);
      local_32e4 = (undefined1 (*) [16])FUN_0040ff30((undefined4 *)local_3084);
      local_3300 = FUN_0050a8e0(local_32e4);
      local_32fc = (void *)(local_32d8 + 0x5740);
      puVar2 = (undefined4 *)FUN_00414580(local_32fc,local_32e0);
      *puVar2 = local_3300;
      if (((*(uint *)(local_32d8 + 0x58d4) >> 2 & 1) != 0) ||
         (BVar3 = FindNextFileW(local_32e8,&local_32d4), BVar3 == 0)) break;
    }
  }
  FindClose(local_32e8);
  local_8 = local_8 & 0xffffff00;
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_3084);
  local_8 = 0xffffffff;
  FUN_0040e6c0(local_306c);
  *(uint *)(local_32d8 + 0x58d4) = *(uint *)(local_32d8 + 0x58d4) | 8;
  *(uint *)(local_32d8 + 0x58d4) = *(uint *)(local_32d8 + 0x58d4) & 0xfffffffb;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

