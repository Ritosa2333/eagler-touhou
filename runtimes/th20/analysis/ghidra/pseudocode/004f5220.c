/* Entry: 0x004f5220; symbol: ~_Tidy_guard<>; body bytes: 30 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall std::_Tidy_guard<class std::numpunct<char> >::~_Tidy_guard<class
   std::numpunct<char> >(void)
    public: __thiscall std::_Tidy_guard<class std::numpunct<unsigned short> >::~_Tidy_guard<class
   std::numpunct<unsigned short> >(void)
    public: __thiscall std::_Tidy_guard<class std::numpunct<wchar_t> >::~_Tidy_guard<class
   std::numpunct<wchar_t> >(void)
    public: __thiscall std::_Tidy_guard<class std::time_get<char,class
   std::istreambuf_iterator<char,struct std::char_traits<char> > > >::~_Tidy_guard<class
   std::time_get<char,class std::istreambuf_iterator<char,struct std::char_traits<char> > > >(void)
     6 names - too many to list
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __fastcall ~_Tidy_guard<>(int *param_1)

{
  if (*param_1 != 0) {
    Tidy(*param_1);
  }
  return;
}

