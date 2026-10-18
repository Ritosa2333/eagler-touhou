/* Entry: 0x00484750; symbol: Swap; body bytes: 51 */

/* Library Function - Multiple Matches With Same Base Name
    protected: void __thiscall std::_Ptr_base<struct _EXCEPTION_RECORD const >::_Swap(class
   std::_Ptr_base<struct _EXCEPTION_RECORD const > &)
    protected: void __thiscall std::_Ptr_base<class __ExceptionPtr>::_Swap(class
   std::_Ptr_base<class __ExceptionPtr> &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall Swap(void *this,undefined4 *param_1)

{
  FUN_0040b5d0((undefined4 *)this,param_1);
  FUN_0040b5d0((undefined4 *)((int)this + 4),param_1 + 1);
  return;
}

