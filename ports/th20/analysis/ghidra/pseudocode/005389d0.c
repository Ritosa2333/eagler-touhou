/* Entry: 0x005389d0; symbol: ~basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>; body bytes: 80 */

/* Library Function - Single Match
    public: virtual __thiscall std::basic_stringbuf<char,struct std::char_traits<char>,class
   std::allocator<char> >::~basic_stringbuf<char,struct std::char_traits<char>,class
   std::allocator<char> >(void)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release */

void __thiscall
std::basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
~basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>
          (basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *(undefined ***)this = &PTR_FUN_00576774;
  FUN_00538ab0((int)this);
  FUN_00538a30((undefined4 *)this);
  *unaff_FS_OFFSET = local_10;
  return;
}

