/* Entry: 0x004140c0; symbol: shared_ptr<struct__EXCEPTION_RECORD_const_>; body bytes: 46 */

/* Library Function - Single Match
    public: __thiscall std::shared_ptr<struct _EXCEPTION_RECORD const >::shared_ptr<struct
   _EXCEPTION_RECORD const >(class std::shared_ptr<struct _EXCEPTION_RECORD const > &&)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

shared_ptr<struct__EXCEPTION_RECORD_const_> * __thiscall
std::shared_ptr<struct__EXCEPTION_RECORD_const_>::shared_ptr<struct__EXCEPTION_RECORD_const_>
          (shared_ptr<struct__EXCEPTION_RECORD_const_> *this,
          shared_ptr<struct__EXCEPTION_RECORD_const_> *param_1)

{
  void *pvVar1;
  
  _Func_class<>(this);
  pvVar1 = (void *)move<>(param_1);
  FUN_00414be0(this,pvVar1);
  return this;
}

