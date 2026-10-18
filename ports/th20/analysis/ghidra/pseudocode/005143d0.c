/* Entry: 0x005143d0; symbol: FUN_005143d0; body bytes: 158 */

basic_streambuf<char,struct_std::char_traits<char>_> * __thiscall
FUN_005143d0(void *this,int param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  uint *puVar4;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ab1d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00515900((basic_streambuf<char,struct_std::char_traits<char>_> *)this);
  local_8 = 0;
  *(undefined ***)this = &PTR__scalar_deleting_destructor__00574de0;
  uVar1 = std::basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
          _Getstate(param_2);
  uVar2 = FUN_0041ca90(param_1);
  uVar3 = move<>(uVar2);
  puVar4 = (uint *)FUN_0044baf0(param_1);
  FUN_0051aa00(this,puVar4,uVar3,uVar1);
  *unaff_FS_OFFSET = local_10;
  return (basic_streambuf<char,struct_std::char_traits<char>_> *)this;
}

