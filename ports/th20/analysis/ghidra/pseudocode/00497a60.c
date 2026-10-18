/* Entry: 0x00497a60; symbol: FID_conflict:_Assign_rv; body bytes: 88 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall std::vector<class std::shared_ptr<struct
   Concurrency::details::_Task_impl<unsigned char> >,class std::allocator<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<unsigned char> > > >::_Assign_rv(class
   std::vector<class std::shared_ptr<struct Concurrency::details::_Task_impl<unsigned char> >,class
   std::allocator<class std::shared_ptr<struct Concurrency::details::_Task_impl<unsigned char> > > >
   &&)
    public: void __thiscall std::vector<class std::shared_ptr<struct
   Concurrency::details::_Task_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > >,class std::allocator<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > > > >::_Assign_rv(class std::vector<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > >,class std::allocator<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > > > > &&)
   
   Library: Visual Studio 2012 Release */

void __thiscall FID_conflict__Assign_rv(void *this,undefined4 *param_1)

{
  FUN_0040ec90();
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = param_1[1];
  *(undefined4 *)((int)this + 8) = param_1[2];
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  return;
}

