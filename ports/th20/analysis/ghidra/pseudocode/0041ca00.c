/* Entry: 0x0041ca00; symbol: FID_conflict:erase; body bytes: 48 */

/* Library Function - Multiple Matches With Different Base Names
    public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char>
   >::erase(unsigned int)
    public: class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class
   std::allocator<unsigned short> > & __thiscall std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> >::erase(unsigned int)
    public: class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class
   std::allocator<wchar_t> > & __thiscall std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t> >::erase(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void * __thiscall FID_conflict_erase(void *this,uint param_1)

{
  Check_offset(this,param_1);
  FUN_0040f0c0(this,param_1);
  return this;
}

