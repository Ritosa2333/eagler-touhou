/* Entry: 0x004f71b0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: virtual void * __thiscall Concurrency::ISource<unsigned int>::`scalar deleting
   destructor'(unsigned int)
    public: virtual void * __thiscall Concurrency::ISource<enum Concurrency::agent_status>::`scalar
   deleting destructor'(unsigned int)
    public: virtual void * __thiscall Concurrency::ITarget<unsigned int>::`scalar deleting
   destructor'(unsigned int)
    public: virtual void * __thiscall Concurrency::ITarget<enum Concurrency::agent_status>::`scalar
   deleting destructor'(unsigned int)
     11 names - too many to list
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

undefined4 * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_004f54f0((undefined4 *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (undefined4 *)this;
}

