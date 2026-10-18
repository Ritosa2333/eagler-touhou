/* Entry: 0x0052d910; symbol: _String_const_iterator<>; body bytes: 40 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<char> > >::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<char> > >(class std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<char> > > const &)
    public: __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<unsigned short> > >::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<unsigned short> > >(class std::_String_const_iterator<class
   std::_String_val<struct std::_Simple_types<unsigned short> > > const &)
    public: __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<wchar_t> > >::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<wchar_t> > >(class std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<wchar_t> > > const &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

_Iterator_base12 * __thiscall _String_const_iterator<>(void *this,_Iterator_base12 *param_1)

{
  std::_Iterator_base12::_Iterator_base12((_Iterator_base12 *)this,param_1);
  *(undefined4 *)((int)this + 8) = *(undefined4 *)(param_1 + 8);
  return (_Iterator_base12 *)this;
}

