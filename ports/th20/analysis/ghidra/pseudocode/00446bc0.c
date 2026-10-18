/* Entry: 0x00446bc0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    protected: virtual void * __thiscall Concurrency::details::_Chore::`scalar deleting
   destructor'(unsigned int)
    public: virtual void * __thiscall std::error_category::`scalar deleting destructor'(unsigned
   int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

undefined4 * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_00446ba0((undefined4 *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (undefined4 *)this;
}

