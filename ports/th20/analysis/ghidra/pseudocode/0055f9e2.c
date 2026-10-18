/* Entry: 0x0055f9e2; symbol: ___acrt_lowio_destroy_handle_array; body bytes: 53 */

/* Library Function - Single Match
    ___acrt_lowio_destroy_handle_array
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___acrt_lowio_destroy_handle_array(LPCRITICAL_SECTION param_1)

{
  LPCRITICAL_SECTION lpCriticalSection;
  
  if (param_1 != (LPCRITICAL_SECTION)0x0) {
    lpCriticalSection = param_1;
    if (param_1 != (LPCRITICAL_SECTION)&param_1[0x95].RecursionCount) {
      do {
        DeleteCriticalSection(lpCriticalSection);
        lpCriticalSection = (LPCRITICAL_SECTION)&lpCriticalSection[2].RecursionCount;
      } while (lpCriticalSection != (LPCRITICAL_SECTION)&param_1[0x95].RecursionCount);
    }
    FUN_00557ba0(param_1);
  }
  return;
}

