/* Entry: 0x0046eaa0; symbol: Check_offset; body bytes: 30 */

/* Library Function - Multiple Matches With Same Base Name
    public: void __thiscall std::_String_val<struct std::_Simple_types<char>
   >::_Check_offset(unsigned int)const 
    public: void __thiscall std::_String_val<struct std::_Simple_types<unsigned short>
   >::_Check_offset(unsigned int)const 
    public: void __thiscall std::_String_val<struct std::_Simple_types<wchar_t>
   >::_Check_offset(unsigned int)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall Check_offset(void *this,uint param_1)

{
  if (*(uint *)((int)this + 4) < param_1) {
    Xran();
  }
  return;
}

