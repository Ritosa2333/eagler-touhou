/* Entry: 0x0040f200; symbol: FUN_0040f200; body bytes: 184 */

void * __cdecl FUN_0040f200(void *param_1)

{
  bool bVar1;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_34 [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_1c;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  puStack_c = &LAB_005678e5;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = 0;
  local_8 = 0;
  bVar1 = FUN_00410360((int)&stack0x00000010);
  if (!bVar1) {
    std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
              ((basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)
               &stack0x00000010,": ");
  }
  local_1c = (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)
             FUN_00411420(&stack0x00000008,local_34);
  local_8._0_1_ = 1;
  local_18 = local_1c;
  std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
            ((basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)
             &stack0x00000010,local_1c);
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0040e5a0(local_34);
  FUN_0040d9e0(param_1,(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)&stack0x00000010);
  local_14 = local_14 | 1;
  local_8 = 0xffffffff;
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&stack0x00000010);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

