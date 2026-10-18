/* Entry: 0x005166f0; symbol: FID_conflict:`scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall std::_Func_impl<bool (__cdecl*)(enum Concurrency::agent_status const
   &),class std::allocator<int>,bool,enum Concurrency::agent_status const &>::`scalar deleting
   destructor'(unsigned int)
    public: void * __thiscall std::_Func_impl<class <lambda_0b644b0099f9cbc573e00435de85ed83>,class
   std::allocator<int>,void,class Concurrency::message<unsigned int> *>::`scalar deleting
   destructor'(unsigned int)
    public: void * __thiscall std::_Func_impl<class <lambda_4471c1faea23acf00f5de6f001106c5d>,class
   std::allocator<int>,void,class Concurrency::message<enum Concurrency::agent_status> *>::`scalar
   deleting destructor'(unsigned int)
    public: void * __thiscall std::_Func_impl<class <lambda_585d1183dd7288406f8b545e733d6ea7>,class
   std::allocator<int>,void,class Concurrency::message<unsigned int> *>::`scalar deleting
   destructor'(unsigned int)
     6 names - too many to list
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2015 Release */

undefined4 * __thiscall FID_conflict__scalar_deleting_destructor_(void *this,uint param_1)

{
  FUN_005162c0((undefined4 *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (undefined4 *)this;
}

