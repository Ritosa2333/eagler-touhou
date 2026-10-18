/* Entry: 0x00411300; symbol: message; body bytes: 55 */

/* Library Function - Single Match
    public: virtual class std::basic_string<char,struct std::char_traits<char>,class
   std::allocator<char> > __thiscall std::_Generic_error_category::message(int)const 
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

int __thiscall std::_Generic_error_category::message(_Generic_error_category *this,int param_1)

{
  uint *puVar1;
  int in_stack_00000008;
  
  puVar1 = (uint *)_Syserror_map(in_stack_00000008);
  FUN_0040db70((void *)param_1,puVar1);
  return param_1;
}

