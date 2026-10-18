/* Entry: 0x00413700; symbol: _Get_size_of_n<8>; body bytes: 42 */

/* Library Function - Single Match
    unsigned int __cdecl std::_Get_size_of_n<8>(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

uint __cdecl std::_Get_size_of_n<8>(uint param_1)

{
  if (0x1fffffff < param_1) {
    _Throw_bad_array_new_length();
  }
  return param_1 << 3;
}

