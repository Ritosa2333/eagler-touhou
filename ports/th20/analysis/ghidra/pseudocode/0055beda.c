/* Entry: 0x0055beda; symbol: __crt_time_elapsed_leap_years<>; body bytes: 58 */

/* Library Function - Multiple Matches With Same Base Name
    int __cdecl __crt_time_elapsed_leap_years<int>(int)
    long __cdecl __crt_time_elapsed_leap_years<long>(long)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

int __cdecl __crt_time_elapsed_leap_years<>(int param_1)

{
  int iVar1;
  
  iVar1 = param_1 + -1;
  return ((int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2) + -0x11 + ((param_1 + 299) / 400 - iVar1 / 100);
}

