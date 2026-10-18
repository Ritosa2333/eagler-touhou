/* Entry: 0x00516620; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: virtual void * __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::basic_streambuf<unsigned short,struct
   std::char_traits<unsigned short> >::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::basic_streambuf<wchar_t,struct std::char_traits<wchar_t>
   >::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

basic_streambuf<char,struct_std::char_traits<char>_> * __thiscall
FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  std::basic_streambuf<char,struct_std::char_traits<char>_>::
  ~basic_streambuf<char,struct_std::char_traits<char>_>
            ((basic_streambuf<char,struct_std::char_traits<char>_> *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (basic_streambuf<char,struct_std::char_traits<char>_> *)this;
}

