/* Entry: 0x004147b0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall std::function<bool __cdecl(unsigned int const &)>::`scalar deleting
   destructor'(unsigned int)
    public: void * __thiscall std::function<bool __cdecl(enum Concurrency::agent_status const
   &)>::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_00414400(this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

