/* Entry: 0x00551ea3; symbol: ___acrt_lock; body bytes: 23 */

/* Library Function - Single Match
    ___acrt_lock
   
   Library: Visual Studio 2019 Release */

void __cdecl ___acrt_lock(int param_1)

{
  EnterCriticalSection((LPCRITICAL_SECTION)(&DAT_005e5320 + param_1 * 0x18));
  return;
}

