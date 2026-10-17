/* Entry: 0x005424a3; symbol: __Mtxlock; body bytes: 14 */

/* Library Function - Single Match
    __Mtxlock
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __Mtxlock(_Rmtx *param_1)

{
  EnterCriticalSection(param_1);
  return;
}

