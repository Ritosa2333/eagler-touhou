/* Entry: 0x004a36d0; symbol: _String_val<>; body bytes: 42 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall std::_String_val<struct std::_Simple_types<char> >::_String_val<struct
   std::_Simple_types<char> >(void)
    public: __thiscall std::_String_val<struct std::_Simple_types<unsigned short>
   >::_String_val<struct std::_Simple_types<unsigned short> >(void)
    public: __thiscall std::_String_val<struct std::_Simple_types<wchar_t> >::_String_val<struct
   std::_Simple_types<wchar_t> >(void)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 * __fastcall _String_val<>(undefined4 *param_1)

{
  FUN_004141c0(param_1);
  param_1[4] = 0;
  param_1[5] = 0;
  return param_1;
}

