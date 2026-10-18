/* Entry: 0x0044a700; symbol: FUN_0044a700; body bytes: 1022 */

void FUN_0044a700(undefined4 param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  function<void___cdecl(void)> local_1ec [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_1d4 [24];
  function<void___cdecl(void)> local_1bc [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_1a4 [24];
  undefined4 local_18c;
  undefined4 local_188;
  int local_184;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_180;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_17c;
  void *local_178;
  default_delete<class_std::_Facet_base> *local_174;
  void *local_170;
  void *local_16c;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_168;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_164;
  void *local_160;
  default_delete<class_std::_Facet_base> *local_15c;
  void *local_158;
  void *local_154;
  int local_150;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_14a;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_149;
  int *local_148;
  uint local_144;
  uint local_140;
  uint local_13c;
  int local_138;
  int *local_134;
  function<void___cdecl(void)> local_130 [24];
  wchar_t local_118 [130];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005684dc;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00429120(local_118,L"猥");
  local_148 = (int *)FUN_00410aa0((undefined1 (*) [16])local_118,(SIZE_T *)0x0,0);
  if (local_148 != (int *)0x0) {
    local_140 = 0;
    local_13c = 0;
    local_144 = 0;
    local_150 = 0;
    *(int **)(local_138 + 0x3c) = local_148;
    FUN_0040c720(local_130,&param_1);
    local_8 = 0;
    local_160 = FUN_0044c590(local_130,local_1bc);
    local_8._0_1_ = 1;
    local_158 = local_160;
    local_154 = local_160;
    local_15c = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first(&local_149);
    local_168 = FUN_004189c0(local_160,local_1a4,local_15c);
    local_8._0_1_ = 2;
    local_164 = local_168;
    FUN_00446dc0((void *)(local_138 + 4),local_168);
    local_8._0_1_ = 1;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_1a4);
    local_8._0_1_ = 0;
    FUN_0040e6c0(local_1bc);
    local_178 = FUN_00451580(local_130,local_1ec);
    local_8._0_1_ = 3;
    local_170 = local_178;
    local_16c = local_178;
    local_174 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first(&local_14a);
    local_180 = FUN_004189c0(local_178,local_1d4,local_174);
    local_8._0_1_ = 4;
    local_17c = local_180;
    FUN_00446dc0((void *)(local_138 + 0x20),local_180);
    local_8._0_1_ = 3;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_1d4);
    local_8 = (uint)local_8._1_3_ << 8;
    FUN_0040e6c0(local_1ec);
    local_134 = local_148;
    while( true ) {
      local_140 = local_140 + 1;
      local_13c = *(ushort *)((int)local_134 + 6) + local_13c;
      local_144 = *(ushort *)(local_134 + 1) + local_144;
      if (local_134[9] == 0) break;
      local_134 = (int *)((int)local_134 + local_134[9]);
    }
    *(uint *)(local_138 + 0x44) = local_140;
    uVar1 = FUN_004475f0("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\sprtlib.cpp:905 SprtGroupTexInf"
                         ,local_140);
    *(undefined4 *)(local_138 + 0x58) = uVar1;
    uVar1 = FUN_00447520("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\sprtlib.cpp:906 SprtDataInf"
                         ,local_144);
    *(undefined4 *)(local_138 + 0x50) = uVar1;
    uVar1 = FUN_00447480("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\sprtlib.cpp:907 int*",
                         local_13c);
    *(undefined4 *)(local_138 + 0x54) = uVar1;
    *(uint *)(local_138 + 0x48) = local_13c;
    *(uint *)(local_138 + 0x4c) = local_144;
    local_134 = local_148;
    while (local_184 = FUN_0044ebf0(local_138,local_150,local_134), -1 < local_184) {
      local_150 = local_150 + 1;
      if (local_134[9] == 0) {
        uVar1 = FUN_004476c0("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\sprtlib.cpp:929 SprtInf"
                             ,local_13c);
        *(undefined4 *)(local_138 + 0x40) = uVar1;
        local_18c = 0;
        local_8 = 0xffffffff;
        FUN_0040e6c0(local_130);
        goto LAB_0044aae3;
      }
      local_134 = (int *)((int)local_134 + local_134[9]);
    }
    local_188 = 0;
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_130);
  }
LAB_0044aae3:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

