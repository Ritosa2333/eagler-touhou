/* Entry: 0x004d8df0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 68 */

/* Library Function - Multiple Matches With Different Base Names
    protected: virtual void * __thiscall std::collate<char>::`scalar deleting destructor'(unsigned
   int)
    protected: virtual void * __thiscall std::collate<unsigned short>::`scalar deleting
   destructor'(unsigned int)
    protected: virtual void * __thiscall std::collate<wchar_t>::`scalar deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

Image * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_004d8d50((Image *)this);
  if ((param_1 & 1) != 0) {
    if ((param_1 & 4) == 0) {
      FUN_004d8de0(this);
    }
    else {
      std::_Adl_verify_range<char*,char_const*>((char **)this,(char **)0x10);
    }
  }
  return (Image *)this;
}

