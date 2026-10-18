/* Entry: 0x0040fbb0; symbol: append; body bytes: 142 */

/* Library Function - Single Match
    public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char>
   >::append(unsigned int,char)
   
   Library: Visual Studio 2019 Release */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          uint param_1,char param_2)

{
  int iVar1;
  undefined4 *puVar2;
  byte local_6;
  undefined1 local_5;
  
  iVar1 = *(int *)(this + 0x10);
  if ((uint)(*(int *)(this + 0x14) - iVar1) < param_1) {
    this = (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)
           FUN_0040d100(this,param_1,(uint)local_6,param_1,param_2);
  }
  else {
    *(uint *)(this + 0x10) = iVar1 + param_1;
    puVar2 = Myptr((undefined4 *)this);
    FUN_0040fe50((undefined1 *)((int)puVar2 + iVar1),param_1,param_2);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)(iVar1 + param_1 + (int)puVar2),&local_5);
  }
  return this;
}

