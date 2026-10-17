/* Entry: 0x0055f0b7; symbol: find_end_of_double_null_terminated_sequence; body bytes: 55 */

/* Library Function - Single Match
    wchar_t const * __cdecl find_end_of_double_null_terminated_sequence(wchar_t const * const)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

wchar_t * __cdecl find_end_of_double_null_terminated_sequence(wchar_t *param_1)

{
  wchar_t wVar1;
  wchar_t *pwVar2;
  
  wVar1 = *param_1;
  while (wVar1 != L'\0') {
    pwVar2 = param_1;
    do {
      wVar1 = *pwVar2;
      pwVar2 = pwVar2 + 1;
    } while (wVar1 != L'\0');
    param_1 = param_1 + ((int)pwVar2 - (int)(param_1 + 1) >> 1) + 1;
    wVar1 = *param_1;
  }
  return param_1 + 1;
}

