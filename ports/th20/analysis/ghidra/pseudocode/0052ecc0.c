/* Entry: 0x0052ecc0; symbol: Mysize; body bytes: 22 */

/* Library Function - Multiple Matches With Same Base Name
    public: unsigned int const & __thiscall std::_String_alloc<struct
   std::_String_base_types<char,class std::allocator<char> > >::_Mysize(void)const 
    public: unsigned int const & __thiscall std::_String_alloc<struct
   std::_String_base_types<unsigned short,class std::allocator<unsigned short> >
   >::_Mysize(void)const 
    public: unsigned int const & __thiscall std::_String_alloc<struct
   std::_String_base_types<wchar_t,class std::allocator<wchar_t> > >::_Mysize(void)const 
   
   Library: Visual Studio 2015 Release */

int __fastcall Mysize(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_004fbff0(param_1);
  return iVar1 + 0x10;
}

