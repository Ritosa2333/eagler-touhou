/* Entry: 0x0040f2c0; symbol: Myptr; body bytes: 54 */

/* Library Function - Multiple Matches With Same Base Name
    public: unsigned short const * __thiscall std::_String_val<struct std::_Simple_types<unsigned
   short> >::_Myptr(void)const 
    public: wchar_t const * __thiscall std::_String_val<struct std::_Simple_types<wchar_t>
   >::_Myptr(void)const 
   
   Library: Visual Studio 2019 Release */

undefined4 * __fastcall Myptr(undefined4 *param_1)

{
  bool bVar1;
  undefined4 local_c;
  
  bVar1 = FUN_0040f180((int)param_1);
  local_c = param_1;
  if (bVar1) {
    local_c = (undefined4 *)move<>(*param_1);
  }
  return local_c;
}

