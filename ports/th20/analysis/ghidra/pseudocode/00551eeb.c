/* Entry: 0x00551eeb; symbol: ___acrt_unlock; body bytes: 23 */

/* Library Function - Single Match
    ___acrt_unlock
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___acrt_unlock(int param_1)

{
  LeaveCriticalSection((LPCRITICAL_SECTION)(&DAT_005e5320 + param_1 * 0x18));
  return;
}

