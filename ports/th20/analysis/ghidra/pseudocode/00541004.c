/* Entry: 0x00541004; symbol: _Winerror_map; body bytes: 34 */

/* Library Function - Single Match
    int __cdecl std::_Winerror_map(int)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

int __cdecl std::_Winerror_map(int param_1)

{
  int *piVar1;
  
  piVar1 = &DAT_00576f28;
  do {
    if (*piVar1 == param_1) {
      return piVar1[1];
    }
    piVar1 = piVar1 + 2;
  } while (piVar1 != &DAT_005771c8);
  return 0;
}

