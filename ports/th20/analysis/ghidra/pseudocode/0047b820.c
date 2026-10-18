/* Entry: 0x0047b820; symbol: shared_ptr<struct__EXCEPTION_RECORD_const_>; body bytes: 46 */

/* Library Function - Single Match
    public: __thiscall std::shared_ptr<struct _EXCEPTION_RECORD const >::shared_ptr<struct
   _EXCEPTION_RECORD const >(class std::shared_ptr<struct _EXCEPTION_RECORD const > &&)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

shared_ptr<struct__EXCEPTION_RECORD_const_> * __thiscall
std::shared_ptr<struct__EXCEPTION_RECORD_const_>::shared_ptr<struct__EXCEPTION_RECORD_const_>
          (shared_ptr<struct__EXCEPTION_RECORD_const_> *this,
          shared_ptr<struct__EXCEPTION_RECORD_const_> *param_1)

{
  _Ptr_base<struct__EXCEPTION_RECORD_const_> *p_Var1;
  
  _Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)this);
  p_Var1 = (_Ptr_base<struct__EXCEPTION_RECORD_const_> *)move<>(param_1);
  _Ptr_base<struct__EXCEPTION_RECORD_const_>::_Move_construct_from<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)this,p_Var1);
  return this;
}

