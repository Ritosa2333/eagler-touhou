/* Entry: 0x004846e0; symbol: Decref; body bytes: 32 */

/* Library Function - Multiple Matches With Same Base Name
    protected: void __thiscall std::_Ptr_base<struct _EXCEPTION_RECORD const >::_Decref(void)
    public: void __thiscall std::_Ptr_base<struct
   Concurrency::details::_Task_completion_event_impl<unsigned char> >::_Decref(void)
    public: void __thiscall std::_Ptr_base<struct
   Concurrency::details::_Task_completion_event_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > >::_Decref(void)
    public: void __thiscall std::_Ptr_base<struct Concurrency::details::_Task_impl<unsigned char>
   >::_Decref(void)
     10 names - too many to list
   
   Library: Visual Studio */

void __fastcall Decref(int param_1)

{
  if (*(int *)(param_1 + 4) != 0) {
    FUN_00484700(*(int *)(param_1 + 4));
  }
  return;
}

