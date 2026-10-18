/* Entry: 0x0045f140; symbol: _Min_value<int>; body bytes: 45 */

/* Library Function - Single Match
    int const & __cdecl std::_Min_value<int>(int const &,int const &)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

int * __cdecl std::_Min_value<int>(int *param_1,int *param_2)

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

