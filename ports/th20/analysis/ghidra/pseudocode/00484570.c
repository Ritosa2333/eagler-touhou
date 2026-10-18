/* Entry: 0x00484570; symbol: Decref; body bytes: 32 */

/* Library Function - Multiple Matches With Same Base Name
    public: void __thiscall std::_Ptr_base<struct
   Concurrency::details::_Task_completion_event_impl<unsigned char> >::_Decref(void)
    public: void __thiscall std::_Ptr_base<struct
   Concurrency::details::_Task_completion_event_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > >::_Decref(void)
    public: void __thiscall std::_Ptr_base<struct Concurrency::details::_Task_impl<unsigned char>
   >::_Decref(void)
    public: void __thiscall std::_Ptr_base<struct Concurrency::details::_Task_impl<struct
   std::pair<unsigned char,class Concurrency::details::_CancellationTokenState *> > >::_Decref(void)
     8 names - too many to list
   
   Libraries: Visual Studio 2010 Debug, Visual Studio 2012 Debug, Visual Studio 2012 Release */

void __fastcall Decref(int param_1)

{
  if (*(int *)(param_1 + 4) != 0) {
    Decref(*(int **)(param_1 + 4));
  }
  return;
}

