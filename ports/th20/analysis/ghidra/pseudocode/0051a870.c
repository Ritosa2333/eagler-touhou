/* Entry: 0x0051a870; symbol: _Getstate; body bytes: 86 */

/* Library Function - Single Match
    private: static int __cdecl std::basic_stringbuf<char,struct std::char_traits<char>,class
   std::allocator<char> >::_Getstate(int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

int __cdecl
std::basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>::_Getstate
          (int param_1)

{
  undefined4 local_8;
  
  local_8 = 0;
  if ((param_1 & 1U) == 0) {
    local_8 = 4;
  }
  if ((param_1 & 2U) == 0) {
    local_8 = local_8 | 2;
  }
  if ((param_1 & 8U) != 0) {
    local_8 = local_8 | 8;
  }
  if ((param_1 & 4U) != 0) {
    local_8 = local_8 | 0x10;
  }
  return local_8;
}

