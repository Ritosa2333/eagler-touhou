/* Entry: 0x0040fc70; symbol: append; body bytes: 140 */

/* Library Function - Single Match
    public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char>
   >::append(char const * const,unsigned int)
   
   Library: Visual Studio 2019 Release */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          char *param_1,uint param_2)

{
  int iVar1;
  undefined4 *puVar2;
  byte local_6;
  undefined1 local_5;
  
  iVar1 = *(int *)(this + 0x10);
  if ((uint)(*(int *)(this + 0x14) - iVar1) < param_2) {
    this = (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)
           FUN_0040d240(this,param_2,(uint)local_6,(uint *)param_1,param_2);
  }
  else {
    *(uint *)(this + 0x10) = iVar1 + param_2;
    puVar2 = Myptr((undefined4 *)this);
    FUN_00411480((uint *)((int)puVar2 + iVar1),(uint *)param_1,(undefined1 *)param_2);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)(iVar1 + param_2 + (int)puVar2),&local_5);
  }
  return this;
}

