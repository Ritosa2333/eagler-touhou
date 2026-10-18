/* Entry: 0x00516570; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: virtual void * __thiscall std::basic_ios<char,struct std::char_traits<char> >::`scalar
   deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::basic_ios<unsigned short,struct std::char_traits<unsigned
   short> >::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::basic_ios<wchar_t,struct std::char_traits<wchar_t>
   >::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

ios_base * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_00516010((ios_base *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (ios_base *)this;
}

