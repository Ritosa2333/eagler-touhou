/* Entry: 0x0040d950; symbol: _Max_value<>; body bytes: 45 */

/* Library Function - Multiple Matches With Same Base Name
    unsigned int const & __cdecl std::_Max_value<unsigned int>(unsigned int const &,unsigned int
   const &)
    char * const & __cdecl std::_Max_value<char *>(char * const &,char * const &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

uint * __cdecl _Max_value<>(uint *param_1,uint *param_2)

{
  undefined4 local_8;
  
  if (*param_1 < *param_2) {
    local_8 = param_2;
  }
  else {
    local_8 = param_1;
  }
  return local_8;
}

