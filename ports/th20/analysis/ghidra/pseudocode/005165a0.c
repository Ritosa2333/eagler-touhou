/* Entry: 0x005165a0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 55 */

/* Library Function - Multiple Matches With Different Base Names
    public: virtual void * __thiscall std::basic_istream<char,struct std::char_traits<char>
   >::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::basic_istream<unsigned short,struct
   std::char_traits<unsigned short> >::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::basic_istream<wchar_t,struct std::char_traits<wchar_t>
   >::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

int __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FID_conflict__vbase_destructor_((int)this + -0x18);
  if ((param_1 & 1) != 0) {
    FUN_0054278d((LPVOID)((int)this + -0x18));
  }
  return (int)this + -0x18;
}

