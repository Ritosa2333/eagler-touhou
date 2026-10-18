/* Entry: 0x0054a476; symbol: ___acrt_locale_get_ctype_array_value; body bytes: 34 */

/* Library Function - Single Match
    ___acrt_locale_get_ctype_array_value
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

ushort __cdecl ___acrt_locale_get_ctype_array_value(int param_1,int param_2,ushort param_3)

{
  if (param_2 + 1U < 0x101) {
    return *(ushort *)(param_1 + param_2 * 2) & param_3;
  }
  return 0;
}

