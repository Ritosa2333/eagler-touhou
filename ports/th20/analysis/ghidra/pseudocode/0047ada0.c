/* Entry: 0x0047ada0; symbol: _Move_construct_from<struct__EXCEPTION_RECORD_const_>; body bytes: 54 */

/* Library Function - Single Match
    protected: void __thiscall std::_Ptr_base<struct _EXCEPTION_RECORD const
   >::_Move_construct_from<struct _EXCEPTION_RECORD const >(class std::_Ptr_base<struct
   _EXCEPTION_RECORD const > &&)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall
std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::
_Move_construct_from<struct__EXCEPTION_RECORD_const_>
          (_Ptr_base<struct__EXCEPTION_RECORD_const_> *this,
          _Ptr_base<struct__EXCEPTION_RECORD_const_> *param_1)

{
  *(undefined4 *)this = *(undefined4 *)param_1;
  *(undefined4 *)(this + 4) = *(undefined4 *)(param_1 + 4);
  *(undefined4 *)param_1 = 0;
  *(undefined4 *)(param_1 + 4) = 0;
  return;
}

