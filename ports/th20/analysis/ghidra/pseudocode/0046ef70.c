/* Entry: 0x0046ef70; symbol: Xran; body bytes: 16 */

/* Library Function - Multiple Matches With Same Base Name
    public: static void __cdecl std::_String_val<struct std::_Simple_types<char> >::_Xran(void)
    public: static void __cdecl std::_String_val<struct std::_Simple_types<unsigned short>
   >::_Xran(void)
    public: static void __cdecl std::_String_val<struct std::_Simple_types<wchar_t> >::_Xran(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void Xran(void)

{
  std::_Xout_of_range("invalid string_view position");
  return;
}

