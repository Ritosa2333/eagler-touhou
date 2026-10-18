/* Entry: 0x0052ddf0; symbol: operator-; body bytes: 37 */

/* Library Function - Single Match
    public: int __thiscall std::_String_const_iterator<class std::_String_val<struct
   std::_Simple_types<char> > >::operator-(class std::_String_const_iterator<class
   std::_String_val<struct std::_Simple_types<char> > > const &)const 
   
   Libraries: Visual Studio 2012 Debug, Visual Studio 2012 Release, Visual Studio 2015 Debug, Visual
   Studio 2015 Release */

int __thiscall
std::_String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_>::operator-
          (_String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_> *this,
          _String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_> *param_1
          )

{
  FUN_0040ec90();
  return *(int *)(this + 8) - *(int *)(param_1 + 8);
}

