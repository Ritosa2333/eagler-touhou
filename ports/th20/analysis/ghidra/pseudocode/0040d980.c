/* Entry: 0x0040d980; symbol: _Min_value<>; body bytes: 45 */

/* Library Function - Multiple Matches With Same Base Name
    unsigned int const & __cdecl std::_Min_value<unsigned int>(unsigned int const &,unsigned int
   const &)
    unsigned long const & __cdecl std::_Min_value<unsigned long>(unsigned long const &,unsigned long
   const &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

uint * __cdecl _Min_value<>(uint *param_1,uint *param_2)

{
  undefined4 local_8;
  
  if (*param_2 < *param_1) {
    local_8 = param_2;
  }
  else {
    local_8 = param_1;
  }
  return local_8;
}

