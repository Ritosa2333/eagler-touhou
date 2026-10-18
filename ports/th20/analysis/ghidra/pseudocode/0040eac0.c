/* Entry: 0x0040eac0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: virtual void * __thiscall Concurrency::details::_Interruption_exception::`scalar
   deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::__non_rtti_object::`scalar deleting destructor'(unsigned
   int)
    public: virtual void * __thiscall std::bad_alloc::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall std::bad_array_new_length::`scalar deleting
   destructor'(unsigned int)
     39 names - too many to list
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

exception * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_0040e5f0((exception *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (exception *)this;
}

