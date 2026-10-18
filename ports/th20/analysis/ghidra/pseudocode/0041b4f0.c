/* Entry: 0x0041b4f0; symbol: FUN_0041b4f0; body bytes: 902 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */

void FUN_0041b4f0(void)

{
  int *unaff_FS_OFFSET;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_20a4 [24];
  function<void___cdecl(void)> local_208c [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_2074 [24];
  undefined4 local_205c;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_2058;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_2054;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_2050;
  int local_204c;
  char **local_2048;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_2044 [24];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_202c [24];
  WCHAR local_2014 [4096];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567ffb;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  SystemParametersInfoW(0x10,0,&DAT_005b87e1,0);
  SystemParametersInfoW(0x53,0,&DAT_005b87e2,0);
  SystemParametersInfoW(0x54,0,&DAT_005b87e3,0);
  SystemParametersInfoW(0x11,0,(PVOID)0x0,2);
  SystemParametersInfoW(0x55,0,(PVOID)0x0,2);
  SystemParametersInfoW(0x56,0,(PVOID)0x0,2);
  QueryPerformanceFrequency((LARGE_INTEGER *)&DAT_005b67d0);
  QueryPerformanceCounter((LARGE_INTEGER *)&DAT_005b67d8);
  FUN_0040dc60(local_2044);
  local_8 = 0;
  FUN_0040e450(local_202c);
  local_8 = CONCAT31(local_8._1_3_,1);
  GetEnvironmentVariableW(L"APPDATA",local_2014,0x1000);
  WideCharToMultiByte(0x3a4,0,local_2014,-1,(LPSTR)(local_204c + 0x89),0x104,(LPCSTR)0x0,(LPBOOL)0x0
                     );
  if (local_2014[0] != L'\0') {
    FUN_00417df0(&DAT_005b67e1);
    FUN_00417e40("ShanghaiAlice");
    FUN_0041c2e0(local_202c);
    FUN_00417e40(&DAT_0056ca9c);
    FUN_0041c2e0(local_202c);
    local_2050 = FUN_0041e790(local_202c,local_2074);
    FUN_00418eb0(local_2044,
                 (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_2050);
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_2074);
    local_2048 = (char **)FUN_0040ff30((undefined4 *)local_2044);
    FUN_00548610((char *)(local_204c + 0x89),0x104,(int)local_2048);
    FUN_00417df0(&DAT_005b67e1);
    FUN_00417e40("replay");
    FUN_0041c2e0(local_202c);
    FUN_00417df0(&DAT_005b67e1);
    FUN_00417e40("snapshot");
    FUN_0041c2e0(local_202c);
    std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056cab8,local_2048);
  }
  GetModuleFileNameW((HMODULE)0x0,local_2014,0x1000);
  WideCharToMultiByte(0x3a4,0,local_2014,-1,(LPSTR)(local_204c + 0x1089),0x104,(LPCSTR)0x0,
                      (LPBOOL)0x0);
  FUN_00417df0(&DAT_005b77e1);
  local_2054 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_0041d2f0(local_202c,local_208c);
  FUN_00419010(local_202c,local_2054);
  FUN_0040e6c0(local_208c);
  local_2058 = FUN_0041e790(local_202c,local_20a4);
  FUN_00418eb0(local_2044,
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_2058);
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_20a4);
  local_2048 = (char **)FUN_0040ff30((undefined4 *)local_2044);
  FUN_00548610((char *)(local_204c + 0x1089),0x104,(int)local_2048);
  local_205c = 0;
  local_8 = local_8 & 0xffffff00;
  FUN_0040e6c0((function<void___cdecl(void)> *)local_202c);
  local_8 = 0xffffffff;
  FUN_0040e5a0(local_2044);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

