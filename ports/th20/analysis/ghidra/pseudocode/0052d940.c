/* Entry: 0x0052d940; symbol: _String_const_iterator<>; body bytes: 46 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<char> > >::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<char> > >(char const *,struct std::_Container_base12 const *)
    public: __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<unsigned short> > >::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<unsigned short> > >(unsigned short const *,struct std::_Container_base12 const
   *)
    public: __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<wchar_t> > >::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<wchar_t> > >(wchar_t const *,struct std::_Container_base12 const *)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

_Ptr_base<struct__EXCEPTION_RECORD_const_> * __thiscall
_String_const_iterator<>(void *this,undefined4 param_1,undefined4 *param_2)

{
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)this);
  *(undefined4 *)((int)this + 8) = param_1;
  FUN_0052edf0(this,param_2);
  return (_Ptr_base<struct__EXCEPTION_RECORD_const_> *)this;
}

