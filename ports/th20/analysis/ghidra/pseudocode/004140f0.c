/* Entry: 0x004140f0; symbol: function<>; body bytes: 93 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall std::function<void __cdecl(unsigned int const &)>::function<void
   __cdecl(unsigned int const &)>(class std::function<void __cdecl(unsigned int const &)> const &)
    public: __thiscall std::function<void __cdecl(class Concurrency::message<unsigned int>
   *)>::function<void __cdecl(class Concurrency::message<unsigned int> *)>(class std::function<void
   __cdecl(class Concurrency::message<unsigned int> *)> const &)
    public: __thiscall std::function<void __cdecl(class Concurrency::message<enum
   Concurrency::agent_status> *)>::function<void __cdecl(class Concurrency::message<enum
   Concurrency::agent_status> *)>(class std::function<void __cdecl(class Concurrency::message<enum
   Concurrency::agent_status> *)> const &)
    public: __thiscall std::function<void __cdecl(void)>::function<void __cdecl(void)>(class
   std::function<void __cdecl(void)> const &)
     6 names - too many to list
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall function<>(void *this,int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567c8d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Func_class<>(this);
  local_8 = 0;
  FUN_00414b90(this,param_1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

