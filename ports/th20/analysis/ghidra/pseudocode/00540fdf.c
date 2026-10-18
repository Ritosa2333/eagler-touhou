/* Entry: 0x00540fdf; symbol: _Syserror_map; body bytes: 37 */

/* Library Function - Single Match
    char const * __cdecl std::_Syserror_map(int)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

char * __cdecl std::_Syserror_map(int param_1)

{
  int *piVar1;
  
  piVar1 = &DAT_005771c8;
  do {
    if (*piVar1 == param_1) {
      return (char *)piVar1[1];
    }
    piVar1 = piVar1 + 2;
  } while (piVar1 != (int *)"success");
  return "unknown error";
}

