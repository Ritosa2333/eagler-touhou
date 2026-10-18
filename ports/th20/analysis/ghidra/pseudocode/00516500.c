/* Entry: 0x00516500; symbol: FID_conflict:`vbase_destructor'; body bytes: 34 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall std::basic_istream<char,struct std::char_traits<char> >::`vbase
   destructor'(void)
    public: void __thiscall std::basic_istream<unsigned short,struct std::char_traits<unsigned
   short> >::`vbase destructor'(void)
    public: void __thiscall std::basic_istream<wchar_t,struct std::char_traits<wchar_t> >::`vbase
   destructor'(void)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __fastcall FID_conflict__vbase_destructor_(int param_1)

{
  std::basic_istream<char,struct_std::char_traits<char>_>::
  ~basic_istream<char,struct_std::char_traits<char>_>
            ((basic_istream<char,struct_std::char_traits<char>_> *)(param_1 + 0x18));
  FUN_00516010((ios_base *)(param_1 + 0x18));
  return;
}

