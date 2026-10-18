/* Entry: 0x0040ea30; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall std::shared_ptr<struct _EXCEPTION_RECORD const >::`scalar deleting
   destructor'(unsigned int)
    public: virtual void * __thiscall std::_Generic_error_category::`scalar deleting
   destructor'(unsigned int)
    public: virtual void * __thiscall std::_Iostream_error_category::`scalar deleting
   destructor'(unsigned int)
    public: virtual void * __thiscall std::_System_error_category::`scalar deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

LPVOID __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_0040e580();
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

