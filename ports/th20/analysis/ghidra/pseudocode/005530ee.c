/* Entry: 0x005530ee; symbol: ___acrt_allocate_buffer_for_argv; body bytes: 79 */

/* Library Function - Single Match
    ___acrt_allocate_buffer_for_argv
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

LPVOID __cdecl ___acrt_allocate_buffer_for_argv(uint param_1,uint param_2,uint param_3)

{
  LPVOID pvVar1;
  
  if ((param_1 < 0x3fffffff) && (param_2 < (uint)(0xffffffff / (ulonglong)param_3))) {
    if (param_2 * param_3 < ~(param_1 * 4)) {
      pvVar1 = FUN_00557b40(param_1 * 4 + param_2 * param_3,1);
      FUN_00557ba0((LPVOID)0x0);
      return pvVar1;
    }
  }
  return (LPVOID)0x0;
}

