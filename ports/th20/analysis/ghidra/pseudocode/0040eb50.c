/* Entry: 0x0040eb50; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: virtual void * __thiscall `void __cdecl Concurrency::wait(unsigned
   int)'::`7'::TimerObj::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::_System_error::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::ios_base::failure::`scalar deleting destructor'(unsigned
   int)
    public: virtual void * __thiscall std::future_error::`scalar deleting destructor'(unsigned int)
     5 names - too many to list
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

exception * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_0040e6e0((exception *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (exception *)this;
}

