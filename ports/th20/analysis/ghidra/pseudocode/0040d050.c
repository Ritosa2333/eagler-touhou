/* Entry: 0x0040d050; symbol: _Get_size_of_n<2>; body bytes: 41 */

/* Library Function - Single Match
    unsigned int __cdecl std::_Get_size_of_n<2>(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

uint __cdecl std::_Get_size_of_n<2>(uint param_1)

{
  if (0x7fffffff < param_1) {
    _Throw_bad_array_new_length();
  }
  return param_1 << 1;
}

